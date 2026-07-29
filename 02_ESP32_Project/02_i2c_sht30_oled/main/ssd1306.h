/**
 * ssd1306.h - SSD1306 OLED 128x64 驱动 (I2C)
 *
 * 控制器: Solomon Systech SSD1306
 * 接口:   I2C, 地址 0x3C
 * 分辨率: 128列 × 64行 = 128×8页
 */

#pragma once

#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化OLED
 * @param bus_handle I2C总线句柄
 * @return ESP_OK=成功
 */
esp_err_t ssd1306_init(i2c_master_bus_handle_t bus_handle);

/**
 * @brief 清屏 (全部像素灭)
 */
void ssd1306_clear(void);

/**
 * @brief 刷新显示 (把缓冲区数据发送到OLED)
 */
void ssd1306_refresh(void);

/**
 * @brief 在指定位置显示字符串 (5x7字体)
 * @param x   列 (0-127)
 * @param y   页 (0-7), 每页8像素高
 * @param str C字符串
 */
void ssd1306_draw_string(uint8_t x, uint8_t y, const char *str);

/**
 * @brief 在指定位置显示一个字符
 * @param x 列 (0-127)
 * @param y 页 (0-7)
 * @param c 字符
 */
void ssd1306_draw_char(uint8_t x, uint8_t y, char c);

/**
 * @brief 写全屏画面 (用户自定义帧缓冲)
 * @param buffer 1024字节 (128×64÷8=1024)
 */
void ssd1306_draw_fullscreen(const uint8_t *buffer);

#ifdef __cplusplus
}
#endif
