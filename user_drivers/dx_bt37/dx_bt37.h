
#pragma once

#include "main.h"
#include "user_uart.h"
#include "user_fifo.h"

typedef enum {
    DX_BT37_BAUD_2400 = 0,
    DX_BT37_BAUD_4800,
    DX_BT37_BAUD_9600,
    DX_BT37_BAUD_19200,
    DX_BT37_BAUD_38400,
    DX_BT37_BAUD_57600,
    DX_BT37_BAUD_115200,
    DX_BT37_BAUD_230400,
    DX_BT37_BAUD_460800,
    DX_BT37_BAUD_921600,
    DX_BT37_BAUD_1000000,
    DX_BT37_BAUD_MAX
} dx_bt37_baud_t;

typedef struct dx_bt37_t {
    user_fifo_t tx_fifo;            // 发送数据的 FIFO
    UART_Send_t *uart_send;         // 给一个经过初始化的串口发送句柄
    UART_Recv_t *uart_recv;         // 给一个经过初始化的串口接收句柄
    void (*dx_bt37_rx_data_proc)(struct dx_bt37_t *bt37, u8 *data, u16 len); // 数据处理回调函数，给 NULL 表示不处理接收到的数据
    dx_bt37_baud_t baud;            // 波特率
    u8 *tx_buf;                     // 发送数据的缓冲区
    u8 *rx_buf;                     // 接收数据的缓冲区
    u8 *rx_proc_buf;                // 数据处理缓冲区
    u16 tx_buf_size;                // 发送数据的缓冲区大小
    u16 rx_buf_size;                // 接收数据的缓冲区大小
    u16 rx_proc_buf_size;           // 数据处理的缓冲区大小
    u16 rx_received_len;            // 本次接收到的数据长度
    bool is_init;                   // DX_BT37 是否已经初始化
    bool rx_enabled;                // DX_BT37 是否使能接收
    bool rx_wait_for_proc;          // 数据接收完成、等待处理
} dx_bt37_t;

typedef struct {
    struct {
        // 给一个经过初始化的串口发送句柄（不可给 NULL，初始化一定会用到发送功能）
        UART_Send_t *uart_send;
        u8 *tx_buf;                     // 发送数据的缓冲区
        u8 *tx_fifo_buf;                // 发送 FIFO 的缓冲区
        u16 tx_buf_size;                // 发送数据的缓冲区大小
        u16 tx_fifo_buf_size;           // 发送 FIFO 的缓冲区大小
    } send_attr;
    struct {
         // 数据处理回调函数，给 NULL 表示不处理接收到的数据
        void (*dx_bt37_rx_data_proc)(dx_bt37_t *bt37, u8 *data, u16 len);
        // 给一个经过初始化的串口接收句柄（不可给 NULL，初始化一定会用到接收功能）
        UART_Recv_t *uart_recv;
        u8 *rx_buf;                     // 接收数据的缓冲区，给 NULL 表示不接收数据
        u8 *rx_proc_buf;                // 数据处理缓冲区，给 NULL 表示不接收数据
        u16 rx_buf_size;                // 接收数据的缓冲区大小，给 0 不接收数据
        u16 rx_proc_buf_size;           // 数据处理的缓冲区大小，给 0 不接收数据
    } recv_attr;
} dx_bt37_config_t;


bool dx_bt37_init(dx_bt37_t *bt37, dx_bt37_config_t *config);
bool dx_bt37_change_baud(dx_bt37_t *bt37, dx_bt37_baud_t baud);
void dx_bt37_printf(dx_bt37_t *bt37, const char *format, ...);
void dx_bt37_send_raw(dx_bt37_t *bt37, const u8 *data, u16 len);
void dx_bt37_send(dx_bt37_t *bt37);
void dx_bt37_send_all(dx_bt37_t *bt37);
void dx_bt37_rx_cplt_handler(dx_bt37_t *bt37, u16 len);
void dx_bt37_recv(dx_bt37_t *bt37);
void dx_bt37_disconnect(dx_bt37_t *bt37);
