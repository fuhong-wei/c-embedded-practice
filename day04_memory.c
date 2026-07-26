/**
 * Day 4: 内存管理 —— 栈 vs 堆
 * 日期: 2026-07-25
 *
 * 编译运行:
 *   gcc -Wall day04_memory.c -o day04_memory.exe
 *   ./day04_memory.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========== 核心概念: 栈 vs 堆 ==========
/*
 * 栈(Stack) ── 自动管理，函数调用时分配，返回时释放
 *   - 快
 *   - 大小有限(MCU上通常几KB)
 *   - 局部变量、函数参数在这里
 *
 * 堆(Heap)  ── 手动管理，malloc申请，free释放
 *   - 慢
 *   - 可以很大(取决于系统)
 *   - 但会产生内存碎片！
 *
 * 嵌入式开发铁律: 尽量用栈，少用堆！
 */

// ========== 练习1: 栈上分配 vs 堆上分配 ==========
void ex1_stack_vs_heap(void) {
    printf("===== 练习1: 栈 vs 堆 =====\n");

    // 栈上分配: 出了函数自动释放
    int stack_var = 100;
    printf("栈变量地址(高地址): %p\n", (void*)&stack_var);

    // 堆上分配: 不free就一直占着
    int *heap_var = (int*)malloc(sizeof(int));
    *heap_var = 200;
    printf("堆变量地址(低地址): %p\n", (void*)heap_var);
    printf("堆变量值: %d\n", *heap_var);

    // 嵌入式关键: 别忘了free!
    free(heap_var);

    printf("\n// 栈地址和堆地址一个高一个低，方向相反\n");
    printf("// 这就是\"堆栈溢出\"——栈向下长，堆向上长，撞到一起就溢出\n\n");
}

// ========== 练习2: malloc/free 的正确姿势 ==========
void ex2_malloc_free(void) {
    printf("===== 练习2: malloc/free 正确用法 =====\n");

    // 1. malloc 要检查返回值!
    int *arr = (int*)malloc(10 * sizeof(int));
    if (arr == NULL) {
        printf("ERROR: 内存分配失败!\n");
        return;
    }

    // 2. 用数据
    for (int i = 0; i < 10; i++) {
        arr[i] = i * i;
    }
    printf("动态数组: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // 3. free 后置NULL —— 防止野指针
    free(arr);
    arr = NULL;  // 好习惯！

    printf("\n// 三条规则:\n");
    printf("// ① malloc必须检查返回值是否为NULL\n");
    printf("// ② 每个malloc必须对应一个free\n");
    printf("// ③ free后把指针置为NULL，防止被误用\n\n");
}

// ========== 练习3: 内存泄漏演示 ==========
// 警告: 这是故意演示bug的代码！
void ex3_memory_leak_demo(void) {
    printf("===== 练习3: 什么是内存泄漏 =====\n");

    printf("这段代码如果运行10000次:\n");
    printf("  for (int i = 0; i < 10000; i++) {\n");
    printf("      char *p = malloc(1024);  // 每次申请1KB\n");
    printf("      // 忘记free(p); ← 泄漏!\n");
    printf("  }\n");
    printf("  结果: 10MB内存被吃掉了，系统OOM\n\n");

    printf("嵌入式里更可怕:\n");
    printf("  MCU总共只有320KB RAM\n");
    printf("  泄漏几十次就死机了\n");
    printf("  → 所以MCU开发中几乎不用malloc\n");
    printf("  → FreeRTOS有专门的 pvPortMalloc，但也要小心\n\n");
}

// ========== 练习4: calloc 和 realloc ==========
void ex4_calloc_realloc(void) {
    printf("===== 练习4: calloc 和 realloc =====\n");

    // calloc: 分配+初始化为0
    int *arr1 = (int*)calloc(5, sizeof(int));
    printf("calloc 分配后: ");
    for (int i = 0; i < 5; i++) printf("%d ", arr1[i]);  // 全0
    printf("(自动初始化为0)\n");

    // realloc: 调整已分配内存的大小
    int *arr2 = (int*)realloc(arr1, 8 * sizeof(int));  // 5→8个int
    if (arr2 == NULL) {
        printf("realloc失败!\n");
        free(arr1);
        return;
    }
    arr2[5] = 100; arr2[6] = 200; arr2[7] = 300;
    printf("realloc扩展到8个: ");
    for (int i = 0; i < 8; i++) printf("%d ", arr2[i]);
    printf("\n");

    free(arr2);
}

// ========== 练习5: 内存碎片演示 ==========
void ex5_fragmentation_demo(void) {
    printf("===== 练习5: 内存碎片（嵌入式大敌） =====\n");
    printf("假设有320KB堆:\n\n");
    printf("Step1: malloc(100KB) → [AAAA...................]\n");
    printf("Step2: malloc(80KB)  → [AAAA.BBB...............]\n");
    printf("Step3: free(AAAA)    → [....BBBB...............]\n");
    printf("Step4: malloc(80KB)  → [CCCCBBBB...............]\n");
    printf("Step5: free(BBBB)    → [CCCC...................]\n");
    printf("Step6: malloc(150KB) → 失败!没有连续150KB!\n\n");
    printf("总空闲=200KB，但最大连续只有100KB → 碎片化了\n");
    printf("// 这就是为什么MCU开发倾向于静态分配\n\n");
}

// ========== 练习6: 内存拷贝函数 ==========
void ex6_mem_functions(void) {
    printf("===== 练习6: memcpy/memset/memmove =====\n");

    // memcpy: 内存拷贝（源和目标不能重叠）
    int src[5] = {1, 2, 3, 4, 5};
    int dst[5];
    memcpy(dst, src, 5 * sizeof(int));
    printf("memcpy 后 dst: ");
    for (int i = 0; i < 5; i++) printf("%d ", dst[i]);
    printf("\n");

    // memset: 内存填充
    char buf[20];
    memset(buf, 'A', 19);
    buf[19] = '\0';
    printf("memset 后 buf: %s\n", buf);

    // memcmp: 内存比较
    int arr_a[3] = {1, 2, 3};
    int arr_b[3] = {1, 2, 4};
    int result = memcmp(arr_a, arr_b, 3 * sizeof(int));
    printf("memcmp(arr_a, arr_b) = %d (负数=a<b, 0=相等, 正数=a>b)\n", result);

    printf("\n// 这三个函数在驱动开发中非常常用\n");
    printf("// 特别是memcpy，SPI/I2C收发数据必备\n\n");
}

// ========== 主函数 ==========
int main(void) {
    printf("========================================\n");
    printf("  韦富宏 - C语言 Day 4: 内存管理\n");
    printf("========================================\n\n");

    ex1_stack_vs_heap();
    ex2_malloc_free();
    ex3_memory_leak_demo();
    ex4_calloc_realloc();
    ex5_fragmentation_demo();
    ex6_mem_functions();

    printf("========================================\n");
    printf("  Day 4 完成!\n");
    printf("  核心: 栈快但小(MCU几KB)，堆慢但大但碎片化\n");
    printf("  嵌入式铁律: 能静态分配就别malloc\n");
    printf("========================================\n");

    return 0;
}
