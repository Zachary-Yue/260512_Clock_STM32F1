
#include "debug.h"
#include "user_fifo.h"
#include <stdlib.h>
#include "sys_err.h"

#include "user_phr.h"
#include "system.h"
#include "bt.h"

#define TAG "DEBUG"

debugm_handle_t hdebug;
debug_static_handle_t hdebug_static;

#define DEBUG_TXFIFO_SIZE 1024      // 增加调试 FIFO 的大小，可以减少丢失调试信息的可能性
#define DEBUG_TXBUF_SIZE 64         // 减小临时发送缓冲区大小的影响只是多发几次
#define DEBUG_RXBUF_SIZE 64         // 定义接收缓冲区大小
#define DEBUG_RXPROCBUF_SIZE 64     // 定义接收处理缓冲区大小

static u8 debug_txfifo_buf[DEBUG_TXFIFO_SIZE];
static u8 debug_txbuf[DEBUG_TXBUF_SIZE];
static u8 debug_rxbuf[DEBUG_RXBUF_SIZE];
static u8 debug_rx_proc_buf[DEBUG_RXPROCBUF_SIZE];

#define DEBUG_S_FIFO_SIZE 4096
static u8 debug_s_fifo_buf[DEBUG_S_FIFO_SIZE];

void debug_rx_dataproc_handler(u8 *data, u16 len);

/**
 * @brief 初始化调试功能。
 * 
 */
void debug_init(void)
{
    debug_static_config_t debug_static_cfg = {
        .log_fifo_buf = debug_s_fifo_buf,
        .log_fifo_size = DEBUG_S_FIFO_SIZE,
    };
    debug_static_init(&hdebug_static, &debug_static_cfg);

    debugm_config_t cfg = {
        .uart_send = &huart1_send,
        .uart_recv = &huart1_recv,
        .tx_fifo_buf = debug_txfifo_buf,
        .tx_fifo_size = DEBUG_TXFIFO_SIZE,
        .tx_buf = debug_txbuf,
        .tx_buf_size = DEBUG_TXBUF_SIZE,
        .rx_buf = debug_rxbuf,
        .rx_buf_size = DEBUG_RXBUF_SIZE,
        .rx_proc_buf = debug_rx_proc_buf,
        .rx_proc_buf_size = DEBUG_RXPROCBUF_SIZE,
        .rx_dataproc_handler = debug_rx_dataproc_handler,
    };
    ERR_CHECK(debugm_init(&hdebug, &cfg));
}

void debug_rx_dataproc_handler(u8 *data, u16 len)
{
    CHECK_FALSE_RET_VOID(data && len > 0);
    if (begins_with_str(data, "hello")) {
        LOGI(TAG, "Hello to you too!\r\nThis is your lovely clock!");
    }
    else if (begins_with_str(data, "sys reset")) {
        LOGW(TAG, "System is resetting...");
        debug_send_all();
        bt_disconnect();
        sys_reset();
    }
    else {
        LOGW(TAG, "Received unknown command: %.*s", len, data);
    }
}
