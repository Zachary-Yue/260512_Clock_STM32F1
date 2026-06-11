
#include "bt.h"
#include "debug.h"
#include "user_phr.h"

#include "msg_parse.h"

#define TAG "BT"

#define BT37_TX_FIFO_SIZE 500
#define BT37_TX_BUF_SIZE 64
#define BT37_RX_BUF_SIZE 64
#define BT37_RX_PROC_BUF_SIZE 64

dx_bt37_t bt37;
static u8 bt37_txfifo_buf[BT37_TX_FIFO_SIZE];
static u8 bt37_tx_buf[BT37_TX_BUF_SIZE];
static u8 bt37_rx_buf[BT37_RX_BUF_SIZE];
static u8 bt37_rx_proc_buf[BT37_RX_PROC_BUF_SIZE];

static void dx_bt37_rx_data_proc(dx_bt37_t *pbt37, u8 *data, u16 len);

void bt_init(void)
{
    LOGI(TAG, "Initializing BLE module...");
    dx_bt37_config_t cfg = {
        .send_attr.uart_send = &huart3_send,
        .recv_attr.uart_recv = &huart3_recv,
        .send_attr.tx_fifo_buf = bt37_txfifo_buf,
        .send_attr.tx_fifo_buf_size = BT37_TX_FIFO_SIZE,
        .send_attr.tx_buf = bt37_tx_buf,
        .send_attr.tx_buf_size = BT37_TX_BUF_SIZE,
        .recv_attr.rx_buf = bt37_rx_buf,
        .recv_attr.rx_buf_size = BT37_RX_BUF_SIZE,
        .recv_attr.rx_proc_buf = bt37_rx_proc_buf,
        .recv_attr.rx_proc_buf_size = BT37_RX_PROC_BUF_SIZE,
        .recv_attr.dx_bt37_rx_data_proc = dx_bt37_rx_data_proc
    };
    dx_bt37_init(&bt37, &cfg);
    LOGI(TAG, "BLE module initialization finished.");
}

void bt_change_baud(u32 baudrate)
{
    if (!bt37.is_init) {
        LOGE(TAG, "Module hasn't been initialized.");
        return;
    }
    dx_bt37_baud_t baud;
    switch (baudrate)
    {
        case 2400: baud = DX_BT37_BAUD_2400; break;
        case 4800: baud = DX_BT37_BAUD_4800; break;
        case 9600: baud = DX_BT37_BAUD_9600; break;
        case 19200: baud = DX_BT37_BAUD_19200; break;
        case 38400: baud = DX_BT37_BAUD_38400; break;
        case 57600: baud = DX_BT37_BAUD_57600; break;
        case 115200: baud = DX_BT37_BAUD_115200; break;
        case 230400: baud = DX_BT37_BAUD_230400; break;
        case 460800: baud = DX_BT37_BAUD_460800; break;
        case 921600: baud = DX_BT37_BAUD_921600; break;
        case 1000000: baud = DX_BT37_BAUD_1000000; break;
        default:
            LOGW(TAG, "Unsupported baud rate: %lu", baudrate);
            return;
    }
    dx_bt37_change_baud(&bt37, baud);
}

static void dx_bt37_rx_data_proc(dx_bt37_t *pbt37, u8 *data, u16 len)
{
    if (pbt37 == &bt37) {
        app_message_parse((char*)data, len);
    }
}
