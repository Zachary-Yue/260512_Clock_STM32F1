
#include "system.h"

/**
 * @brief 复位整个系统，进入 Bootloader 模式。
 *
 */
void sys_reset(void)
{
    __set_FAULTMASK(1); // 禁止所有中断
    NVIC_SystemReset();
}

/**
 * @brief us 延时。CPU 主频须小于 4.29 GHz，否则会溢出。
 *
 * @param us 范围 1 ~ 1,000,000（1秒）， 超过 1,000,000 的值会被限制为 1,000,000。
 */
void sys_delay_us(u32 us)
{
    if (UNLIKELY(us == 0))
        return;
    if (UNLIKELY(us > 1000000UL))
        us = 1000000UL;
    u32 cycles = us * (SystemCoreClock / 1000000UL);
    u32 reload = SysTick->LOAD + 1;
    u32 start = SysTick->VAL;
    u32 elapsed = 0;
    while (elapsed < cycles)
    {
        u32 current = SysTick->VAL;
        if (current != start)
        {
            if (current < start)
            {
                elapsed += start - current;
            }
            else
            {
                elapsed += reload - current + start;
            }
            start = current;
        }
    }
}
