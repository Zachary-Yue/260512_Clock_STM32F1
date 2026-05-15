
#include "debug_module.h"
#include "sys_err.h"
#include <stdarg.h>

#define DEBUGM_PRINTF_BUFSIZE 1000

/**
 * @brief 初始化调试模块。
 * 
 * @param h 调试模块句柄
 * @param config 调试模块配置
 * @return true 初始化成功
 * @return false 初始化失败，可能是配置参数错误或者内存不足等原因导致的。
 */
bool debugm_init(debugm_handle_t *h, debugm_config_t *config)
{
    CHECK_FALSE_RET(h && config, false);

    h->is_init = false;
    h->uart_send = NULL;
    h->uart_recv = NULL;
    h->tx_buf = NULL;
    h->tx_buf_size = 0;
    h->rx_buf = NULL;
    h->rx_buf_size = 0;
    h->rx_proc_buf = NULL;
    h->rx_proc_buf_size = 0;
    h->rx_dataproc_handler = NULL;
    h->rx_cplt = false;
    h->rx_len = 0;
#if USE_DXBT37_AS_DEBUGM_OUTPUT
    h->bt37 = NULL;
#endif
    h->dbg_printf = NULL;
    memset(&h->tx_fifo, 0, sizeof(user_fifo_t));

    bool use_uart = (config->uart_send && config->tx_buf && config->tx_buf_size);

    if (!use_uart) {
        // 不使用串口通信，只配置数据处理函数
        h->rx_dataproc_handler = config->rx_dataproc_handler;
        h->is_init = true;
        return true;
    }

    CHECK_FALSE_RET(config->tx_buf && config->tx_buf_size
        && config->tx_fifo_buf && config->tx_fifo_size, false);

    // 使用串口通信，配置相关参数
    bool en_recv = (config->uart_recv && config->rx_buf && config->rx_buf_size &&
        config->rx_proc_buf && config->rx_proc_buf_size &&
        config->rx_dataproc_handler);
    h->uart_send = config->uart_send;
    h->tx_buf = config->tx_buf;
    h->tx_buf_size = config->tx_buf_size;
    user_fifo_init(&h->tx_fifo, config->tx_fifo_buf, config->tx_fifo_size);
    h->dbg_printf = debugm_printf;

    if (en_recv) {
        h->uart_recv = config->uart_recv;
        h->rx_buf = config->rx_buf;
        h->rx_buf_size = config->rx_buf_size;
        h->rx_proc_buf = config->rx_proc_buf;
        h->rx_proc_buf_size = config->rx_proc_buf_size;
        h->rx_dataproc_handler = config->rx_dataproc_handler;
        uart_receive_start(h->uart_recv, h->rx_buf, true, h->rx_buf_size);
    }

    h->is_init = true;
    return true;
}

/**
 * @brief 格式化打印调试信息，类似于 printf 函数，但会将格式化后的字符串写入到调试模块的发送缓冲区中，等待发送。
 * 
 * @param h 调试模块句柄
 * @param fmt 格式字符串，类似于 printf 的格式字符串
 * @param ... 可变参数，按照 fmt 中的格式进行传递
 */
void debugm_printf(debugm_handle_t *h, const char *fmt, ...)
{
    u8 printf_buf[DEBUGM_PRINTF_BUFSIZE];
    CHECK_FALSE_RET_VOID(h && h->is_init == true);

    va_list args;
    va_start(args, fmt);
    int len = vsnprintf((char*)printf_buf, DEBUGM_PRINTF_BUFSIZE, fmt, args);
    if (len > 0) {
        // 这里不需要做检查，因为 user_fifo_write 内部会检查缓冲区是否有足够的空间，如果没有空间就丢弃多余的部分。
        // debugm_printf 的设计目标是尽可能地将调试信息发送出去，而不是保证每一条调试信息都能被发送。
        user_fifo_write(&h->tx_fifo, printf_buf, (u16)len);
    }
    va_end(args);
}

/**
 * @brief 发送调试信息，在 main 循环中调用。
 * 
 * @param h 调试模块句柄
 */
void debugm_send(debugm_handle_t *h)
{
    CHECK_FALSE_RET_VOID(h);
    CHECK_FALSE_RET_VOID(h->is_init == true && h->uart_send && &h->tx_fifo && h->tx_buf);
    CHECK_FALSE_RET_VOID(!uart_is_sending(h->uart_send));  // 确保串口处于空闲状态

    u16 len = user_fifo_get_used_length(&h->tx_fifo);
    CHECK_FALSE_RET_VOID(len > 0);   // 没有数据要发送

    if (len > h->tx_buf_size) len = h->tx_buf_size;   // 限制每次发送的长度
    user_fifo_read(&h->tx_fifo, h->tx_buf, len, USER_FIFO_READ_AND_CLEAN);  // 从 fifo 中读取数据并清除
    uart_send_start(h->uart_send, h->tx_buf, len);  // 发送数据
}

/**
 * @brief 将串口收到的消息拷贝到处理缓冲区，并设置接收完成标志，在串口接收完成回调函数中调用。
 * 
 * @param len 接收到的数据长度
 */
void debugm_rx_cplt_handler(debugm_handle_t *h, u16 len)
{
    CHECK_FALSE_RET_VOID(h);
    CHECK_FALSE_RET_VOID(h->is_init == true && h->rx_proc_buf);
    memset(h->rx_proc_buf, 0, h->rx_proc_buf_size);  // 清空处理缓冲区
    h->rx_len = min(len, min(h->rx_buf_size, h->rx_proc_buf_size));  // 确保不会越界
    memcpy(h->rx_proc_buf, h->rx_buf, h->rx_len);  // 将接收到的数据复制到处理缓冲区
    h->rx_cplt = true;
    // 接收由 uart_receive_start 以 keepListen 方式持续进行
}

/**
 * @brief 接收并处理调试信息，在 main 循环中调用。
 * 
 */
void debugm_recv(debugm_handle_t *h)
{
    CHECK_FALSE_RET_VOID(h);
    CHECK_FALSE_RET_VOID(h->is_init && h->rx_dataproc_handler);
    CHECK_FALSE_RET_VOID(h->rx_cplt);
    h->rx_cplt = false;
    h->rx_dataproc_handler(h->rx_proc_buf, h->rx_len);  // 调用用户定义的数据处理函数
}

/**
 * @brief 发送所有待发送的调试信息。
 * 
 * @param h 调试模块句柄
 */
void debugm_send_all(debugm_handle_t *h)
{
    CHECK_FALSE_RET_VOID(h);
    CHECK_FALSE_RET_VOID(h->is_init == true && h->uart_send && &h->tx_fifo && h->tx_buf);
    CHECK_FALSE_RET_VOID(!uart_is_sending(h->uart_send));  // 确保串口处于空闲状态

    while (user_fifo_get_used_length(&h->tx_fifo) > 0) {
        u16 len = user_fifo_get_used_length(&h->tx_fifo);
        if (len > h->tx_buf_size) len = h->tx_buf_size;   // 限制每次发送的长度
        user_fifo_read(&h->tx_fifo, h->tx_buf, len, USER_FIFO_READ_AND_CLEAN);  // 从 fifo 中读取数据并清除
        uart_send_start(h->uart_send, h->tx_buf, len);  // 发送数据
        uart_send_wait(h->uart_send, 5000);  // 等待发送完成，避免过快地发送下一批数据
    }
}

#if USE_DXBT37_AS_DEBUGM_OUTPUT
    /**
     * @brief 将 dx_bt37 模块集成到 debugm 模块中，使得 debugm 可以通过 dx_bt37 来输出调试信息。
     * 
     * @param h 调试模块句柄
     * @param bt37 已经初始化的 dx_bt37 模块句柄
     */
    void debugm_init_dxbt37(debugm_handle_t *h, dx_bt37_t *bt37)
    {
        CHECK_FALSE_RET_VOID(h && h->is_init == true && bt37 && bt37->is_init);
        h->bt37 = bt37;
        h->dbg_printf = NULL; // set dbg_printf to bt37 printf function
        // set bt37 recv handler function to h->rx_dataproc_handler
    }
#endif
