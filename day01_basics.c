/**
 * Day 1: C语言基础回顾
 * 日期: 2026-07-23
 * 内容: 数据类型、运算符、控制流
 *
 * 编译运行:
 *   gcc day01_basics.c -o day01_basics.exe
 *   ./day01_basics.exe
 */

#include <stdio.h>
// ========== 练习1: 数据类型与sizeof ==========sizeof:编译器运算符，对应size\_t，uul，no%d
void exercise1_data_types(void) {
    printf("===== 练习1: 数据类型与内存大小 =====\n");

    printf("char:        %2llu 字节 (范围: %d ~ %d)\n",
           sizeof(char), -128, 127);
    printf("short:       %2llu 字节\n", sizeof(short));
    printf("int:         %2llu 字节\n", sizeof(int));
    printf("long:        %2llu 字节\n", sizeof(long));
    printf("long long:   %2llu 字节\n", sizeof(long long));
    printf("float:       %2llu 字节\n", sizeof(float));
    printf("double:      %2llu 字节\n", sizeof(double));
    printf("指针(void*): %2llu 字节\n\n", sizeof(void*));
    // 思考：为什么嵌入式MCU上 int 可能是2字节，而PC上是4字节？答：本质根源：CPU 的通用寄存器位宽（处理器字长），stm32:4字节
}

// ========== 练习2: 运算符 ==========
void exercise2_operators(void) {
    printf("===== 练习2: 位运算（嵌入式核心技能） =====\n");

    unsigned char reg = 0x00;  // 模拟一个8位寄存器

    // 置位（Set bit）: 把第3位置1
    reg |= (1 << 3);
    printf("置位第3位后: 0x%02X (二进制: 0000_1000)\n", reg);

    // 清位（Clear bit）: 把第3位清0
    reg &= ~(1 << 3);
    printf("清零第3位后: 0x%02X\n", reg);

    // 翻转（Toggle bit）: 翻转第5位
    reg ^= (1 << 5);
    printf("翻转第5位后: 0x%02X (二进制: 0010_0000)\n", reg);

    // 读取某一位
    if (reg & (1 << 5)) {
        printf("第5位 = 1 (高电平)\n");
    } else {
        printf("第5位 = 0 (低电平)\n");
    }
    printf("// 提示: 位运算是操作MCU寄存器的基本功，必须滚瓜烂熟\n\n");
}

// ========== 练习3: 控制流 ==========
void exercise3_control_flow(void) {
    printf("===== 练习3: 控制流 =====\n");

    // for循环: 计算1到100的和
    int sum = 0;
    for (int i = 1; i <= 100; i++) {
        sum += i;
    }
    printf("1+2+...+100 = %d (公式验证: %d)\n", sum, 100 * 101 / 2);

    // switch-case: 模拟外设状态机
    int device_state = 2;  // 0=空闲 1=运行 2=错误
    switch (device_state) {
        case 0:
            printf("设备状态: 空闲\n");
            break;
        case 1:
            printf("设备状态: 运行中\n");
            break;
        case 2:
            printf("设备状态: 错误! 需要复位\n");
            break;
        default:
            printf("设备状态: 未知\n");
            break;
    }
    printf("// 提示: 状态机是嵌入式开发最常用的设计模式\n\n");
}

// ========== 练习4: 数组与指针初步 ==========
void exercise4_array_pointer(void) {
    printf("===== 练习4: 数组与指针的等价关系 =====\n");

    int arr[5] = {10, 20, 30, 40, 50};

    // 数组名就是首元素地址
    printf("arr     = %p\n", (void*)arr);
    printf("&arr[0] = %p\n", (void*)&arr[0]);
    printf("两者相等，说明数组名=首地址\n\n");

    // 四种等价的方式访问数组
    for (int i = 0; i < 5; i++) {
        printf("arr[%d]=%2d | *(arr+%d)=%2d | ",
               i, arr[i], i, *(arr + i));
        int *p = arr;
        printf("p[%d]=%2d | *(p+%d)=%2d\n",
               i, p[i], i, *(p + i));
    }
    printf("// 四种写法完全等价！理解这一点，指针就入门了\n");
}

// ========== 主函数 ==========
int main(void) {
    printf("========================================\n");
    printf("  韦富宏 - C语言嵌入式学习 Day 1\n");
    printf("  2026-07-23\n");
    printf("========================================\n\n");

    exercise1_data_types();
    exercise2_operators();
    exercise3_control_flow();
    exercise4_array_pointer();

    printf("========================================\n");
    printf("  Day 1 完成！代码已上传 GitHub\n");
    printf("========================================\n");

    return 0;
}

/**
练习 1：数据类型 + sizeof
核心：char/short/int/long/long long、float/double 各自固定占用字节大小；sizeof 是编译期运算符，不是函数
关键区分：PC (64 位) int 固定 4 字节；老式 8 位单片机 int 为 2 字节，根源是 CPU 字长与硬件内存成本不同
嵌入式用处：定义寄存器、采集数据时，优先用stdint.h固定长度类型，不会因平台变化出错
练习 2：位运算（嵌入式重中之重）
4 条万能固定公式（操作寄存器标准写法）

置 1 某一位：reg |= 1 << n
清 0 某一位：reg &= ~(1 << n)
翻转某一位：reg ^= 1 << n
读取某位电平：if(reg & 1 << n)

本质：寄存器就是 8/32 位二进制开关，位运算精准修改单个 bit，不影响其余位
嵌入式用处：控制 LED、引脚高低电平、配置外设寄存器、硬件开关控制
练习 3：程序控制流
for 循环：用来重复执行代码，典型场景数值累加、批量遍历数据
switch-case：匹配数值执行对应分支，标准用法设备状态机（空闲 / 运行 / 故障三种状态分开处理逻辑）
硬性规则：switch 每个 case 必须加 break；default 捕获异常状态
嵌入式用处：电机工作状态、串口收发状态、设备故障判断全部依靠状态机
练习 4：数组与指针入门
核心等式：数组名 = 数组首个元素的内存地址，arr == &arr[0]
万能等价公式：arr[i] == *(arr+i)，指针变量接收数组地址后p完全等同于数组名arr
4 种访问数组方式本质一模一样，数组只是指针的简易写法
嵌入式用处：单片机硬件寄存器本质就是固定地址，只能用指针方式读写
整体串联一条学习逻辑数据类型→把控内存占用大小
位运算→操控硬件最底层的比特位
控制流→编写设备不同状态的运行逻辑
数组指针→看懂内存地址，看懂单片机硬件底层读写原理
整套就是嵌入式 C 语言最基础的底层骨架。
*/
