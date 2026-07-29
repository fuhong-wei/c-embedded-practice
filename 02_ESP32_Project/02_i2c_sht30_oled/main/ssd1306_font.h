/**
 * ssd1306_font.h - 5x7 像素字体
 *
 * 覆盖: ASCII 0x20(空格) ~ 0x7E(~)
 * 每个字符: 5列×7行 = 5字节 (垂直排列, 高位在上)
 */

#pragma once

#include <stdint.h>

// 字体表: 95个字符 × 5字节/字符
extern const uint8_t font5x7[95][5];
