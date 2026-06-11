#ifndef __ICON16_H
#define __ICON16_H

#include "user_def.h"

#define Icon16_CHECK_MARK           0
#define Icon16_EMPTY_SQUARE         1
#define Icon16_WATCH                2
#define Icon16_ALARM_CLOCK          3
#define Icon16_MUSIC_NOTE           4
#define Icon16_LOW_BATTERY          5

void oled_show_icon16(u8 row, u8 col, u8 index);

#endif // !__ICON16_H
