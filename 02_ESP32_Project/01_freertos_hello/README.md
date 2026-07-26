# 01_freertos_hello — FreeRTOS 多任务入门

## 目标

验证 FreeRTOS 多任务创建和调度：
- 3个任务以不同频率运行（500ms / 1000ms / 2000ms）
- 理解任务优先级和vTaskDelay的协作调度

## FreeRTOS 任务架构

```
Task_Fast(优先级3) ── 每500ms打印 ──┐
                                      │
Task_Medium(优先级2) ─ 每1000ms打印 ──├── 协作调度（都有vTaskDelay）
                                      │
Task_Slow(优先级1) ─── 每2000ms打印 ─┘
```

## 关键代码

```c
xTaskCreate(Task_Fast,   "Task_Fast",   2048, NULL, 3, NULL);
//           ↑任务函数    ↑任务名       ↑栈    ↑参数 ↑优先级
```

## 运行结果

```
========================================
  韦富宏 - FreeRTOS 多任务练习 Day 1
  ESP32-S3 + ESP-IDF v5.3
========================================

[FAST]   第1次, 优先级=3, 间隔=500ms
[FAST]   第2次, 优先级=3, 间隔=500ms
[MEDIUM] 第1次, 优先级=2, 间隔=1000ms
[FAST]   第3次, 优先级=3, 间隔=500ms
[SLOW]   第1次, 优先级=1, 间隔=2000ms
...
```

## 学到了什么

- ESP-IDF 项目结构和编译流程
- FreeRTOS `xTaskCreate()` 参数含义
- `vTaskDelay()` 让出CPU的协作机制
- 任务优先级不影响"谁先运行"，因为每个任务都主动让出CPU
- `portTICK_PERIOD_MS` = 系统滴答周期(FreeRTOS的时间单位)
