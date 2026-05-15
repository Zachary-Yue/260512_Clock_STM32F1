#ifndef __FONT_NUM3216_H
#define __FONT_NUM3216_H

#include "user_def.h"

typedef enum {
    FONT_NUM3216_1 = 1, // MS Serif (Like Song font, print-like)
    FONT_NUM3216_2,     // MS Sans Serif (Like Hei font, print-like)
    FONT_NUM3216_3,     // MS Gothic (Like Kai font, print-like)
    FONT_NUM3216_4,     // MS Mincho (Like Fangsong font, print-like)
    FONT_COLON3216,     // Colon (for time display)
    FONT_NUM3216_MAX
} font_num3216_index_t;

void oled_show_num3216(u8 row, u8 col, u8 num, font_num3216_index_t font_index);

#endif // !__FONT_NUM3216_H
