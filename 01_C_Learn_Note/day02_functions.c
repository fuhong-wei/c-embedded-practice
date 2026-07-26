/** 
 * Day 2: 函数、作用域、static/extern
 * 日期: 2026-07-23
 * 内容: 函数定义与调用、局部/全局变量、static的三种用法、extern
 *
 * 编译运行:
 *   gcc -Wall day02_functions.c -o day02_functions.exe
 *   ./day02_functions.exe
 *
*/
#include <stdio.h>
#include <string.h>

// ========== 全局变量 ==========
// 在所有函数外面定义 = 全局变量，整个文件都能用
int global_count = 0;  // 全局变量，默认初始化为0

// ========== 练习1: 函数基础 ==========
// 函数 = 把一段代码打包，取个名字，之后可以反复调用

// 有参有返回值
int add(int a, int b) {
    return a + b;
}

// 有参无返回值
void print_message(const char *msg) {
    printf("消息: %s\n", msg);
    global_count++;  // 函数内部可以访问全局变量
}

// 无参有返回值
int get_system_status(void) {
    return 0;  // 0=正常，嵌入式常见约定
}

// 无参无返回值
void say_hello(void) {
    // static 局部变量: 只在第一次调用时初始化为0,
    // 之后每次调用保留上次的值！
    static int call_times = 0;
    call_times++;
    printf("Hello! (第%d次调用本函数)\n", call_times);
}

// ========== 练习2: 作用域 ==========
int sensor_value = 100;  // 全局变量

void exercise2_scope(void) {
    printf("===== 练习2: 作用域 =====\n");

    int sensor_value = 50;  // 局部变量，和全局变量同名
    printf("局部 sensor_value = %d\n", sensor_value);   // 输出 50
    // 局部变量会"遮蔽"全局变量——就近原则

    // 如果想在这个函数里访问全局的那个同名变量，C语言做不到
    // 所以嵌入式开发中，全局变量通常会加一个 g_ 前缀来区分
    printf("// 提示: 局部变量优先于全局变量，这就是\"就近原则\"\n\n");
}

// ========== 练习3: static 的三种用法 ==========

// 用法①: static 局部变量 —— 生命周期持续到程序结束
void count_calls(void) {
    static int counter = 0;  // 只初始化一次！后面每次调用都保留上次的值
    counter++;
    printf("  本函数被调用了 %d 次\n", counter);
}

// 用法②: static 全局变量 —— 只在当前.c文件可见
static int internal_config = 42;  // 用static修饰，其他.c文件访问不到

void set_config(int val) {
    internal_config = val;
}

int get_config(void) {
    return internal_config;
}

// 用法③: static 函数 —— 只在当前.c文件可见
static void private_helper(void) {
    printf("  我是内部辅助函数，外面看不到我\n");
}

void exercise3_static(void) {
    printf("===== 练习3: static 的三种用法 =====\n");

    printf("\n--- 用法①: static局部变量 ---\n");
    count_calls();  // 第1次
    count_calls();  // 第2次
    count_calls();  // 第3次
    // 每次调用，counter 的值会保留，不会被重置为0

    printf("\n--- 用法②: static全局变量 ---\n");
    printf("  当前配置值: %d\n", get_config());
    set_config(99);
    printf("  修改后配置值: %d\n", get_config());
    // internal_config 在外面无法直接访问，只能通过函数操作——这叫"封装"

    printf("\n--- 用法③: static函数 ---\n");
    private_helper();
    printf("// 嵌入式里，static常用于把底层驱动函数\"隐藏\"起来\n");
    printf("// 比如I2C的起始信号函数只在本文件用，就加static\n\n");
}

// ========== 练习4: extern ==========
// extern = 声明"这个变量/函数在别的.c文件里定义的"
// 实际演示需要两个文件，这里用注释说明

// 假设有个文件叫 sensor.c，里面定义了:
//   int temperature = 25;
// 那在这个文件里想用这个变量，就得写:
//   extern int temperature;   // 声明，不是定义！

void exercise4_extern_concept(void) {
    printf("===== 练习4: extern 概念 =====\n");
    printf("extern = \"这个变量在别的.c文件里，链接时能找到\"\n");
    printf("\n");
    printf("sensor.c (定义):\n");
    printf("  int temperature = 25;         // 真正分配内存\n");
    printf("\n");
    printf("main.c (声明):\n");
    printf("  extern int temperature;        // 只是告诉编译器\"有这个变量\"\n");
    printf("\n");
    printf("// 关键词: 定义 = 分配内存，声明 = 告诉编译器类型和名字\n");
    printf("// 嵌入式里，中断服务函数经常用extern声明\n\n");
}

// ========== 练习5: 综合——模拟一个传感器读取模块 ==========

// 模拟硬件寄存器的值(实际是MCU的某个地址)
static int sensor_register = 0;

// 初始化传感器(实际会配置GPIO、I2C等)
static void sensor_init(void) {
    sensor_register = 0;
    printf("[传感器] 初始化完成\n");
}

// 读取传感器值(实际会读I2C数据寄存器)
static int sensor_read(void) {
    // 模拟: 每次读取值+1
    sensor_register++;
    return sensor_register;
}

// 对外只暴露这两个函数
void sensor_module_start(void) {
    sensor_init();
    printf("[传感器] 模块启动\n");
}

int sensor_module_get_value(void) {
    return sensor_read();
}

void exercise5_sensor_simulation(void) {
    printf("===== 练习5: 综合——模拟传感器模块 =====\n");

    sensor_module_start();
    printf("第1次读取: %d\n", sensor_module_get_value());
    printf("第2次读取: %d\n", sensor_module_get_value());
    printf("第3次读取: %d\n", sensor_module_get_value());

    printf("\n// 这个例子体现了:\n");
    printf("// static函数(sensor_init/sensor_read) = 模块内部实现，外部不可见\n");
    printf("// 公开函数(sensor_module_start/xxx) = 对外的API接口\n");
    printf("// 这就是嵌入式驱动的常见写法：隐藏底层细节，暴露简洁接口\n\n");
}

// ========== 主函数 ==========
int main(void) {
    printf("========================================\n");
    printf("  韦富宏 - C语言嵌入式学习 Day 2\n");
    printf("  函数、作用域、static、extern\n");
    printf("========================================\n\n");

    // 练习1
    printf("===== 练习1: 函数基础 =====\n");
    int sum = add(3, 5);
    printf("3 + 5 = %d\n", sum);
    print_message("嵌入式学习第2天");
    printf("系统状态: %d\n", get_system_status());
    say_hello();
    say_hello();
    say_hello();
    printf("\n");

    // 练习2
    exercise2_scope();

    // 练习3
    exercise3_static();

    // 练习4
    exercise4_extern_concept();

    // 练习5
    exercise5_sensor_simulation();

    printf("========================================\n");
    printf("  Day 2 完成！\n");
    printf("  核心收获: static的三种用法、作用域、封装思想\n");
    printf("========================================\n");

    return 0;
}
