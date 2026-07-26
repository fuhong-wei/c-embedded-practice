/**
 * Day 6: 嵌入式C专项 —— volatile / const / #pragma / 中断
 * 日期: 2026-07-25
 *
 * 编译运行:
 *   gcc -Wall day06_embedded_c.c -o day06_embedded_c.exe
 *   ./day06_embedded_c.exe
 */

#include <stdio.h>
#include <stdint.h>

// ========== 练习1: volatile —— 嵌入式最重要的关键字 ==========

// 模拟一个硬件寄存器(在真实MCU中，这个地址映射到外设)
volatile int g_flag = 0;  // volatile = 每次从内存读，不要优化到寄存器

void ex1_volatile(void) {
    printf("===== 练习1: volatile =====\n");
    printf("volatile 告诉编译器:\n");
    printf("  \"这个变量的值可能在任何时候被改变\"\n");
    printf("  → 不要优化它！\n");
    printf("  → 每次访问都从内存读/写\n\n");

    printf("必须用 volatile 的场景:\n");
    printf("① 中断服务函数(ISR)中修改的变量\n");
    printf("   例: volatile int uart_data_ready = 0;\n");
    printf("   主循环检查 uart_data_ready，ISR把它置1\n\n");

    printf("② 硬件寄存器映射\n");
    printf("   例: volatile uint32_t *GPIO_OUT = (uint32_t*)0x40020000;\n");
    printf("   寄存器值会随硬件状态变化，不能缓存\n\n");

    printf("③ 多任务共享的变量(FreeRTOS里)\n");
    printf("   两个任务同时访问同一个变量\n\n");

    printf("如果不用volatile会怎样?\n");
    printf("  while (!flag) {}  // 编译器可能优化成: 读一次flag到寄存器 → 死循环!\n");
    printf("  volatile while (!flag) {} // 每次循环都重新读flag值\n\n");
}

// ========== 练习2: const ==========
void ex2_const(void) {
    printf("===== 练习2: const =====\n");

    int val = 100;
    const int c_val = 200;

    // c_val = 300;  // 编译错误! 不能修改const变量

    // const 和指针的组合（面试高频题！）
    const int *p1;      // 指向的值不能改，指针本身可以改
    int const *p2;      // 同上，写法不同意思一样
    int * const p3 = &val;  // 指针本身不能改，指向的值可以改
    const int * const p4 = &c_val;  // 都不能改

    p1 = &c_val;
    // *p1 = 10;  // 错误! 不能通过p1修改值

    *p3 = 999;
    // p3 = &c_val;  // 错误! p3不能指向别的地方

    printf("val = %d (通过 int * const p3 修改了)\n", val);
    printf("\n// 记忆口诀: const在*左边 = 值不能改; const在*右边 = 指针不能改\n\n");
}

// ========== 练习3: #pragma / 属性 ==========
// #pragma pack(1) —— 取消结构体对齐
#pragma pack(push, 1)  // 保存当前对齐设置，设对齐为1字节
typedef struct {
    char a;
    int b;
    char c;
} PackedStruct_t;
#pragma pack(pop)  // 恢复之前的对齐设置

void ex3_pragma(void) {
    printf("===== 练习3: #pragma =====\n");

    printf("#pragma pack(1) 取消对齐:\n");
    printf("  普通结构体大小: 12字节(char+padding+int+char+padding)\n");
    printf("  Packed大小:      %llu 字节\n", sizeof(PackedStruct_t));
    printf("\n");

    printf("嵌入式常用 #pragma:\n");
    printf("  #pragma pack(1) —— 紧凑排列，省内存\n");
    printf("  #pragma once    —— 防止头文件重复包含\n");
    printf("  #pragma GCC optimize(\"O0\") —— 关闭优化(调试用)\n\n");

    printf("GCC属性(attribute):\n");
    printf("  __attribute__((packed)) —— 紧凑排列\n");
    printf("  __attribute__((aligned(4))) —— 4字节对齐\n");
    printf("  __attribute__((weak)) —— 弱符号(可被覆盖)\n");
    printf("  __attribute__((section(\".iram1\"))) —— 放到指定内存段\n\n");
}

// ========== 练习4: 中断服务函数(ISR)写法 ==========
// 在ESP-IDF里，中断函数长这样:
// static void IRAM_ATTR gpio_isr_handler(void *arg) { ... }

// 模拟一个中断场景
static volatile int sensor_new_data = 0;  // ISR会修改，必须volatile!
static volatile float sensor_value = 0.0f;

// 模拟中断服务函数(真实环境由硬件触发)
static void simulate_isr(float new_val) {
    sensor_value = new_val;       // ISR只做最少的事
    sensor_new_data = 1;          // 通知主循环:有新数据了
    // ISR里不要printf! 不要delay! 不要复杂的操作!
}

// 主循环中的处理
static void process_sensor_data(void) {
    if (sensor_new_data) {
        float local_copy = sensor_value;  // 先拷贝局部变量
        sensor_new_data = 0;              // 清标志
        printf("  处理数据: %.2f\n", local_copy);
        // 这里可以做耗时操作
    }
}

void ex4_interrupt(void) {
    printf("===== 练习4: 中断处理模式 =====\n");
    printf("黄金法则: 中断服务函数(ISR)要短!\n");
    printf("  ISR: 读数据→存缓冲区→置标志→退出\n");
    printf("  主循环/任务: 检查标志→处理数据\n\n");

    printf("模拟中断流程:\n");
    simulate_isr(28.5f);  // 硬件触发
    process_sensor_data(); // 主循环处理
    simulate_isr(29.1f);
    process_sensor_data();

    printf("\n// ESP-IDF 中断写法:\n");
    printf("// gpio_install_isr_service(0);\n");
    printf("// gpio_isr_handler_add(GPIO_NUM_4, my_isr, NULL);\n");
    printf("// 注意: ISR函数要加 IRAM_ATTR，放到IRAM里保证执行速度\n\n");
}

// ========== 练习5: 位操作宏 ==========
// 嵌入式里天天用的宏
#define SET_BIT(reg, bit)       ((reg) |= (1U << (bit)))
#define CLEAR_BIT(reg, bit)     ((reg) &= ~(1U << (bit)))
#define TOGGLE_BIT(reg, bit)    ((reg) ^= (1U << (bit)))
#define READ_BIT(reg, bit)      (((reg) >> (bit)) & 1U)
#define WRITE_BITS(reg, mask, val) ((reg) = ((reg) & ~(mask)) | ((val) & (mask)))

void ex5_bit_macros(void) {
    printf("===== 练习5: 位操作宏(每天都要用) =====\n");

    uint32_t reg = 0x00;

    SET_BIT(reg, 3);
    printf("SET_BIT(reg, 3):     0x%02X\n", reg);

    SET_BIT(reg, 7);
    printf("SET_BIT(reg, 7):     0x%02X\n", reg);

    CLEAR_BIT(reg, 3);
    printf("CLEAR_BIT(reg, 3):   0x%02X\n", reg);

    TOGGLE_BIT(reg, 0);
    printf("TOGGLE_BIT(reg, 0):  0x%02X\n", reg);
    TOGGLE_BIT(reg, 0);
    printf("TOGGLE_BIT(reg, 0):  0x%02X (再翻转一次恢复)\n", reg);

    printf("READ_BIT(reg, 7):    %u\n", (unsigned int)READ_BIT(reg, 7));
    printf("READ_BIT(reg, 3):    %u\n", (unsigned int)READ_BIT(reg, 3));

    printf("\n// 这些宏在ESP-IDF和STM32 HAL库里到处都是\n");
    printf("// 面试官看到你手写位操作宏 = 你真的写过MCU代码\n\n");
}

// ========== 练习6: 大小端检测 ==========
void ex6_endian(void) {
    printf("===== 练习6: 大小端 =====\n");

    uint16_t val = 0x1234;
    uint8_t *p = (uint8_t*)&val;

    if (p[0] == 0x34) {
        printf("你的系统是: 小端(Little Endian)\n");
        printf("  低字节在低地址: [0x34][0x12]\n");
    } else {
        printf("你的系统是: 大端(Big Endian)\n");
        printf("  高字节在低地址: [0x12][0x34]\n");
    }
    printf("\n// ARM Cortex-M/ESP32 = 小端\n");
    printf("// 网络字节序 = 大端\n");
    printf("// 跨系统通信要注意字节序转换!\n\n");
}

// ========== 主函数 ==========
int main(void) {
    printf("========================================\n");
    printf("  韦富宏 - C语言 Day 6: 嵌入式C专项\n");
    printf("========================================\n\n");

    ex1_volatile();
    ex2_const();
    ex3_pragma();
    ex4_interrupt();
    ex5_bit_macros();
    ex6_endian();

    printf("========================================\n");
    printf("  Day 6 完成!\n");
    printf("  volatile = 防优化，中断/寄存器必用\n");
    printf("  const = 只读保护\n");
    printf("  位操作宏 = MCU开发的基本功\n");
    printf("========================================\n");

    return 0;
}
