
#include "system.h"
#include "task.h"
#include "sys_err.h"
#include "tim.h"
#include "rtc.h"

#include "bt.h"
#include "display.h"
#include "app_music.h"
#include "app_clock.h"
#include "user_phr.h"

#define TAG "SYSTEM"

u32 sys_primask_ = 0UL;
u32 sys_critical_count_ = 0;

static void systick_init(void)
{
    // 1ms 中断
    SysTick_Config(SystemCoreClock / 1000);
}

void sys_init(void)
{
    systick_init();
    user_phr_init();
    debug_init();
    bt_init();
    display_init();
    app_music_init();
    Clock_Init();
    LOGI(TAG, "System init finished.");
}

void sys_task(void)
{
    task_exe();
    debug_send();
    debug_recv();
    bt_send();
    bt_recv();
}

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

#if 0
/*--------------------------------- HAL Callback Functions --------------------------------*/

// 串口 DMA + 空闲中断接收完成回调函数
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    HAL_UART_RxEventTypeTypeDef ev = HAL_UARTEx_GetRxEventType(huart);

    if (ev == HAL_UART_RXEVENT_IDLE || ev == HAL_UART_RXEVENT_TC)
    {
        if (huart == hdebug.huart) {
            debug_rx_it_handler(Size);
        }
        else if (huart == bt37.huart) {
            bt_rx_cplt_handler(Size);
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim1) {
        app_music_task();
    }
}

void HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef *arg_hrtc)
{
    if (arg_hrtc == &hrtc) {
        Clock_Inc1s_Handler();
    }
}
#endif
