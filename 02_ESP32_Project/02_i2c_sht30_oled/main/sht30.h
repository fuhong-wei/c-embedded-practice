/**
 * sht30.h - SHT30 温湿度传感器驱动 (I2C)
 *
 * 芯片: Sensirion SHT30-DIS
 * 接口: I2C, 地址 0x44
 * 模式: 单次测量, 高重复性, 时钟拉伸使能
 */

#pragma once

#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化SHT30传感器
 *
 * @param bus_handle I2C总线句柄 (由main.c创建)
 * @return esp_err_t ESP_OK=成功
 *
 * 用法:
 *   i2c_master_bus_handle_t i2c_bus;
 *   // ... 创建I2C总线 ...
 *   sht30_init(i2c_bus);
 */
esp_err_t sht30_init(i2c_master_bus_handle_t bus_handle);

/**
 * @brief 读取一次温湿度数据
 *
 * @param temperature [out] 温度值 (°C), 分辨率0.01°C
 * @param humidity    [out] 湿度值 (%RH), 分辨率0.01%
 * @return esp_err_t ESP_OK=成功, ESP_FAIL=CRC校验失败, ESP_ERR_TIMEOUT=I2C超时
 *
 * 用法:
 *   float t, h;
 *   if (sht30_read(&t, &h) == ESP_OK) {
 *       printf("温度=%.1f°C, 湿度=%.1f%%\n", t, h);
 *   }
 */
esp_err_t sht30_read(float *temperature, float *humidity);

#ifdef __cplusplus
}
#endif
