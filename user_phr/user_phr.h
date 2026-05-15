
#pragma once

#include "main.h"
#include "user_i2c.h"
#include "user_uart.h"
#include "user_dma.h"
#include "user_tick.h"

extern I2C_Send_t hi2c1;
extern UART_Send_t huart1_send;
extern UART_Send_t huart3_send;
extern UART_Recv_t huart1_recv;
extern UART_Recv_t huart3_recv;

void user_phr_init(void);
