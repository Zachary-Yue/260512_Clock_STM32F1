/**
 * @file debug_static.c
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
#include "debug_static.h"
#include "sys_err.h"
#include <stdarg.h>

#define TMP_LOG_BUF_SIZE 256

bool debug_static_init(debug_static_handle_t *h, debug_static_config_t *config)
{
    CHECK_FALSE_RET(h && config, false);
    CHECK_FALSE_RET(config->log_fifo_buf && config->log_fifo_size, false);
    user_fifo_init(&h->log_fifo, config->log_fifo_buf, config->log_fifo_size);
    h->is_init = true;
    return true;
}

/**
 * @brief The printf function for static log
 * 
 * @param h the handle of the static log module
 * @param fmt the format string, which supports the same format as printf
 * @param ... the arguments for the format string
 */
void debug_static_printf(debug_static_handle_t *h, const char *fmt, ...)
{
    CHECK_FALSE_RET_VOID(h && fmt);
    CHECK_FALSE_RET_VOID(debug_static_is_init(h));
    char tmp_buf[TMP_LOG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(tmp_buf, TMP_LOG_BUF_SIZE, fmt, args);
    va_end(args);
    if (len < 0) return;
    if (len >= TMP_LOG_BUF_SIZE) len = TMP_LOG_BUF_SIZE - 1;
    user_fifo_write(&h->log_fifo, (u8 *)tmp_buf, len);
}

/**
 * @brief Read the static logs
 * 
 * @param h the handle of the static log module
 * @param buf the buffer to store the logs
 * @param buf_size the size of the buffer
 */
void debug_static_read_logs(debug_static_handle_t *h, u8 *buf, u16 buf_size)
{
    CHECK_FALSE_RET_VOID(h && buf);
    CHECK_FALSE_RET_VOID(debug_static_is_init(h));
    user_fifo_read(&h->log_fifo, buf, buf_size, USER_FIFO_READ_ONLY);
}

/**
 * @brief Dump the static logs
 * 
 * @param h the handle of the static log module
 * @param len the number of bytes to dump
 */
void debug_static_dump_logs(debug_static_handle_t *h, u16 len)
{
    CHECK_FALSE_RET_VOID(h);
    CHECK_FALSE_RET_VOID(debug_static_is_init(h));
    user_fifo_clear(&h->log_fifo, len);
}

/**
 * @brief Clear the static logs
 * 
 * @param h the handle of the static log module
 */
void debug_static_clear_logs(debug_static_handle_t *h)
{
    CHECK_FALSE_RET_VOID(h);
    CHECK_FALSE_RET_VOID(debug_static_is_init(h));
    user_fifo_clear(&h->log_fifo, -1);
}
