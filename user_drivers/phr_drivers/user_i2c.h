#ifndef __USER_I2C_H
#define __USER_I2C_H

#include "main.h"

/*------------------------------------ Private Defines and Typedefs ---------------------------------*/

#define User_I2C_State_IDLE							0
#define User_I2C_State_START						1
#define User_I2C_State_ADDR							2
#define User_I2C_State_MEMADDR                      3
#define User_I2C_State_DATA							4
#define User_I2C_State_STOP							5

#define User_I2C_ErrorCode_NoError					0
#define User_I2C_ErrorCode_BERR						1
#define User_I2C_ErrorCode_AF						2
#define User_I2C_ErrorCode_ARLO						3
#define User_I2C_ErrorCode_OVR						4

#define User_I2C_SendMode_WRITE						1
#define User_I2C_SendMode_MEMWRITE					2

typedef struct I2C_Send_t
{
    I2C_TypeDef *Instance;					// the i2c instance
    DMA_TypeDef *DMA_Instance;				// the dma instance
    u32 DMA_Channel;						// the dma channel

    u8 State;								// i2c transmission state
    u8 SendMode;                            // write or mem_write
	u8 SlvAddr;								// slave address
    u8 MemAddr;								// memory address
	u8 *pData;								// data buffer of DMA tarnsfer
	u16 Len;								// length of DMA tarnsfer
	u8 ErrorCode;							// The error code of i2c peripheral
} I2C_Send_t;

typedef void (*I2C_SendCplt_CB_t)(I2C_Send_t *hi2c);


/*------------------------------------ Exported Functions ---------------------------------*/

#define i2c_is_sending(hi2c) ((bool)((hi2c)->State))

void i2c_send_init(I2C_Send_t *hi2c, I2C_TypeDef *instance, DMA_TypeDef *dma_instance, u32 dma_channel);
bool i2c_send_start(I2C_Send_t *hi2c, u8 slvAddr, u8 *dat, u16 len);
bool i2c_mem_write_start(I2C_Send_t *hi2c, u8 slvAddr, u8 memAddr, u8 *dat, u16 len);
bool i2c_send_wait(I2C_Send_t *hi2c, u32 ms);
void i2c_send_dmaCplt_handler(I2C_Send_t *hi2c);
void i2c_send_it_handler(I2C_Send_t *hi2c);
void i2c_error_handler(I2C_Send_t *hi2c);
void i2c_send_abort(I2C_Send_t *hi2c);
void i2c_send_setcb(I2C_SendCplt_CB_t callback);

#endif // !__USER_I2C_H
