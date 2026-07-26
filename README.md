# 🚀 Embedded Learning Repository

> **韦富宏** · 通信工程 2027 届 · 嵌入式软件工程师方向  
> 深圳 · [GitHub](https://github.com/fuhong-wei)

---

## 🗂 仓库结构

```
c-embedded-practice/
│
├── 📖 01_C_Learn_Note/          C语言学习笔记（7天速通）
│   ├── day01~07 .c              每日练习代码（含详细注释）
│   └── README.md                知识点索引 + 嵌入式应用场景
│
├── 🔧 02_ESP32_Project/         核心项目：工业多传感器边缘采集系统
│   ├── 01_freertos_hello/       ✅ FreeRTOS 多任务入门
│   ├── 02_i2c_sht30/            ⬜ I2C 温湿度传感器驱动
│   ├── 03_oled_display/         ⬜ OLED 本地显示
│   ├── 04_spi_imu/              ⬜ SPI 6轴IMU驱动
│   ├── 05_data_pipeline/        ⬜ 数据采集管线+环形缓冲
│   ├── 06_ble_report/           ⬜ BLE 无线数据上报
│   ├── 07_host_python/          ⬜ Python 上位机
│   ├── 08_final_integration/    ⬜ 系统联调最终版
│   ├── doc/                     项目文档（架构图/接线图/调试记录）
│   └── log_data/                测试数据（串口日志/CSV）
│
├── 🎥 03_Resource_Video/        演示视频 / 波形截图 / 学习资料
│
├── .gitignore                   忽略编译产物和临时文件
└── README.md                    本文件
```

---

## 🎯 项目简介

**ESP32-S3 + FreeRTOS 工业多传感器边缘采集系统**

- **MCU**：ESP32-S3 (ESP-IDF v5.3 原生开发)
- **RTOS**：FreeRTOS — 5任务并行调度
- **通信协议**：I2C / SPI (10MHz) / UART / ADC / BLE 5.0
- **传感器**：SHT30(温湿度) + ICM-42688(6轴IMU) + 光敏 + PM2.5
- **上位机**：Python Streamlit 实时波形 + CSV 导出
- **调试工具**：逻辑分析仪 + 示波器

---

## 🛠 技术栈

`C语言` `FreeRTOS` `ESP-IDF` `I2C` `SPI` `UART` `BLE` `Python` `逻辑分析仪` `Git`

---

## 📈 学习进度

- [x] 2026.07.23 — GitHub + Git + GCC 环境搭建
- [x] 2026.07.23-25 — C语言 7天速通（Day1~7 完成）
- [x] 2026.07.25 — ESP-IDF v5.3 安装 + FreeRTOS 多任务编译通过
- [ ] 待板子到 — 烧录运行 FreeRTOS 三任务程序
- [ ] Week1 — I2C 驱动 SHT30 + OLED
- [ ] Week2 — SPI 驱动 IMU + 数据管线 + BLE
- [ ] Week3 — Python 上位机 + 系统联调
- [ ] Week4 — 文档完善 + 简历更新 + 投递

---

## 📬 联系

- **GitHub**：[github.com/fuhong-wei](https://github.com/fuhong-wei)
- **求职意向**：嵌入式软件实习生（深圳，可连续实习6个月）
