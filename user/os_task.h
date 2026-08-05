#ifndef OS_TASK_H
#define OS_TASK_H

#include "main.h"

typedef enum task_e
{
	TASK_TEMP,
	TASK_DISPLAY,
	TASK_DISPLAY_FPS,
	TASK_BATTERY,
	TASK_BATTERY_REPORT_LP,

	TASK_MAX
} task_e;

// 每个任务的数据结构
typedef struct
{
	u16 time;
	u32 state;
} task_t;

extern task_e this_task_;			 // 当前运行的任务全局标志
extern u8 task_for_cnt_;			 // 使用 task_for 时所需的临时变量
extern task_t user_tasks_[TASK_MAX]; // 每个协程的运行记忆变量

// 用于设置定时器的时间基准，需要每 1ms 调用一次（推荐），可以设置为其他基准
// 如果设置为 1s 调用一次，那么 task_delay(1) 就变成 1s 的延时了，以此类推
void os_task_update(void);

// 初始化线程，调用前必备
#define task_start(task_num)                   \
	this_task_ = (task_e)task_num;             \
	if (task_num < TASK_MAX)                   \
		switch (user_tasks_[this_task_].state) \
		{                                      \
		case 0:

// 等待表达式变成真
#define task_wait(ifx)                        \
	user_tasks_[this_task_].state = __LINE__; \
	case __LINE__:                            \
		if (LIKELY(!(ifx)))                   \
			break;

// 延时后继续向下，内部填入延时ms时间，等待时间后继续向下
#define task_delay(delay_ms)                           \
	user_tasks_[this_task_].state = __LINE__;          \
	user_tasks_[this_task_].time = delay_ms;           \
	case __LINE__:                                     \
		if (LIKELY(user_tasks_[this_task_].time != 0)) \
			break;

// 传入两个表达式，init 是初始化(仅调用一次)，cnt 是自增自减表达式(每次运行都调用一次( task_for_end 返回来也算))
#define task_for(init, cnt)                   \
	init;                                     \
	user_tasks_[this_task_].state = __LINE__; \
	case __LINE__:                            \
		for (task_for_cnt_ = 1; task_for_cnt_--; cnt, user_tasks_[this_task_].state = __LINE__)

// while(1) 循环，判断条件通过 task_break 进行判断跳出
#define task_while(ifx)                       \
	user_tasks_[this_task_].state = __LINE__; \
	case __LINE__:                            \
		for (task_for_cnt_ = 1; (task_for_cnt_--) && (ifx); user_tasks_[this_task_].state = __LINE__)

// 对 ifx 的条件进行判断，为 1 则回到 task_for/task_while 的开始，为 0 则往下继续执行
#define task_loop(ifx) \
	if (ifx)           \
		break;

// 循环体内跳过本轮剩余代码，下一轮从循环头继续（语义等同 C 的 continue）。
// 只能在 task_for / task_while 的循环体内使用。
#define task_continue() \
	continue;

// 循环体内立即结束循环，任务停在此处（不再回到循环头）。
// 注意：不能在循环体内使用裸 break —— 裸 break 会跳过 state 更新，下次调用
// 会从体内最后一个挂起点恢复而不是循环头。中途退出循环用本宏，
// 循环体末尾的条件退出仍用 task_loop(ifx)。
#define task_break()                          \
	user_tasks_[this_task_].state = __LINE__; \
	case __LINE__:                            \
		break;

// 线程结束，内部填入是否重新开始线程，1 为重新开始，0 为结束等待
#define task_end(reload)                          \
	if (reload)                                   \
		user_tasks_[this_task_].state = 0;        \
	else                                          \
		user_tasks_[this_task_].state = __LINE__; \
	break;                                        \
	}

#endif // OS_TASK_H
