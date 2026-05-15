/**
 * @file ascii_1608.h
 * @author Zach (zachary-yue@qq.com)
 * 
 * @brief This is a fundamental font for OLED display, which contains 95 ascii characters.
 *        Each character is 8 pixels wide and 16 pixels high.
 * 
 * @version 0.1
 * @date 2026-05-13
 * 
 */
#pragma once

#include "user_def.h"

void oled_show_char(u8 row, u8 col, char chr);
void oled_show_string(u8 row, u8 col, const char *str);
void oled_show_int(u8 row, u8 col, int num, u8 len, bool zero_pad);
