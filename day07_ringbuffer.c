/**
 * Day 7: 环形缓冲区（Ring Buffer）—— 嵌入式最经典的数据结构
 * 日期: 2026-07-25
 *
 * 编译运行:
 *   gcc -Wall day07_ringbuffer.c -o day07_ringbuffer.exe
 *   ./day07_ringbuffer.exe
 *
 * 应用场景:
 *   - 串口接收缓冲(UART RX)
 *   - 传感器数据采集缓冲
 *   - FreeRTOS StreamBuffer 的底层实现
 *   - 音频/视频流缓冲
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ========== Ring Buffer 数据结构 ==========
typedef struct {
    uint8_t *buffer;      // 数据存储区
    uint32_t size;        // 缓冲区总大小
    uint32_t head;        // 写指针(生产者)
    uint32_t tail;        // 读指针(消费者)
    uint32_t count;       // 当前存储的数据量
} RingBuffer_t;

// ========== API 函数 ==========

// 初始化环形缓冲区
void rb_init(RingBuffer_t *rb, uint32_t size) {
    rb->buffer = (uint8_t*)malloc(size);
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

// 销毁
void rb_destroy(RingBuffer_t *rb) {
    free(rb->buffer);
    rb->buffer = NULL;
    rb->size = 0;
}

// 判断是否满
bool rb_is_full(RingBuffer_t *rb) {
    return rb->count == rb->size;
}

// 判断是否空
bool rb_is_empty(RingBuffer_t *rb) {
    return rb->count == 0;
}

// 获取已用空间
uint32_t rb_used(RingBuffer_t *rb) {
    return rb->count;
}

// 获取剩余空间
uint32_t rb_free(RingBuffer_t *rb) {
    return rb->size - rb->count;
}

// ========== 核心: 写入数据 ==========
// 返回实际写入的字节数
uint32_t rb_write(RingBuffer_t *rb, const uint8_t *data, uint32_t len) {
    if (rb_is_full(rb)) {
        return 0;  // 满了，写不进去
    }

    // 最多写 free 个字节
    if (len > rb_free(rb)) {
        len = rb_free(rb);
    }

    // 判断是否会绕回(circular!)
    if (rb->head + len <= rb->size) {
        // 情况1: 不绕回 [  tail....head|-->|         ]
        memcpy(rb->buffer + rb->head, data, len);
    } else {
        // 情况2: 绕回    [-->|     tail....head|-->|  ]
        uint32_t first_part = rb->size - rb->head;
        memcpy(rb->buffer + rb->head, data, first_part);
        memcpy(rb->buffer, data + first_part, len - first_part);
    }

    rb->head = (rb->head + len) % rb->size;  // 取模实现环形移动
    rb->count += len;
    return len;
}

// ========== 核心: 读取数据 ==========
// 返回实际读取的字节数
uint32_t rb_read(RingBuffer_t *rb, uint8_t *out, uint32_t len) {
    if (rb_is_empty(rb)) {
        return 0;  // 空了，没数据
    }

    if (len > rb->count) {
        len = rb->count;
    }

    if (rb->tail + len <= rb->size) {
        // 不绕回
        memcpy(out, rb->buffer + rb->tail, len);
    } else {
        // 绕回
        uint32_t first_part = rb->size - rb->tail;
        memcpy(out, rb->buffer + rb->tail, first_part);
        memcpy(out + first_part, rb->buffer, len - first_part);
    }

    rb->tail = (rb->tail + len) % rb->size;
    rb->count -= len;
    return len;
}

// ========== 练习演示 ==========

// 模拟传感器结构
typedef struct {
    uint32_t timestamp;
    float temperature;
    float humidity;
} SensorSample_t;

void demo_byte_buffer(void) {
    printf("===== 演示1: 字节级环形缓冲 =====\n");

    RingBuffer_t rb;
    rb_init(&rb, 16);  // 16字节的小缓冲

    printf("缓冲大小: %u 字节\n\n", rb.size);

    // 写入
    const char *msg = "Hello ESP32!";
    uint32_t written = rb_write(&rb, (uint8_t*)msg, strlen(msg));
    printf("写入 \"%s\" (%u字节)\n", msg, written);
    printf("  head=%u, tail=%u, count=%u\n\n", rb.head, rb.tail, rb.count);

    // 读取
    char out[32] = {0};
    uint32_t read = rb_read(&rb, (uint8_t*)out, sizeof(out) - 1);
    printf("读取 %u 字节: \"%s\"\n", read, out);

    rb_destroy(&rb);
}

void demo_sensor_buffer(void) {
    printf("===== 演示2: 传感器数据环形缓冲 =====\n");

    // 存10个传感器采样
    RingBuffer_t rb;
    rb_init(&rb, 10 * sizeof(SensorSample_t));

    printf("缓冲容量: 10 个采样点\n\n");

    // 生产者: 模拟传感器采集(100Hz)
    printf("--- 模拟采集5个采样 ---\n");
    for (int i = 0; i < 5; i++) {
        SensorSample_t sample;
        sample.timestamp = 1000 + i * 10;  // 每10ms一个
        sample.temperature = 25.0f + i * 0.5f;
        sample.humidity = 60.0f + i;

        rb_write(&rb, (uint8_t*)&sample, sizeof(SensorSample_t));
        printf("采集 #%d: 时间=%u, 温度=%.1f, 湿度=%.1f | 缓冲使用=%u/%u\n",
               i + 1, sample.timestamp, sample.temperature,
               sample.humidity, rb_used(&rb), rb.size);
    }

    // 消费者: 模拟数据处理(50Hz, 比采集慢)
    printf("\n--- 模拟处理3个采样(速度比采集慢) ---\n");
    for (int i = 0; i < 3; i++) {
        SensorSample_t sample;
        if (rb_read(&rb, (uint8_t*)&sample, sizeof(SensorSample_t)) > 0) {
            printf("处理 #%d: 时间=%u, 温度=%.1f, 湿度=%.1f | 缓冲剩余=%u\n",
                   i + 1, sample.timestamp, sample.temperature,
                   sample.humidity, rb_used(&rb));
        }
    }

    // 这时缓冲里还有2个采样没处理——这就是缓冲的作用！
    printf("\n缓冲里还有 %llu 个采样等待处理(生产者比消费者快!)\n\n",
           (unsigned long long)(rb_used(&rb) / sizeof(SensorSample_t)));

    printf("// 关键理解:\n");
    printf("// 传感器产生数据 = 生产者(快,100Hz)\n");
    printf("// 数据处理/显示    = 消费者(慢,50Hz)\n");
    printf("// Ring Buffer 吸收了速度差, 没有丢数据!\n");
    printf("// 这在FreeRTOS项目里就是 SensorTask → Queue → ProcessTask\n\n");

    rb_destroy(&rb);
}

void demo_full_and_empty(void) {
    printf("===== 演示3: 满和空的处理 =====\n");

    RingBuffer_t rb;
    rb_init(&rb, 8);

    // 写到满
    printf("写入9个字节到8字节缓冲:\n");
    uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (int i = 0; i < 9; i++) {
        uint32_t w = rb_write(&rb, &data[i], 1);
        printf("  第%d次: 写%u字节, 缓冲使用=%u",
               i + 1, w, rb_used(&rb));
        if (w == 0) printf(" ← 满了!拒绝写入!");
        printf("\n");
    }

    // 读到空
    printf("\n读取9个字节:\n");
    for (int i = 0; i < 9; i++) {
        uint8_t val;
        uint32_t r = rb_read(&rb, &val, 1);
        if (r > 0) {
            printf("  第%d次: 读到 %u, 缓冲剩余=%u\n",
                   i + 1, val, rb_used(&rb));
        } else {
            printf("  第%d次: 空了!无数据可读!\n", i + 1);
        }
    }

    rb_destroy(&rb);
}

// ========== 主函数 ==========
int main(void) {
    printf("========================================\n");
    printf("  韦富宏 - C语言 Day 7: 环形缓冲区\n");
    printf("  嵌入式最经典的数据结构\n");
    printf("========================================\n\n");

    demo_byte_buffer();
    printf("\n---\n\n");
    demo_sensor_buffer();
    printf("---\n\n");
    demo_full_and_empty();

    printf("\n========================================\n");
    printf("  Day 7 完成! C语言7天速通结束!\n");
    printf("\n");
    printf("  7天回顾:\n");
    printf("  Day1: 数据类型 + 位运算 + 控制流\n");
    printf("  Day2: 函数 + 作用域 + static/extern\n");
    printf("  Day3: 指针 + 函数指针 + void*\n");
    printf("  Day4: 栈vs堆 + malloc/free + 内存碎片\n");
    printf("  Day5: 结构体 + 对齐 + 位域 + 枚举\n");
    printf("  Day6: volatile + const + 中断 + 位宏\n");
    printf("  Day7: 环形缓冲区(生产者-消费者模式)\n");
    printf("\n");
    printf("  这些就是嵌入式C的全部核心知识。\n");
    printf("  接下来: 在ESP32上实战!\n");
    printf("========================================\n");

    return 0;
}
