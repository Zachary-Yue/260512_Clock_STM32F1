
#include "user_uart.h"
#include "user_dma.h"
#include "user_tick.h"
#include "sys_err.h"

#define TAG "USER_UART"

/*------------------------------------------- Transition Part -----------------------------------------*/

/**
 * @brief Things to do when transmission is completed.
 * @note  Determined by user.
 * 
 */
static UART_SendCplt_CB_t sendCplt_cb = NULL;

void uart_send_setcb(UART_SendCplt_CB_t callback)
{
	sendCplt_cb = callback;
}


/**
 * @brief Initialize the uart send structure.
 * 
 * @param send the uart send structure.
 * @param instance usart instance
 * @param dma_instance dma instance
 * @param dma_channel dma channel
 */
void uart_send_init(
	UART_Send_t *send,
	USART_TypeDef *instance,
	DMA_TypeDef *dma_instance,
	u32 dma_channel )
{
	send->Instance = instance;
	send->DMA_Instance = dma_instance;
	send->DMA_Channel = dma_channel;
	send->Busy = false;
}


/**
 * @brief 启动发送。用中断的方式发送。
 * @note  txBuf 需要是全局变量。
 * 
 * @param send   Handle
 * @param txBuf  Buffer with data to be send
 * @param txSize 要发送的数据长度，不可大于数组长度。
 */
bool uart_send_start(UART_Send_t *send, u8 const *buf, u16 len)
{
	CHECK_FALSE_RET(send && buf && len > 0, false);
	CHECK_FALSE_RET(send->Busy == false, false);

	send->Busy = true;
	
	LL_USART_DisableIT_TC(send->Instance);
	LL_USART_ClearFlag_TC(send->Instance);

	LL_DMA_DisableChannel(send->DMA_Instance, send->DMA_Channel);
	LL_DMA_SetMemoryAddress(send->DMA_Instance, send->DMA_Channel, (u32)(buf));
	LL_DMA_SetPeriphAddress(send->DMA_Instance, send->DMA_Channel, LL_USART_DMA_GetRegAddr(send->Instance));
	LL_DMA_SetDataLength(send->DMA_Instance, send->DMA_Channel, len);
	LL_DMA_DisableIT_HT(send->DMA_Instance, send->DMA_Channel);
	LL_DMA_DisableIT_TE(send->DMA_Instance, send->DMA_Channel);
	user_dma_clear_it_flag_tc(send->DMA_Instance, send->DMA_Channel);
	LL_DMA_EnableIT_TC(send->DMA_Instance, send->DMA_Channel);
	LL_DMA_EnableChannel(send->DMA_Instance, send->DMA_Channel);

	// Enable DMA request.
	LL_USART_EnableDMAReq_TX(send->Instance);
	LL_USART_EnableDirectionTx(send->Instance);

	// I was wondering enable the interrupt at the beginning of the program,
	// but then the damn interrupt occured at the beginning of the program,
	// and never exited.
	// Therefore, you ought to enable the interrupt at the beginning of trasnmission...
	// It seems that TC bit is 1 when the usart is idle.
	// And if you turn on TCIE, it'll call the interrupt all the time.
	// LL_USART_EnableIT_TC(send->Instance);

	// TC interrupt is enabled in dma cplt handler.

	return true;
}


/**
 * @brief Handle something when dma transmission is over.
 * 
 * @param send the handle
 */
void uart_send_dmaCplt_handler(UART_Send_t *send)
{
	if (user_dma_get_it_flag_tc(send->DMA_Instance, send->DMA_Channel))
	{
		user_dma_clear_it_flag_tc(send->DMA_Instance, send->DMA_Channel);
		if (LL_DMA_IsEnabledChannel(send->DMA_Instance, send->DMA_Channel))
		{
			// If the dma transmission completion occurs.
			// Disable the channel, calc received length, and do not disable DMA TC interrupt.
			LL_DMA_DisableChannel(send->DMA_Instance, send->DMA_Channel);
			// LL_DMA_DisableIT_TC(send->DMA_Instance, send->DMA_Channel);
			LL_USART_DisableDMAReq_TX(send->Instance);
			// LL_USART_DisableDirectionTx(send->Instance);		// You must not disable now! Which will damage the last flame!
			// send->Busy = false;								// Not now either.

			LL_USART_EnableIT_TC(send->Instance);
			// Call the data process handler.
			// if (uart_sendcplt_callback != NULL) uart_sendcplt_callback(send);	// Not exactly now either.
		}
	}
}


/**
 * @brief Handle something when whole transmission process is completely over.
 * 
 * @param send the handle
 */
void uart_send_tc_handler(UART_Send_t *send)
{
	if (LL_USART_IsActiveFlag_TC(send->Instance))
	{
		// Clear TC flag
		// Whether it's an expected transmission or not, TC flag must be cleared.
		// CLEAR_BIT(send->Instance->SR, (0x00000001 << 6));
		LL_USART_ClearFlag_TC(send->Instance);
		if (LL_USART_IsEnabledIT_TC(send->Instance)) {
			// Transmission over.
			// Otherwise it'll stuck in the interrupt.
			// Disable the uart's transmission part.
			LL_USART_DisableDirectionTx(send->Instance);
			// Disable the interrupt
			LL_USART_DisableIT_TC(send->Instance);

			// Another transmission task is allowed to assign.
			if (send->Busy == true)
			{
				send->Busy = false;
				// Call the data process handler.
				if (sendCplt_cb != NULL) sendCplt_cb(send);
			}
		}
	}
}


/**
 * @brief Wait for the transmission to be completed, or timeout.
 * 
 * @param send the handle
 * @param timeout_ms timeout in milliseconds
 * @return bool true if the transmission is completed before timeout, false if timeout occurs.
 */
bool uart_send_wait(UART_Send_t *send, u32 timeout_ms)
{
	CHECK_FALSE_RET(send, false);
	u32 start = user_get_tick();
	while (send->Busy) {
		if ((user_get_tick() - start) > timeout_ms) {
			return false;
		}
	}
	return true;
}


/*------------------------------------------- Reception Part -----------------------------------------*/

/**
 * @brief Things to do when reception is completed.
 * @note  Determined by user.
 * 
 */
static UART_RecvCplt_CB_t recvCplt_cb = NULL;

void uart_recv_setcb(UART_RecvCplt_CB_t callback)
{
	recvCplt_cb = callback;
}


/**
 * @brief Initialize the uart recv structure.
 * 
 * @param recv the uart recv structure.
 * @param instance usart instance
 * @param dma_instance dma instance
 * @param dma_channel dma channel
 */
void uart_recv_init(
	UART_Recv_t *recv,
	USART_TypeDef *instance,
	DMA_TypeDef *dma_instance,
	u32 dma_channel )
{
	recv->Instance = instance;
	recv->DMA_Instance = dma_instance;
	recv->DMA_Channel = dma_channel;
	recv->IsStart = false;
	recv->KeepListen = false;
	recv->ReceivedLen = 0;
	recv->ExpectedLen = 0;
}


/**
 * @brief Start usart reception by DMA mode.
 * 
 * @param recv 			the structure that controls periphs' behavior
 * @param buf 			the reception buffer (must be a global variable)
 * @param keepListen 	if keeps listen to usart rx, keep receiveing an amount of data specified by ExpectedLen
 * @param expectedLen 	the expected amount of data to be received
 * @return bool 		If the reception process is successfully started.
 */
bool uart_receive_start(UART_Recv_t *recv, u8 *buf, bool keepListen, u16 expectedLen)
{
	if (expectedLen == 0 || buf == NULL || recv->IsStart) return false;

	recv->IsStart = true;
	recv->Buf = buf;
	recv->KeepListen = keepListen;
	recv->ExpectedLen = expectedLen;
	
	// Configure DMA channel, count-down register, and start the transport.
	LL_DMA_DisableChannel(recv->DMA_Instance, recv->DMA_Channel);
	LL_DMA_SetMemoryAddress(recv->DMA_Instance, recv->DMA_Channel, (u32)(recv->Buf));
	LL_DMA_SetPeriphAddress(recv->DMA_Instance, recv->DMA_Channel, LL_USART_DMA_GetRegAddr(recv->Instance));
	LL_DMA_SetDataLength(recv->DMA_Instance, recv->DMA_Channel, recv->ExpectedLen);
	LL_DMA_DisableIT_HT(recv->DMA_Instance, recv->DMA_Channel);
	LL_DMA_DisableIT_TE(recv->DMA_Instance, recv->DMA_Channel);
	LL_DMA_EnableIT_TC(recv->DMA_Instance, recv->DMA_Channel);
	LL_DMA_EnableChannel(recv->DMA_Instance, recv->DMA_Channel);
	// Enable DMA request.
	LL_USART_EnableIT_IDLE(recv->Instance);
	LL_USART_EnableDMAReq_RX(recv->Instance);
	LL_USART_EnableDirectionRx(recv->Instance);
	return true;
}


/**
 * @brief Things to do when an idle event occurs.
 * 
 * @param recv The struct that responsible for reception.
 */
void uart_recv_idle_handler(UART_Recv_t *recv)
{
	if (LL_USART_IsActiveFlag_IDLE(recv->Instance))
	{
		LL_USART_ClearFlag_IDLE(recv->Instance);

		if (LL_USART_IsEnabledIT_IDLE(recv->Instance))
		{
			// Stops DMA transportation and get the amount transported.
			LL_DMA_DisableChannel(recv->DMA_Instance, recv->DMA_Channel);
			recv->ReceivedLen = (recv->ExpectedLen - (u16)LL_DMA_GetDataLength(recv->DMA_Instance, recv->DMA_Channel));
			if (recv->KeepListen)
			{
				/**
				 * No need to rewrite the same address to the register.
				 * If the channel is disabled and then enabled again, it'll transmit data
				 * from and to original sources and destinations.
				 */
				// LL_DMA_SetMemoryAddress(recv->DMA_Instance, recv->DMA_Channel, (u32)(recv->Buf));
				LL_DMA_SetDataLength(recv->DMA_Instance, recv->DMA_Channel, recv->ExpectedLen);
				LL_DMA_EnableChannel(recv->DMA_Instance, recv->DMA_Channel);
			}
			else
			{
				LL_DMA_DisableIT_TC(recv->DMA_Instance, recv->DMA_Channel);
				LL_USART_DisableIT_IDLE(recv->Instance);
				LL_USART_DisableDMAReq_RX(recv->Instance);
				LL_USART_DisableDirectionRx(recv->Instance);
				recv->IsStart = false;
			}

			// Call the data process handler.
			if (recvCplt_cb != NULL) recvCplt_cb(recv);
		}
	}
}


/**
 * @brief Things to do when DMA transmission completion interrupt occurs.
 * 
 * @param recv The struct that responsible for reception.
 */
void uart_recv_dmaCplt_handler(UART_Recv_t *recv)
{
	if (user_dma_get_it_flag_tc(recv->DMA_Instance, recv->DMA_Channel))
	{
		user_dma_clear_it_flag_tc(recv->DMA_Instance, recv->DMA_Channel);
		if (LL_DMA_IsEnabledChannel(recv->DMA_Instance, recv->DMA_Channel) &&
			LL_DMA_IsEnabledIT_TC(recv->DMA_Instance, recv->DMA_Channel))
		{
			// If the dma transmission completion occurs.
			// Disable the channel, calc received length, and disable DMA TC interrupt.
			LL_DMA_DisableChannel(recv->DMA_Instance, recv->DMA_Channel);
			recv->ReceivedLen = recv->ExpectedLen;
			if (recv->KeepListen)
			{
				/**
				 * No need to rewrite the same address to the register.
				 * If the channel is disabled and then enabled again, it'll transmit data
				 * from and to original sources and destinations.
				 */
				// LL_DMA_SetMemoryAddress(recv->DMA_Instance, recv->DMA_Channel, (u32)(recv->Buf));
				LL_DMA_SetDataLength(recv->DMA_Instance, recv->DMA_Channel, recv->ExpectedLen);
				LL_DMA_EnableChannel(recv->DMA_Instance, recv->DMA_Channel);
			}
			else
			{
				LL_DMA_DisableIT_TC(recv->DMA_Instance, recv->DMA_Channel);
				LL_USART_DisableIT_IDLE(recv->Instance);
				LL_USART_DisableDMAReq_RX(recv->Instance);
				LL_USART_DisableDirectionRx(recv->Instance);
				recv->IsStart = false;
			}

			// Call the data process handler.
			if (recvCplt_cb != NULL) recvCplt_cb(recv);
		}
	}
	if (user_dma_get_it_flag_te(recv->DMA_Instance, recv->DMA_Channel))
	{
		user_dma_clear_it_flag_te(recv->DMA_Instance, recv->DMA_Channel);
		SLOGE(TAG, "DMA channel %u failed.", recv->DMA_Channel);
	}
}


/**
 * @brief Wait for the reception to be completed, or timeout.
 * 
 * @param recv the handle
 * @param timeout_ms timeout in milliseconds
 * @return bool true if the reception is completed before timeout, false if timeout occurs.
 */
bool uart_recv_wait(UART_Recv_t *recv, u32 timeout_ms)
{
	u32 start = user_get_tick();
	while (recv->IsStart) {
		if ((user_get_tick() - start) > timeout_ms) {
			return false;
		}
	}
	return true;
}

/*------------------------------------------------------------------------------------*/

/**
 * @brief Change the baud rate of the usart.
 * 
 * @param instance usart instance
 * @param baud new baud rate
 */
void uart_change_baud(USART_TypeDef *instance, u32 baud)
{
	CHECK_FALSE_RET_VOID(instance);
	LL_USART_Disable(instance);
	LL_RCC_ClocksTypeDef rcc_clocks;
	u32 pclk = 0;

	LL_RCC_GetSystemClocksFreq(&rcc_clocks);
	pclk = (instance == USART1) ? rcc_clocks.PCLK2_Frequency : rcc_clocks.PCLK1_Frequency;
	LL_USART_SetBaudRate(instance, pclk, baud);
	LL_USART_Enable(instance);
}


/**
 * @brief Abort the UART transmission and reception.
 * @note  send and recv can be NULL, but at least one of them should not be NULL.
 * @note  If both of them are not NULL, they should have the same usart instance.
 * 
 * @param send send handle
 * @param recv receive handle
 */
void uart_abort(UART_Send_t *send, UART_Recv_t *recv)
{
    USART_TypeDef *instance = NULL;
    if (send) instance = send->Instance;
    if (instance == NULL && recv) instance = recv->Instance;
    if (instance == NULL) return;

    if (send) {
		LL_USART_DisableDirectionTx(send->Instance);
		LL_USART_DisableDMAReq_TX(instance);
        LL_DMA_DisableChannel(send->DMA_Instance, send->DMA_Channel);
		LL_USART_DisableIT_TC(send->Instance);
		LL_USART_ClearFlag_TC(instance);
		send->Busy = false;
    }
    if (recv) {
		LL_USART_DisableDirectionRx(recv->Instance);
        LL_USART_DisableDMAReq_RX(instance);
        LL_DMA_DisableChannel(recv->DMA_Instance, recv->DMA_Channel);
		LL_USART_DisableIT_IDLE(instance);
		LL_USART_ClearFlag_IDLE(instance);
		LL_USART_DisableIT_TC(instance);
		LL_USART_ClearFlag_TC(instance);
		recv->IsStart = false;
		recv->ReceivedLen = 0;
    }
}
