/**
 * @file debug.h
 * @author Zach (zachary-yue@qq.com)
 * 
 * @brief 本文件是一个用户可修改的文件。
 *        实例化一个 debug 模块，实现系统日志输出功能，实现 debug 相关的回调函数等。
 * 
 *        用户需要第一个初始化 debug 模块，并且把回调函数加入串口中断回调里（如果使用 UART 作为 Debug 模块的输出）。
 *        如果在没有先初始化 debug 模块的情况下使用了日志打印宏，那么日志打印宏会自己检查 debug 模块是否初始化，
 *        如果没有初始化就不打印日志。
 *        需要指出的是，检查 debug 是否初始化的方式是判断 debug 句柄里的 is_init 字段是否为 true，
 *        在初始化之前这个字段的值是未知的，如果恰好是 true，那么日志打印宏就会误以为 debug 模块已经初始化了，
 *        从而调用了一个未初始化的函数指针来打印日志，这时就会出问题。虽然概率很小，
 *        但是用户还是要注意在使用日志打印宏之前先初始化 debug 模块。
 * 
 * @version 0.1
 * @date 2026-04-30
 * 
 */
#pragma once

#include "debug_module.h"
#include "debug_static.h"

#define USE_DXBT37_AS_DEBUG_OUTPUT USE_DXBT37_AS_DEBUGM_OUTPUT

// Must be initialized by user by calling debug_start()
// before using LOGI, LOGW, LOGE macros.
extern debugm_handle_t hdebug;
extern debug_static_handle_t hdebug_static;

// 需要第一个初始化 debug 模块
void debug_init(void);

#define debug_send() debugm_send(&hdebug)
#define debug_recv() debugm_recv(&hdebug)
#define debug_send_all() debugm_send_all(&hdebug)

// 需要加入串口接收完成中断回调函数
#define debug_rx_it_handler(len) debugm_rx_cplt_handler(&hdebug, len)

/*-------------------------------- Log -------------------------------*/

#ifndef LOG_ENABLE
#define LOG_ENABLE 1    // whether enable log output using LOGI, LOGW, LOGE macros.
#endif

#if LOG_ENABLE
    #define LOGI(tag, fmt, ...) DEBUGM_LOGI(&hdebug, tag, fmt, ##__VA_ARGS__)
    #define LOGW(tag, fmt, ...) DEBUGM_LOGW(&hdebug, tag, fmt, ##__VA_ARGS__)
    #define LOGE(tag, fmt, ...) DEBUGM_LOGE(&hdebug, tag, fmt, ##__VA_ARGS__)
    #ifdef DEBUG
        #define LOGD(tag, fmt, ...) DEBUGM_LOGD(&hdebug, tag, fmt, ##__VA_ARGS__)
    #else
        #define LOGD(...)
    #endif
#else
    #define LOGI(...)
    #define LOGW(...)
    #define LOGE(...)
    #define LOGD(...)
#endif

/*-------------------------------- Static Log -------------------------------*/

#define SLOGI(tag, fmt, ...) DEBUG_S_LOGI(&hdebug_static, tag, fmt, ##__VA_ARGS__)
#define SLOGW(tag, fmt, ...) DEBUG_S_LOGW(&hdebug_static, tag, fmt, ##__VA_ARGS__)
#define SLOGE(tag, fmt, ...) DEBUG_S_LOGE(&hdebug_static, tag, fmt, ##__VA_ARGS__)

#define debug_get_slog_fifo_used_size()  user_fifo_get_used_length(&(hdebug_static.log_fifo))
#define debug_s_read_logs(buf, len) debug_static_read_logs(&hdebug_static, buf, len)
#define debug_s_dump_logs(len)      debug_static_dump_logs(&hdebug_static, len)
#define debug_s_clear_logs()        debug_static_clear_logs(&hdebug_static)
