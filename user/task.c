

#include "task.h"
#include "system.h"
#include "light.h"
#include "display.h"

typedef struct
{
    const u16   period;
    u16         count;
    void        (*task_hook)(void);
} Task_t;


// 任务总数。每添加一个任务，记得修改这个宏定义
#define TASK_TOTAL      3

// 任务列表
Task_t Task[TASK_TOTAL] =
{
    // ms, count, taskHook
    { 1000, 0, light_task },
    { 1000, 0, display_reinit_timer },
    { 100, 0, display_task },
};


/**
 * @brief Increment task counters. This function should be called
 *        in the SysTick_Handler or a timer interrupt handler.
 * 
 */
void task_counter_inc(void)
{
    u8 i = 0;
    for (; i < TASK_TOTAL; i++)
    {
        if (Task[i].count < Task[i].period)
        {
            Task[i].count++;
        }
    }
}


/**
 * @brief Execute tasks. This function should be called in the main loop.
 * 
 */
void task_exe(void)
{
    u8 i = 0;
    for (; i < TASK_TOTAL; i++)
    {
        if (Task[i].count >= Task[i].period)
        {
            Task[i].count = 0;
            if (Task[i].task_hook != NULL)
            {
                Task[i].task_hook();
            }
        }
    }
}
