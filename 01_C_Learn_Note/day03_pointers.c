/**
 * Day 3: 指针深入 —— 嵌入式程序员的第一道坎
 * 日期: 2026-07-25
 *
 * 编译运行:
 *   gcc -Wall day03_pointers.c -o day03_pointers.exe
 *   ./day03_pointers.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========== 练习1: 指针是什么？ ==========
void ex1_pointer_basics(void) {
    printf("===== 练习1: 指针是什么？ =====\n");

    int a = 42;
    int *p = &a;  // p 存的是 a 的地址

    printf("a 的值: %d\n", a);
    printf("a 的地址: %p\n", (void*)&a);
    printf("p 的值(存的地址): %p\n", (void*)p);
    printf("*p 的值(解引用): %d\n", *p);

    // 通过指针修改原变量
    *p = 99;
    printf("修改*p=99后, a=%d\n\n", a);
    // 关键结论: p 指向 a, 改 *p 就是改 a
}

// ========== 练习2: 指针与数组 ==========
void ex2_pointer_array(void) {
    printf("===== 练习2: 指针与数组等价 =====\n");

    int arr[5] = {10, 20, 30, 40, 50};
    int *p = arr;  // 数组名就是首地址，不需要 &

    // 四种等价的访问方式
    printf("arr[2]   = %d\n", arr[2]);     // 下标法
    printf("*(arr+2) = %d\n", *(arr+2));   // 指针偏移
    printf("p[2]     = %d\n", p[2]);       // 把指针当下标用
    printf("*(p+2)   = %d\n", *(p+2));     // 纯指针法

    printf("\n// 这四种写法编译后的汇编代码完全一样！\n");
    printf("// 指针+1 = 地址移动 sizeof(类型) 个字节\n");
    printf("// int* p 移动1 = 移动4字节, char* 移动1 = 移动1字节\n\n");
}

// ========== 练习3: 指针与函数（函数指针） ==========
// FreeRTOS 的 xTaskCreate 就用到了函数指针!

// 定义一个函数指针类型: 指向"返回void, 参数void*"的函数
typedef void (*TaskFunction_t)(void*);

void my_task_1(void *param) {
    int *p = (int*)param;
    printf("  任务1执行, 参数=%d\n", *p);
}

void my_task_2(void *param) {
    char *msg = (char*)param;
    printf("  任务2执行, 参数=%s\n", msg);
}

void ex3_function_pointer(void) {
    printf("===== 练习3: 函数指针 =====");
    printf("（就是 xTaskCreate 的原理！）\n\n");

    TaskFunction_t func_ptr;  // 声明一个函数指针

    int arg1 = 100;
    func_ptr = my_task_1;     // 让指针指向任务1
    func_ptr(&arg1);          // 调用任务1

    char *arg2 = "hello freertos";
    func_ptr = my_task_2;     // 同一个指针，换指向任务2
    func_ptr(arg2);           // 调用任务2

    printf("\n// xTaskCreate(Task_Fast, ...) 就是这样把函数名传进去的\n");
    printf("// 函数名本身就是一个函数指针!\n\n");
}

// ========== 练习4: void* 万能指针 ==========
// FreeRTOS 任务参数用的是 void* pvParameters
void ex4_void_pointer(void) {
    printf("===== 练习4: void* 万能指针 =====\n");

    int num = 42;
    float pi = 3.14;
    char msg[] = "embedded";

    void *vp;  // void* 可以指向任何类型

    vp = &num;
    printf("void* → int:    %d\n", *(int*)vp);     // 用的时候要强制转换

    vp = &pi;
    printf("void* → float:  %.2f\n", *(float*)vp);

    vp = msg;
    printf("void* → string: %s\n", (char*)vp);

    printf("\n// xTaskCreate最后一个参数是 void* pvParameters\n");
    printf("// 就是你可以传任意类型的数据给任务！\n\n");
}

// ========== 练习5: 指针与动态内存 ==========
typedef struct {
    int id;
    float temperature;
    float humidity;
} SensorData;

void ex5_dynamic_memory(void) {
    printf("===== 练习5: 动态内存分配 =====\n");

    // malloc: 在"堆"上分配内存(栈是自动的, 堆是手动管理的)
    SensorData *data = (SensorData*)malloc(sizeof(SensorData));
    if (data == NULL) {
        printf("内存分配失败!\n");
        return;
    }

    data->id = 1;  // -> 是 (*data).id 的简写
    data->temperature = 28.5;
    data->humidity = 65.3;

    printf("传感器ID: %d\n", data->id);
    printf("温度: %.1f°C\n", data->temperature);
    printf("湿度: %.1f%%\n", data->humidity);

    // 用完了要释放！不然就内存泄漏
    free(data);
    printf("\n// 嵌入式里尽量少用malloc, 因为内存碎片很致命\n");
    printf("// MCU开发中一般用静态分配或FreeRTOS的pvPortMalloc\n\n");
}

// ========== 练习6: 指针常见错误(要会识别) ==========
void ex6_common_bugs(void) {
    printf("===== 练习6: 指针常见Bug =====\n");

    printf("Bug1: 野指针\n");
    // int *p; *p = 10;  // 危险! p指向随机地址
    printf("  int *p = NULL;  // 好习惯: 初始化指针为NULL\n\n");

    printf("Bug2: 返回局部变量地址\n");
    // int* get_value(void) { int x=5; return &x; } // x在栈上, 函数返回就消失
    printf("  不要返回局部变量的地址！用static或malloc\n\n");

    printf("Bug3: 忘记free导致内存泄漏\n");
    printf("  每次malloc必须对应一个free\n\n");

    printf("Bug4: 数组越界\n");
    printf("  int arr[5]; arr[5]=10;  // 越界! 下标范围0-4\n");
    printf("  FreeRTOS栈溢出就是这种错误\n\n");
}

// ========== 练习7: 综合——手写一个简化版xTaskCreate ==========
// 看看FreeRTOS的 TaskFunction_t 本质上是什么

#define MAX_TASKS 10

typedef struct {
    char name[16];
    TaskFunction_t function;
    void *param;
    int priority;
} SimpleTask;

SimpleTask task_list[MAX_TASKS];  // 模拟FreeRTOS的任务列表
int task_count = 0;

// 我们的简化版 "xTaskCreate"
int Simple_xTaskCreate(TaskFunction_t func, const char *name,
                        void *param, int priority) {
    strncpy(task_list[task_count].name, name, 15);
    task_list[task_count].function = func;
    task_list[task_count].param = param;
    task_list[task_count].priority = priority;
    printf("  [Scheduler] 创建任务: %s, 优先级=%d\n", name, priority);
    task_count++;
    return 0;
}

// 用我们简化版API创建的任务
void test_task(void *param) {
    printf("  test_task运行! 收到消息: %s\n", (char*)param);
}

void ex7_freertos_simulation(void) {
    printf("===== 练习7: 模拟FreeRTOS任务创建 =====\n");

    Simple_xTaskCreate(test_task, "Task_A", "Hello任务A", 3);
    Simple_xTaskCreate(test_task, "Task_B", "Hello任务B", 2);

    printf("\n当前任务列表:\n");
    for (int i = 0; i < task_count; i++) {
        printf("  任务名=%s, 优先级=%d\n",
               task_list[i].name, task_list[i].priority);
    }

    printf("\n// 这就是FreeRTOS xTaskCreate的内部原理！\n");
    printf("// 你传入的函数指针和参数被存到TCB(Task Control Block)里\n");
    printf("// 调度器根据优先级决定下一个运行哪个任务\n\n");
}

// ========== 主函数 ==========
int main(void) {
    printf("========================================\n");
    printf("  韦富宏 - C语言 Day 3: 指针深入\n");
    printf("========================================\n\n");

    ex1_pointer_basics();
    ex2_pointer_array();
    ex3_function_pointer();
    ex4_void_pointer();
    ex5_dynamic_memory();
    ex6_common_bugs();
    ex7_freertos_simulation();

    printf("========================================\n");
    printf("  Day 3 完成!\n");
    printf("  指针 = 嵌入式程序员的基本功\n");
    printf("  函数指针/void* = FreeRTOS的基石\n");
    printf("========================================\n");

    return 0;
}
