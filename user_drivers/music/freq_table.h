
#pragma once

#include "main.h"

#define MUSIC_TIM_PSC 2 // e.g. PSC = 2 means timer freq = 72MHz / (PSC + 1) = 24MHz

u16 Get_Freq(u8 index);
