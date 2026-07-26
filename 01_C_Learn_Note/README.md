# 📖 C语言嵌入式学习笔记

> 7天速通嵌入式C语言核心知识点 | 2026.07

## 学习路线图

```
Day1 数据类型+位运算+控制流 → Day2 函数+作用域+static → Day3 指针核心
    ↓                              ↓                           ↓
  位操作MCU寄存器基础           static封装驱动函数           函数指针→FreeRTOS基石
    ↓                              ↓                           ↓
Day4 栈vs堆+内存管理          Day5 结构体+位域+枚举        Day6 嵌入式C专项
    ↓                              ↓                           ↓
  嵌入式少用malloc的原因        外设寄存器映射原理           volatile/const/中断
    ↓
Day7 环形缓冲区(数据结构) ──→ 直接用于ESP32多传感器项目！
```

## 每日内容

| 文件 | 内容 | 嵌入式应用场景 |
|------|------|---------------|
| [day01_basics.c](./day01_basics.c) | 数据类型、sizeof、位运算、控制流 | `REG |= (1<<3)` 操作MCU寄存器 |
| [day02_functions.c](./day02_functions.c) | 函数、作用域、static/extern | static封装I2C驱动内部函数 |
| [day03_pointers.c](./day03_pointers.c) | 指针、函数指针、void* | `xTaskCreate(func_ptr, ...)` |
| [day04_memory.c](./day04_memory.c) | 栈vs堆、malloc/free、碎片化 | 嵌入式尽量静态分配 |
| [day05_struct_union.c](./day05_struct_union.c) | 结构体对齐、位域、union、枚举 | 外设寄存器映射(struct) |
| [day06_embedded_c.c](./day06_embedded_c.c) | volatile、const、中断、位操作宏 | ISR写法、防编译器优化 |
| [day07_ringbuffer.c](./day07_ringbuffer.c) | 环形缓冲区(生产者-消费者) | 传感器采集→数据处理的缓冲 |

## 编译运行

```bash
gcc -Wall dayXX_xxx.c -o dayXX_xxx.exe
./dayXX_xxx.exe
```

## 关键收获

- ✅ C语言指针 = 嵌入式程序员的筷子，天天拿天天用
- ✅ 函数指针 + void* = FreeRTOS `xTaskCreate` 的底层原理
- ✅ volatile = 中断服务函数修改的变量**必须**加，否则编译器优化掉
- ✅ 环形缓冲区 = 嵌入式最经典数据结构，解耦生产者和消费者
- ✅ 位操作 = MCU寄存器编程的基本功，`SET_BIT/CLEAR_BIT/TOGGLE_BIT` 宏
