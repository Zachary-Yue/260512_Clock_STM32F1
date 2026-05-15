
#include "user_phr.h"
#include "sys_err.h"
#include "bt.h"

I2C_Send_t hi2c1;
UART_Send_t huart1_send;
UART_Send_t huart3_send;
UART_Recv_t huart1_recv;
UART_Recv_t huart3_recv;

static void user_uart_recv_cplt(UART_Recv_t *recv);

void user_phr_init(void)
{
    uart_recv_setcb(user_uart_recv_cplt);
    uart_send_init(&huart1_send, USART1, DMA1, LL_DMA_CHANNEL_4);
    uart_send_init(&huart3_send, USART3, DMA1, LL_DMA_CHANNEL_2);
    uart_recv_init(&huart1_recv, USART1, DMA1, LL_DMA_CHANNEL_5);
    uart_recv_init(&huart3_recv, USART3, DMA1, LL_DMA_CHANNEL_3);
    i2c_send_init(&hi2c1, I2C1, DMA1, LL_DMA_CHANNEL_6);
}

static void user_uart_recv_cplt(UART_Recv_t *recv)
{
    if (recv == &huart1_recv) {
        debug_rx_it_handler(recv->ReceivedLen);
    } else if (recv == &huart3_recv) {
        bt_rx_cplt_handler(recv->ReceivedLen);
    }
}
