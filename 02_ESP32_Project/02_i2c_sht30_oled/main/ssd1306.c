/**
 * ssd1306.c - SSD1306 OLED 128x64 驱动实现
 *
 * 显示组织: 128列 × 8页 × 8位/页 = 128×64 = 1024字节
 * Page0 = 最上面8行, Page7 = 最下面8行
 *
 * I2C协议:
 *   发命令: [START][0x3C][0x00][cmd_bytes...][STOP]
 *   发数据: [START][0x3C][0x40][data_bytes...][STOP]
 */

#include "ssd1306.h"
#include "ssd1306_font.h"
#include <string.h>
#include "esp_log.h"

static const char *TAG = "SSD1306";

#define SSD1306_I2C_ADDR   0x3C    // 7位I2C地址
#define SSD1306_WIDTH      128     // 像素宽度
#define SSD1306_HEIGHT     64      // 像素高度
#define SSD1306_PAGES      8       // 页数 (64÷8)

#define CTRL_CMD   0x00   // 控制字节: 后面是命令
#define CTRL_DATA  0x40   // 控制字节: 后面是显示数据

static i2c_master_dev_handle_t s_dev_handle = NULL;

// 帧缓冲区: 1024字节
static uint8_t s_framebuffer[SSD1306_WIDTH * SSD1306_PAGES];

// ---- 内部函数 ----

static esp_err_t ssd1306_write_cmd(uint8_t cmd) {
    uint8_t buf[2] = {CTRL_CMD, cmd};
    return i2c_master_transmit(s_dev_handle, buf, 2, 100);
}

static esp_err_t ssd1306_write_cmds(const uint8_t *cmds, int len) {
    // 需要先发控制字节, 再发命令序列
    // 这里逐条发送(简单可靠)
    for (int i = 0; i < len; i++) {
        esp_err_t ret = ssd1306_write_cmd(cmds[i]);
        if (ret != ESP_OK) return ret;
    }
    return ESP_OK;
}

// ---- 公开API ----

esp_err_t ssd1306_init(i2c_master_bus_handle_t bus_handle) {
    ESP_LOGI(TAG, "正在初始化 SSD1306 OLED...");

    // 挂载OLED到I2C总线
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = SSD1306_I2C_ADDR,
        .scl_speed_hz    = 400000,   // OLED可以跑400KHz快速模式
    };
    esp_err_t ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &s_dev_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "挂载OLED到I2C失败: %s", esp_err_to_name(ret));
        return ret;
    }

    // ---- 初始化命令序列 (必须严格按此顺序!) ----
    const uint8_t init_cmds[] = {
        0xAE,       // 显示关闭(休眠模式)
        0xD5, 0x80, // 时钟分频/振荡器频率
        0xA8, 0x3F, // 多路复用比 = 64 (64行)
        0xD3, 0x00, // 显示偏移 = 0
        0x40,       // 起始行 = 0
        0x8D, 0x14, // 使能电荷泵 (内部升压到7.5V驱动OLED)
        0x20, 0x00, // 内存寻址模式 = 水平模式
        0xA1,       // 段重映射 (列127映射到SEG0, 左右镜像)
        0xC8,       // COM扫描方向 (COM63→COM0, 上下翻转)
        0xDA, 0x12, // COM引脚硬件配置
        0x81, 0xCF, // 对比度 = 207
        0xD9, 0xF1, // 预充电周期
        0xDB, 0x40, // VCOMH取消选择电平
        0xA4,       // 全局显示开启(从RAM取数据)
        0xA6,       // 正常显示 (A7=反色)
        0x2E,       // 停止水平滚动
        0xAF,       // ★ 显示开启! (忘了这条=黑屏)
    };

    ret = ssd1306_write_cmds(init_cmds, sizeof(init_cmds));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "OLED初始化序列失败: %s", esp_err_to_name(ret));
        return ret;
    }

    ssd1306_clear();
    ssd1306_refresh();

    ESP_LOGI(TAG, "SSD1306 OLED 初始化成功!");
    return ESP_OK;
}

void ssd1306_clear(void) {
    memset(s_framebuffer, 0x00, sizeof(s_framebuffer));
}

void ssd1306_refresh(void) {
    // 逐页发送: 设置页地址 → 设置列地址 → 发送128字节数据
    for (int page = 0; page < SSD1306_PAGES; page++) {
        ssd1306_write_cmd(0xB0 + page);  // 设置页地址
        ssd1306_write_cmd(0x00);          // 列低4位 = 0
        ssd1306_write_cmd(0x10);          // 列高4位 = 0

        // 发送一整页数据(128字节)
        uint8_t *page_ptr = s_framebuffer + (page * SSD1306_WIDTH);
        uint8_t ctrl_byte = CTRL_DATA;
        // 使用i2c_master_transmit一次性发送控制字节+128字节数据
        // 但I2C FIFO有限制... 分批发送更可靠
        for (int col = 0; col < SSD1306_WIDTH; col += 32) {
            int chunk = (col + 32 > SSD1306_WIDTH) ? (SSD1306_WIDTH - col) : 32;
            uint8_t buf[33];  // 1 + 32
            buf[0] = CTRL_DATA;
            memcpy(buf + 1, page_ptr + col, chunk);
            i2c_master_transmit(s_dev_handle, buf, chunk + 1, 100);
        }
    }
}

void ssd1306_draw_char(uint8_t x, uint8_t y, char c) {
    if (x > SSD1306_WIDTH - 5 || y > SSD1306_PAGES - 1) return;
    if (c < 0x20 || c > 0x7E) c = '?';  // 不可打印字符

    const uint8_t *glyph = font5x7[c - 0x20];

    for (int col = 0; col < 5; col++) {
        // 检查边界
        if (x + col >= SSD1306_WIDTH) break;
        s_framebuffer[(y * SSD1306_WIDTH) + x + col] = glyph[col];
    }

    // 字符间距 (1像素空白列)
    if (x + 5 < SSD1306_WIDTH) {
        s_framebuffer[(y * SSD1306_WIDTH) + x + 5] = 0x00;
    }
}

void ssd1306_draw_string(uint8_t x, uint8_t y, const char *str) {
    uint8_t cx = x;
    while (*str) {
        if (cx > SSD1306_WIDTH - 6) {
            // 换行
            cx = 0;
            y++;
            if (y >= SSD1306_PAGES) break;
        }
        ssd1306_draw_char(cx, y, *str);
        cx += 6;  // 5像素字符 + 1像素间距
        str++;
    }
}

void ssd1306_draw_fullscreen(const uint8_t *buffer) {
    memcpy(s_framebuffer, buffer, sizeof(s_framebuffer));
}
