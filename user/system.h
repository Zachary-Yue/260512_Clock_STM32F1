
#pragma once

#include "main.h"

extern u32 sys_primask_;
extern u32 sys_critical_count_;

#define SYS_ENTER_CRITICAL()  \
    if (sys_critical_count_ == 0) { \
        sys_primask_ = __get_PRIMASK(); \
        __disable_irq(); \
    } \
    sys_critical_count_++;

#define SYS_EXIT_CRITICAL()   \
    if (sys_critical_count_ > 0) { \
        sys_critical_count_--; \
        if (sys_critical_count_ == 0) { \
            __set_PRIMASK(sys_primask_); \
        } \
    }

void sys_init(void);
void sys_task(void);
void sys_reset(void);
void sys_delay_us(u32 us);
