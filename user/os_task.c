#include "os_task.h"

os_task_e this_task_ = 0;			   // 当前运行的任务全局标志
u8 task_for_cnt_ = 0;				   // 使用 task_for 时所需的临时变量
os_task_t user_tasks_[TASK_MAX] = {0}; // 每个协程的运行记忆变量

// 函数说明：用于设定软定时器的时间基准，需要每隔 1ms (推荐时间) 调用一次
void os_task_update(void)
{
	u8 task_num;										// 临时变量
	for (task_num = 0; task_num < TASK_MAX; task_num++) // 遍历所有的软定时器
		if (user_tasks_[task_num].time > 0)
			user_tasks_[task_num].time--; // 需要自减的减 1
}
