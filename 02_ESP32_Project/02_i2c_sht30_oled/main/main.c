/**
 * main.c - I2C驱动温湿度传感器 + OLED显示
 *
 * 韦富宏 · ESP32-S3 工业传感器项目 · Stage 2.1
 *
 * 硬件:
 *   YD-ESP32-S3 (ESP32-S3-WROOM-1)
 *   SHT30  (I2C 0x44)  → GPIO6(SCL) + GPIO7(SDA)
 *   SSD1306 (I2C 0x3C)  → 同一I2C总线
 *
 * 功能:
 *   1. 初始化I2C总线
 *   2. 初始化SHT30和SSD1306
 *   3. 显示启动画面
 *   4. 每2秒读取温湿度, 串口打印 + OLED显示
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "sht30.h"
#include "ssd1306.h"

static const char *TAG = "MAIN";

// ---- I2C引脚定义 (Stage 1确定的) ----
#define I2C_PORT        I2C_NUM_0
#define I2C_SCL_IO      GPIO_NUM_6
#define I2C_SDA_IO      GPIO_NUM_7
#define I2C_FREQ_HZ     100000   // 100KHz, OLED可在初始化时升到400KHz

// ---- 采样周期 ----
#define SAMPLE_INTERVAL_MS  2000   // 每2秒采样一次

// ---- 全局I2C总线句柄 ----
static i2c_master_bus_handle_t g_i2c_bus = NULL;

/**
 * @brief 初始化I2C总线 (被SHT30和OLED共享)
 */
static esp_err_t i2c_bus_init(void) {
    ESP_LOGI(TAG, "初始化 I2C 总线: SCL=GPIO%d, SDA=GPIO%d, %dHz",
             I2C_SCL_IO, I2C_SDA_IO, I2C_FREQ_HZ);

    i2c_master_bus_config_t bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port   = I2C_PORT,
        .scl_io_num = I2C_SCL_IO,
        .sda_io_num = I2C_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,  // 使用ESP32内部上拉
    };

    return i2c_new_master_bus(&bus_cfg, &g_i2c_bus);
}

/**
 * @brief 传感器采集任务
 *
 * 每2秒:
 *   1. 读SHT30
 *   2. 串口打印数据
 *   3. 更新OLED显示
 */
void Task_SensorRead(void *pvParameters) {
    ESP_LOGI(TAG, "传感器采集任务启动 (周期=%dms)", SAMPLE_INTERVAL_MS);

    int sample_count = 0;
    int error_count  = 0;

    while (1) {
        sample_count++;
        float temperature = 0.0f;
        float humidity    = 0.0f;

        esp_err_t ret = sht30_read(&temperature, &humidity);

        if (ret == ESP_OK) {
            // ---- 串口输出 ----
            printf("\n[SHT30] 采样 #%d | 温度=%.1f°C | 湿度=%.1f%% | 错误=%d\n",
                   sample_count, temperature, humidity, error_count);

            // ---- OLED 显示 ----
            ssd1306_clear();

            // 第1行 (Page 0): 标题
            ssd1306_draw_string(0, 0, "ESP32-S3 Sensor");

            // 第3行 (Page 2): 温度
            char buf[32];
            snprintf(buf, sizeof(buf), "T: %.1f C", temperature);
            ssd1306_draw_string(0, 2, buf);

            // 第5行 (Page 4): 湿度
            snprintf(buf, sizeof(buf), "H: %.1f %%", humidity);
            ssd1306_draw_string(0, 4, buf);

            // 第7行 (Page 6): 采样计数
            snprintf(buf, sizeof(buf), "Sample: #%d", sample_count);
            ssd1306_draw_string(0, 6, buf);

            ssd1306_refresh();

        } else {
            error_count++;
            ESP_LOGW(TAG, "SHT30 读取失败 (错误#%d): %s",
                     error_count, esp_err_to_name(ret));

            // 显示错误信息
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "SENSOR ERROR!");
            char buf[32];
            snprintf(buf, sizeof(buf), "Err: #%d", error_count);
            ssd1306_draw_string(0, 3, buf);
            ssd1306_refresh();
        }

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_INTERVAL_MS));
    }
}

/**
 * @brief app_main - ESP-IDF 入口函数
 */
void app_main(void) {
    printf("\n");
    printf("========================================\n");
    printf("  韦富宏 - I2C驱动温湿度传感器+OLED\n");
    printf("  YD-ESP32-S3 + ESP-IDF v5.3\n");
    printf("  SHT30(0x44) + SSD1306(0x3C)\n");
    printf("========================================\n\n");

    // ---- 1. 初始化I2C总线 ----
    ESP_ERROR_CHECK(i2c_bus_init());

    // ---- 2. 初始化SHT30 ----
    esp_err_t ret = sht30_init(g_i2c_bus);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SHT30初始化失败! 检查接线和I2C上拉电阻");
        // 不中止, 让OLED至少能显示错误
    }

    // ---- 3. 初始化OLED ----
    ret = ssd1306_init(g_i2c_bus);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "OLED初始化失败! 检查I2C地址(应为0x3C)和接线");
    }

    // ---- 4. 显示启动画面 ----
    ssd1306_clear();
    ssd1306_draw_string(10, 1, "Wei Fuhong");
    ssd1306_draw_string(10, 3, "ESP32-S3");
    ssd1306_draw_string(5, 5, "SHT30 + OLED");
    ssd1306_refresh();
    vTaskDelay(pdMS_TO_TICKS(2000));  // 启动画面停留2秒

    // ---- 5. 创建传感器采集任务 ----
    xTaskCreate(
        Task_SensorRead,     // 任务函数
        "Task_SensorRead",   // 任务名
        4096,                // 栈大小 (留足余量, 有printf)
        NULL,                // 参数
        3,                   // 优先级 (采集任务用较高优先级)
        NULL                 // 任务句柄
    );

    ESP_LOGI(TAG, "系统启动完成! FreeRTOS调度器运行中...");
    ESP_LOGI(TAG, "每%dms读取一次SHT30, 串口输出+OLED显示", SAMPLE_INTERVAL_MS);
}
