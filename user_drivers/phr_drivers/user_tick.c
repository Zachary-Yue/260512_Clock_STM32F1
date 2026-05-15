
#include "user_tick.h"

static volatile u32 tick_count = 0;

/**
 * @brief put it in SysTick_Handler, and it will count the tick for you.
 *        frequency of systick is assumed to be 1 kHz.
 * 
 */
void user_tick_handler(void)
{
    tick_count++;
}

u32 user_get_tick(void)
{
    return tick_count;
}
