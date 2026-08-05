
#pragma once

#include "main.h"

#define LED0_Pinout     PCout(13)
#define LED0_On()       (LED0_Pinout = Bit_RESET)
#define LED0_Off()      (LED0_Pinout = Bit_SET)
#define LED0_Toggle()   (LED0_Pinout ^= Bit_SET)

void light_task(void);
