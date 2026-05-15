/**
 * @file sys_err.h
 * @author Zach (zachary-yue@qq.com)
 * 
 * @brief 实现常用的错误处理，其中日志打印部分使用 debug 模块提供的 API。
 * 
 * @version 0.1
 * @date 2026-04-30
 * 
 */
#pragma once

#include "debug.h"

/*-------------------------------- Error Check -------------------------------*/

/**
 * APIs:
 * 
 * ERR_CHECK(ret)
 * 
 * CHECK_FALSE_RET(expr, ret_val)
 * CHECK_FALSE_GOTO(expr, label)
 * CHECK_FALSE_RET_VOID(expr)
 * 
 * CHECK_FALSE_RET_LOG(expr, ret_val, tag, fmt, ...)
 * CHECK_FALSE_GOTO_LOG(expr, label, tag, fmt, ...)
 * CHECK_FALSE_RET_VOID_LOG(expr, tag, fmt, ...)
 */

void sys_error_handler(void);

#define ERR_CHECK(expr) do { \
    if (UNLIKELY(!(expr))) { \
        sys_error_handler(); \
    } \
} while (0)

#ifndef CHECK_FALSE_RET
    #define CHECK_FALSE_RET(expr, ret_val) do { \
        if (UNLIKELY(!(expr))) { \
            return (ret_val); \
        } \
    } while (0)
#endif // !CHECK_FALSE_RET

#ifndef CHECK_FALSE_GOTO
    #define CHECK_FALSE_GOTO(expr, label) do { \
        if (UNLIKELY(!(expr))) { \
            goto label; \
        } \
    } while (0)
#endif // !CHECK_FALSE_GOTO

#ifndef CHECK_FALSE_RET_VOID
    #define CHECK_FALSE_RET_VOID(expr) do { \
        if (UNLIKELY(!(expr))) { \
            return; \
        } \
    } while (0)
#endif // !CHECK_FALSE_RET_VOID

#if defined(LOG_ENABLE) && LOG_ENABLE == 1
    #ifndef CHECK_FALSE_RET_LOG
        #define CHECK_FALSE_RET_LOG(expr, ret_val, tag, fmt, ...) do { \
            if (UNLIKELY(!(expr))) { \
                LOGE(tag, fmt, ##__VA_ARGS__); \
                return (ret_val); \
            } \
        } while (0)
    #endif // !CHECK_FALSE_RET_LOG

    #ifndef CHECK_FALSE_GOTO_LOG
        #define CHECK_FALSE_GOTO_LOG(expr, label, tag, fmt, ...) do { \
            if (UNLIKELY(!(expr))) { \
                LOGE(tag, fmt, ##__VA_ARGS__); \
                goto label; \
            } \
        } while (0)
    #endif // !CHECK_FALSE_GOTO_LOG

    #ifndef CHECK_FALSE_RET_VOID_LOG
        #define CHECK_FALSE_RET_VOID_LOG(expr, tag, fmt, ...) do { \
            if (UNLIKELY(!(expr))) { \
                LOGE(tag, fmt, ##__VA_ARGS__); \
                return; \
            } \
        } while (0)
    #endif// !CHECK_FALSE_RET_VOID_LOG
#else
    #ifndef CHECK_FALSE_RET_LOG
        #define CHECK_FALSE_RET_LOG(expr, ret_val, tag, fmt, ...) CHECK_FALSE_RET(expr, ret_val)
    #endif // !CHECK_FALSE_RET_LOG

    #ifndef CHECK_FALSE_GOTO_LOG
        #define CHECK_FALSE_GOTO_LOG(expr, label, tag, fmt, ...) CHECK_FALSE_GOTO(expr, label)
    #endif // !CHECK_FALSE_GOTO_LOG

    #ifndef CHECK_FALSE_RET_VOID_LOG
        #define CHECK_FALSE_RET_VOID_LOG(expr, tag, fmt, ...) CHECK_FALSE_RET_VOID(expr)
    #endif// !CHECK_FALSE_RET_VOID_LOG
#endif // LOG_ENABLE
