#ifndef __USER_UART_H
#define __USER_UART_H

#include "main.h"

/**
 * @brief 串口行为管理结构体
 * 
 */
typedef struct UART_Send_t
{
	USART_TypeDef 	*Instance;
	DMA_TypeDef		*DMA_Instance;
	u32 			 DMA_Channel;

	bool  			 Busy;		// 忙标志
} UART_Send_t;

/**
 * @brief 串口接收行为控制结构体。
 * 
 */
typedef struct UART_Recv_t
{
	USART_TypeDef 	*Instance;
	DMA_TypeDef		*DMA_Instance;
	u32 			 DMA_Channel;

	u8		*Buf;				// 接收缓冲区
	u16 	ExpectedLen;		// 要接收的字节数，小于缓冲区大小
	u16 	ReceivedLen;		// 实际接收到的字节数
	bool	IsStart;			// 接收是否正在进行
	bool	KeepListen;			// 是否持续监听
} UART_Recv_t;

#define uart_is_sending(send) ((send)->Busy)


typedef void (*UART_SendCplt_CB_t)(UART_Send_t *send);
typedef void (*UART_RecvCplt_CB_t)(UART_Recv_t *recv);

void uart_send_init(
	UART_Send_t *send,
	USART_TypeDef *instance,
	DMA_TypeDef *dma_instance,
	u32 dma_channel );
bool uart_send_start(UART_Send_t *send, u8 const *buf, u16 len);
void uart_send_dmaCplt_handler(UART_Send_t *send);
void uart_send_tc_handler(UART_Send_t *send);
void uart_send_setcb(UART_SendCplt_CB_t callback);
bool uart_send_wait(UART_Send_t *send, u32 timeout_ms);

void uart_recv_init(
	UART_Recv_t *recv,
	USART_TypeDef *instance,
	DMA_TypeDef *dma_instance,
	u32 dma_channel );
bool uart_receive_start(UART_Recv_t *recv, u8 *buf, bool keepListen, u16 expectedLen);
void uart_recv_dmaCplt_handler(UART_Recv_t *recv);
void uart_recv_idle_handler(UART_Recv_t *recv);
void uart_recv_setcb(UART_RecvCplt_CB_t callback);
bool uart_recv_wait(UART_Recv_t *recv, u32 timeout_ms);

void uart_change_baud(USART_TypeDef *instance, u32 baud);
void uart_abort(UART_Send_t *send, UART_Recv_t *recv);

#endif // !__USER_UART_H
