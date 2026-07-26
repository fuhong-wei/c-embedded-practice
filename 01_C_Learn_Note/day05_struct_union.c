/**
 * Day 5: 结构体、联合体、位运算、枚举
 * 日期: 2026-07-25
 *
 * 编译运行:
 *   gcc -Wall day05_struct_union.c -o day05_struct_union.exe
 *   ./day05_struct_union.exe
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

// ========== 练习1: 结构体基础 ==========

// 定义传感器数据结构体
typedef struct {
    uint16_t id;          // 2字节
    float temperature;    // 4字节
    float humidity;       // 4字节
    uint32_t timestamp;   // 4字节
} SensorData_t;

void ex1_struct_basics(void) {
    printf("===== 练习1: 结构体基础 =====\n");

    SensorData_t data;
    data.id = 1;
    data.temperature = 28.5f;
    data.humidity = 65.3f;
    data.timestamp = 1751364000;

    printf("传感器ID: %u\n", data.id);
    printf("温度: %.1f C\n", data.temperature);
    printf("湿度: %.1f %%\n", data.humidity);
    printf("结构体大小: %llu 字节\n", sizeof(SensorData_t));

    // 用指针访问结构体
    SensorData_t *p = &data;
    printf("通过指针: 温度=%.1f\n", p->temperature);  // -> 是 (*p).xxx 的简写

    printf("\n// 嵌入式里，结构体用来封装外设寄存器映射\n");
    printf("// 传感器数据通常打包成结构体在任务间传递\n\n");
}

// ========== 练习2: 结构体内存对齐（嵌入式关键！） ==========
void ex2_struct_alignment(void) {
    printf("===== 练习2: 内存对齐 =====\n");

    // 未优化的结构体
    typedef struct {
        char a;      // 1字节
        int b;       // 4字节
        char c;      // 1字节
        short d;     // 2字节
    } BadLayout_t;

    // 优化后的结构体
    typedef struct {
        int b;       // 4字节（最长的放前面）
        short d;     // 2字节
        char a;      // 1字节
        char c;      // 1字节
    } GoodLayout_t;

    printf("BadLayout 大小: %llu 字节 (浪费了填充字节)\n",
           sizeof(BadLayout_t));
    printf("GoodLayout大小: %llu 字节 (紧凑排列)\n",
           sizeof(GoodLayout_t));
    printf("节省: %llu 字节\n\n",
           sizeof(BadLayout_t) - sizeof(GoodLayout_t));

    printf("// MCU内存只有几百KB，每个字节都要省\n");
    printf("// 规则: 按成员大小从大到小排列\n\n");
}

// ========== 练习3: 位域 —— 节省到每一位 ==========
void ex3_bitfield(void) {
    printf("===== 练习3: 位域 =====\n");

    // 用位域定义一个8位状态寄存器
    typedef struct {
        uint8_t power_on  : 1;  // bit0: 电源状态
        uint8_t error     : 1;  // bit1: 错误标志
        uint8_t reserved  : 2;  // bit2-3: 保留
        uint8_t mode      : 3;  // bit4-6: 工作模式(0-7)
        uint8_t interrupt : 1;  // bit7: 中断标志
    } StatusReg_t;

    StatusReg_t reg;
    memset(&reg, 0, sizeof(reg));  // 清零

    reg.power_on = 1;
    reg.error = 0;
    reg.mode = 5;  // 二进制101

    printf("StatusReg 大小: %llu 字节\n", sizeof(StatusReg_t));
    printf("power_on=%d, error=%d, mode=%d\n",
           reg.power_on, reg.error, reg.mode);
    printf("\n// 位域常用于映射MCU硬件寄存器\n");
    printf("// 一个字节就能表达8种状态信息\n\n");
}

// ========== 练习4: 联合体（Union） ==========
void ex4_union(void) {
    printf("===== 练习4: 联合体 =====\n");

    // union: 所有成员共享同一块内存
    typedef union {
        float float_val;
        uint8_t bytes[4];
        uint32_t raw;
    } FloatConverter_t;

    FloatConverter_t conv;
    conv.float_val = 28.5f;

    printf("浮点数: %.2f\n", conv.float_val);
    printf("原始字节: 0x%02X %02X %02X %02X\n",
           conv.bytes[0], conv.bytes[1],
           conv.bytes[2], conv.bytes[3]);
    printf("32位整数值: 0x%08X\n", conv.raw);

    printf("\n// 嵌入式里，union常用于:\n");
    printf("// ① float和字节数组互转(I2C传输浮点数)\n");
    printf("// ② 用不同方式解读同一段内存\n");
    printf("// ③ 节省内存(成员共用空间)\n\n");
}

// ========== 练习5: 枚举 ==========
void ex5_enum(void) {
    printf("===== 练习5: 枚举 =====\n");

    // 定义设备状态枚举
    typedef enum {
        STATE_IDLE = 0,
        STATE_RUNNING,
        STATE_ERROR,
        STATE_MAINTENANCE
    } DeviceState_t;

    DeviceState_t state = STATE_RUNNING;

    switch (state) {
        case STATE_IDLE:
            printf("设备: 空闲\n"); break;
        case STATE_RUNNING:
            printf("设备: 运行中\n"); break;
        case STATE_ERROR:
            printf("设备: 故障!\n"); break;
        case STATE_MAINTENANCE:
            printf("设备: 维护中\n"); break;
    }

    printf("\n// 枚举 = 给数字取有意义的名字\n");
    printf("// STATE_IDLE=0, STATE_RUNNING=1, ...\n");
    printf("// 比直接写数字0/1/2语义清晰多了\n\n");
}

// ========== 练习6: 综合 —— 模拟MCU外设寄存器映射 ==========
void ex6_register_map(void) {
    printf("===== 练习6: 外设寄存器映射(嵌入式核心技能) =====\n");

    // 模拟一个I2C外设的寄存器结构
    typedef struct {
        volatile uint32_t CTRL;      // 控制寄存器
        volatile uint32_t STATUS;    // 状态寄存器
        volatile uint32_t DATA;      // 数据寄存器
        volatile uint32_t BAUD;      // 波特率寄存器
    } I2C_Registers_t;

    // 在MCU里，这个结构体的基地址就是一个绝对地址
    // 比如 #define I2C1_BASE 0x40005400
    // I2C_Registers_t *I2C1 = (I2C_Registers_t*)I2C1_BASE;

    printf("I2C寄存器结构体大小: %llu 字节\n", sizeof(I2C_Registers_t));
    printf("\n");
    printf("在真实MCU代码里，你会看到:\n");
    printf("  SPI1->DATA = 0xAB;  // 写数据到SPI1的DATA寄存器\n");
    printf("  uint8_t val = I2C1->STATUS;  // 读I2C1的状态寄存器\n\n");
    printf("// -> 操作符 = 通过指针访问结构体成员\n");
    printf("// 这就是STM32 HAL库/ESP-IDF底层的工作原理!\n\n");
}

// ========== 主函数 ==========
int main(void) {
    printf("========================================\n");
    printf("  韦富宏 - C语言 Day 5: 结构体/联合体/位运算\n");
    printf("========================================\n\n");

    ex1_struct_basics();
    ex2_struct_alignment();
    ex3_bitfield();
    ex4_union();
    ex5_enum();
    ex6_register_map();

    printf("========================================\n");
    printf("  Day 5 完成!\n");
    printf("  结构体 = 把相关数据打包成一个类型\n");
    printf("  位域 = 省内存到极致\n");
    printf("  Union = 同一内存多种解读方式\n");
    printf("========================================\n");

    return 0;
}
