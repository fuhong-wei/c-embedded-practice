/**
 * 01_freertos_hello - 第一个FreeRTOS多任务程序
 *
 * 目标：理解FreeRTOS任务创建、优先级、延时
 * 板子：YD-ESP32-S3
 *
 * 三个任务：
 *   Task_Fast   - 优先级3, 每500ms打印一次
 *   Task_Medium - 优先级2, 每1000ms打印一次
 *   Task_Slow   - 优先级1, 每2000ms打印一次
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 任务1：快速任务，优先级最高
void Task_Fast(void *pvParameters) {
    int count = 0;
    while (1) {
        count++;
        printf("[FAST]   第%d次, 优先级=3, 间隔=500ms\n", count);
        vTaskDelay(500 / portTICK_PERIOD_MS);  // 延时500ms, 让出CPU
    }
}

// 任务2：中速任务
void Task_Medium(void *pvParameters) {
    int count = 0;
    while (1) {
        count++;
        printf("[MEDIUM] 第%d次, 优先级=2, 间隔=1000ms\n", count);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// 任务3：慢速任务，优先级最低
void Task_Slow(void *pvParameters) {
    int count = 0;
    while (1) {
        count++;
        printf("[SLOW]   第%d次, 优先级=1, 间隔=2000ms\n", count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

// ESP-IDF 的入口函数（相当于 main()，但由FreeRTOS调用）
void app_main(void) {
    printf("\n");
    printf("========================================\n");
    printf("  韦富宏 - FreeRTOS 多任务练习 Day 1\n");
    printf("  ESP32-S3 + ESP-IDF v5.3\n");
    printf("========================================\n\n");

    // 创建三个任务
    // 参数：任务函数, 任务名, 栈大小, 参数, 优先级, 任务句柄, 核心
    xTaskCreate(Task_Fast,   "Task_Fast",   2048, NULL, 3, NULL);
    xTaskCreate(Task_Medium, "Task_Medium", 2048, NULL, 2, NULL);
    xTaskCreate(Task_Slow,   "Task_Slow",   2048, NULL, 1, NULL);

    printf("3个FreeRTOS任务已创建\n");
    printf("Fast(3)每500ms | Medium(2)每1000ms | Slow(1)每2000ms\n");
    printf("观察：优先级高的任务会抢占低优先级的任务吗？\n");
    printf("答案:不会,因为每个任务都有vTaskDelay,主动让出CPU\n\n");
}
