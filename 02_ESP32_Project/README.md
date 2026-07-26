# 🔧 ESP32-S3 工业多传感器边缘采集系统

> 嵌入式软件求职核心项目 | 2026.07 - 2026.08

## 项目概述

基于 ESP32-S3 + FreeRTOS + ESP-IDF v5.3 的工业级多传感器边缘采集节点，集成 I2C/SPI/ADC/UART 四种总线传感器，BLE 无线数据上报，Python 上位机实时监控。

## 技术栈

```
MCU:      ESP32-S3 (Xtensa LX7)
RTOS:     FreeRTOS (任务调度/Queue/Mutex)
通信:     I2C(100KHz) / SPI(10MHz Mode0) / UART(115200) / BLE 5.0
传感器:   SHT30(I2C) + ICM-42688(SPI) + 光敏(ADC) + PM2.5(UART)
上位机:   Python Streamlit + PySerial
调试工具: 逻辑分析仪(24MHz) + 示波器
```

## 目录结构

```
02_ESP32_Project/
├── 01_freertos_hello/      # Week1: FreeRTOS多任务入门
├── 02_i2c_sht30/           # Week1: I2C驱动温湿度传感器 (待添加)
├── 03_oled_display/        # Week1: OLED本地显示 (待添加)
├── 04_spi_imu/             # Week2: SPI驱动6轴IMU (待添加)
├── 05_data_pipeline/       # Week2: 数据采集管线+环形缓冲 (待添加)
├── 06_ble_report/          # Week2: BLE无线数据上报 (待添加)
├── 07_host_python/         # Week3: Python上位机 (待添加)
├── 08_final_integration/   # Week3-4: 系统联调+最终版 (待添加)
├── doc/                    # 文档: 架构图/接线图/调试记录
└── log_data/               # 测试数据: 串口日志/CSV数据
```

## 开发环境

```bash
# 编译
idf.py set-target esp32s3
idf.py build

# 烧录
idf.py -p COM3 flash

# 查看串口输出
idf.py -p COM3 monitor
```

## 硬件清单

| 模块 | 型号 | 协议 | 关键参数 |
|------|------|------|----------|
| 温湿度传感器 | SHT30 | I2C (0x44) | ±0.3°C, ±2%RH |
| 6轴IMU | ICM-42688-P | SPI (10MHz) | ±4g, ±500dps |
| OLED | SSD1306 0.96" | I2C (0x3C) | 128×64 |
| 光敏传感器 | GL5516 | ADC (12bit) | 0-4095 |
| PM2.5传感器 | PMS5003 | UART (9600) | 待添加 |
