
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "user_fifo.h"
#include "user_uart.h"

#define USE_DXBT37_AS_DEBUGM_OUTPUT 0 // 是否使用 dx_bt37 模块来输出 debug 信息。

#if USE_DXBT37_AS_DEBUGM_OUTPUT
#include "dx_bt37.h"
#endif

#define DEBUGM_LOGI(hdebug, tag, fmt, ...) do { \
    if (LIKELY((hdebug)->is_init == true && (hdebug)->dbg_printf)) { \
        ((hdebug)->dbg_printf)(hdebug, "[" tag "] " fmt "\r\n", ##__VA_ARGS__); \
    } \
} while (0)

#define DEBUGM_LOGW(hdebug, tag, fmt, ...) do { \
    if (LIKELY((hdebug)->is_init == true && (hdebug)->dbg_printf)) { \
        ((hdebug)->dbg_printf)(hdebug, "\r\n[" tag "] W: " fmt "\r\n\r\n", ##__VA_ARGS__); \
    } \
} while (0)

#define DEBUGM_LOGE(hdebug, tag, fmt, ...) do { \
    if (LIKELY((hdebug)->is_init == true && (hdebug)->dbg_printf)) { \
        ((hdebug)->dbg_printf)(hdebug, "\r\n[" tag "] E: " fmt "\r\n\r\n", ##__VA_ARGS__); \
    } \
} while (0)

#if defined(DEBUG) && DEBUG == 1
    #define DEBUGM_LOGD(hdebug, tag, fmt, ...) do { \
        if (LIKELY((hdebug)->is_init == true && (hdebug)->dbg_printf)) { \
            ((hdebug)->dbg_printf)(hdebug, "[" tag "] D: " fmt "\r\n", ##__VA_ARGS__); \
        } \
    } while (0)
#else
    #define DEBUGM_LOGD(...)
#endif

typedef struct debugm_handle_t
{
    user_fifo_t tx_fifo;
    UART_Send_t *uart_send;
    UART_Recv_t *uart_recv;
    void (*rx_dataproc_handler)(u8 *data, u16 len);
    void (*dbg_printf)(struct debugm_handle_t *h, const char *fmt, ...);
#if USE_DXBT37_AS_DEBUGM_OUTPUT
    dx_bt37_t *bt37;
#endif
    u8 *tx_buf;
    u8 *rx_buf;
    u8 *rx_proc_buf;
    u16 tx_buf_size;
    u16 rx_buf_size;
    u16 rx_proc_buf_size;
    u8 rx_len;
    bool is_init;
    bool rx_cplt;
} debugm_handle_t;

typedef struct debugm_config_t
{
    UART_Send_t *uart_send;     // 给一个经过初始化的串口发送句柄，若给 NULL 表示不用串口通信，
                                // 且只有 rx_dataproc_handler 需要配置。
    UART_Recv_t *uart_recv;     // 给一个经过初始化的串口接收句柄。给 NULL 表示不使用接收功能。
    u8 *tx_fifo_buf;            // 发送 FIFO 的缓冲区，必须提供
    u16 tx_fifo_size;           // 发送 FIFO 的大小，单位是字节。必须提供
    u8 *tx_buf;                 // 发送缓冲区，作为 debugm_printf 打印数据的缓冲区
    u16 tx_buf_size;            // 发送内存池每块的大小，单位是字节。实际上是单次发送数据的最大长度
    u8 *rx_buf;                 // 接收数据的缓冲区，作为串口接收的直接缓冲区。给 NULL 表示不使用接收功能
    u16 rx_buf_size;            // 接收数据缓冲区的大小。给 0 表示不使用接收功能
    u8 *rx_proc_buf;            // 接收数据的处理缓冲区，用户可以在这里处理接收到的数据
    u16 rx_proc_buf_size;       // 接收数据处理缓冲区的大小。给 0 表示不使用接收功能
    void (*rx_dataproc_handler)(u8 *data, u16 len); // 接收数据处理函数。可以阻塞处理数据。给 NULL 表示不处理接收数据
} debugm_config_t;

bool debugm_init(debugm_handle_t *h, debugm_config_t *cfg);
void debugm_printf(debugm_handle_t *h, const char *fmt, ...);
void debugm_send(debugm_handle_t *h);
void debugm_recv(debugm_handle_t *h);
void debugm_rx_cplt_handler(debugm_handle_t *h, u16 len);
void debugm_send_all(debugm_handle_t *h);

#if USE_DXBT37_AS_DEBUGM_OUTPUT
void debugm_init_dxbt37(debugm_handle_t *h, dx_bt37_t *bt37);
#endif

#ifdef __cplusplus
} // extern "C"
#endif
