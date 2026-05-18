/**
 * @file debug_static.h
 * @author Zach (zachary-yue@qq.com)
 * 
 * @brief Static log implementation. This kind of log is recorded in ram
 *        and can be dumped when needed, which is useful for debugging issues
 *        that aren't be able to be observed in real time, such as system crashes, resets, etc.
 * 
 *        The future plan is to record the static log in eeprom or flash chips.
 * 
 * @version 0.1
 * @date 2026-05-18
 * 
 */
#pragma once

#include "main.h"
#include "user_fifo.h"
#include "app_clock.h"

typedef struct {
    user_fifo_t log_fifo;  // 用于存储日志的 FIFO
    bool is_init;          // 是否已经初始化
} debug_static_handle_t;

typedef struct {
    u8 *log_fifo_buf;
    u16 log_fifo_size;
} debug_static_config_t;

#define debug_static_is_init(h) ((h) && (h)->is_init == true)

// format of the log is: [timestamp] [TAG] log content
// e.g. [2026-05-18 12:00:00] [INFO] System started successfully.

#define DEBUG_S_LOGI(h, tag, fmt, ...) do { \
    debug_static_printf(h, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] " fmt "\r\n", \
        Calendar.year + 2000, Calendar.month, Calendar.day, \
        Calendar.hour, Calendar.min, Calendar.sec, tag, ##__VA_ARGS__); \
} while (0)

#define DEBUG_S_LOGW(h, tag, fmt, ...) do { \
    debug_static_printf(h, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] [W] " fmt "\r\n", \
        Calendar.year + 2000, Calendar.month, Calendar.day, \
        Calendar.hour, Calendar.min, Calendar.sec, tag, ##__VA_ARGS__); \
} while (0)

#define DEBUG_S_LOGE(h, tag, fmt, ...) do { \
    debug_static_printf(h, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] [E] " fmt "\r\n", \
        Calendar.year + 2000, Calendar.month, Calendar.day, \
        Calendar.hour, Calendar.min, Calendar.sec, tag, ##__VA_ARGS__); \
} while (0)

bool debug_static_init(debug_static_handle_t *h, debug_static_config_t *config);
void debug_static_printf(debug_static_handle_t *h, const char *fmt, ...);
void debug_static_read_logs(debug_static_handle_t *h, u8 *buf, u16 buf_size);
void debug_static_dump_logs(debug_static_handle_t *h, u16 len);
void debug_static_clear_logs(debug_static_handle_t *h);
