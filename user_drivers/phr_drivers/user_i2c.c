
#include "user_i2c.h"
#include "user_dma.h"
#include "debug.h"
#include "user_tick.h"

#define TAG "USER_I2C"

static I2C_SendCplt_CB_t sendCplt_cb = NULL;

void i2c_send_setcb(I2C_SendCplt_CB_t callback)
{
    sendCplt_cb = callback;
}

void i2c_send_abort(I2C_Send_t *hi2c)
{
    LL_I2C_DisableDMAReq_TX(hi2c->Instance);
    LL_DMA_DisableChannel(hi2c->DMA_Instance, hi2c->DMA_Channel);
    LL_DMA_DisableIT_TC(hi2c->DMA_Instance, hi2c->DMA_Channel);
    LL_I2C_DisableIT_EVT(hi2c->Instance);
    LL_I2C_DisableIT_ERR(hi2c->Instance);
    LL_I2C_GenerateStopCondition(hi2c->Instance);
    hi2c->State = User_I2C_State_IDLE;
}

static void i2c_send_dma_start(I2C_Send_t *hi2c)
{
    LL_I2C_DisableIT_BUF(hi2c->Instance);

    LL_DMA_SetMemoryAddress(hi2c->DMA_Instance, hi2c->DMA_Channel, (u32)(hi2c->pData));
    LL_DMA_SetPeriphAddress(hi2c->DMA_Instance, hi2c->DMA_Channel, LL_I2C_DMA_GetRegAddr(hi2c->Instance));
    LL_DMA_SetDataLength(hi2c->DMA_Instance, hi2c->DMA_Channel, hi2c->Len);
    LL_DMA_EnableIT_TC(hi2c->DMA_Instance, hi2c->DMA_Channel);
    LL_DMA_DisableIT_HT(hi2c->DMA_Instance, hi2c->DMA_Channel);
    LL_DMA_DisableIT_TE(hi2c->DMA_Instance, hi2c->DMA_Channel);
    LL_DMA_EnableChannel(hi2c->DMA_Instance, hi2c->DMA_Channel);
    LL_I2C_EnableDMAReq_TX(hi2c->Instance);
    hi2c->State = User_I2C_State_DATA;
}


/**
 * @brief Initialize i2c handle
 * 
 * @param hi2c the handle to be initialized
 * @param instance i2c instance
 * @param dma_instance dma instance
 * @param dma_channel dma channel
 */
void i2c_send_init(I2C_Send_t *hi2c, I2C_TypeDef *instance, DMA_TypeDef *dma_instance, u32 dma_channel)
{
    hi2c->Instance = instance;
    hi2c->DMA_Instance = dma_instance;
    hi2c->DMA_Channel = dma_channel;
    hi2c->State = User_I2C_State_IDLE;
}

/**
 * @brief Start i2c transmission
 * 
 * @param hi2c the handle
 * @param dat  data array to be transferred
 * @param len  length to transfer
 */
bool i2c_send_start(I2C_Send_t *hi2c, u8 slvAddr, u8 *dat, u16 len)
{
    if (hi2c->State != User_I2C_State_IDLE) return false;

    // This loop was to wait for busy flag disappear, but when an error comes accross
    // it'll be an infinite loop. So it was commented to avoid such condition.
    // while (LL_I2C_IsActiveFlag_BUSY(hi2c->Instance));

    // Enable related interrupts
    LL_I2C_EnableIT_EVT(hi2c->Instance);
    LL_I2C_EnableIT_ERR(hi2c->Instance);

    // Generate start condition and shift to slave mode.
    LL_I2C_GenerateStartCondition(hi2c->Instance);
    
    // Change handle state to start, and mark down the slave address.
    hi2c->SendMode = User_I2C_SendMode_WRITE;
    hi2c->ErrorCode = User_I2C_ErrorCode_NoError;
    hi2c->State = User_I2C_State_START;
    hi2c->SlvAddr = slvAddr;
    hi2c->pData = dat;
    hi2c->Len = len;

    return true;
}


bool i2c_mem_write_start(I2C_Send_t *hi2c, u8 slvAddr, u8 memAddr, u8 *dat, u16 len)
{
    if (hi2c->State != User_I2C_State_IDLE) return false;

    // This loop was to wait for busy flag disappear, but when an error comes accross
    // it'll be an infinite loop. So it was commented to avoid such condition.
    // while (LL_I2C_IsActiveFlag_BUSY(hi2c->Instance));

    // Enable related interrupts
    LL_I2C_EnableIT_EVT(hi2c->Instance);
    LL_I2C_EnableIT_ERR(hi2c->Instance);

    // Generate start condition and shift to slave mode.
    LL_I2C_GenerateStartCondition(hi2c->Instance);
    
    // Change handle state to start, and mark down the slave address.
    hi2c->SendMode = User_I2C_SendMode_MEMWRITE;
    hi2c->ErrorCode = User_I2C_ErrorCode_NoError;
    hi2c->State = User_I2C_State_START;
    hi2c->SlvAddr = slvAddr;
    hi2c->MemAddr = memAddr;
    hi2c->pData = dat;
    hi2c->Len = len;

    return true;
}

/**
 * @brief Get whether i2c instance completes its sending.
 * 
 * @param hi2c  the instance
 * @return bool if it's sending
 */
bool i2c_send_wait(I2C_Send_t *hi2c, u32 timeout_ms)
{
    u32 start = user_get_tick();
    while (i2c_is_sending(hi2c)) {
        if (user_get_tick() - start > timeout_ms) {
            return false;
        }
    }
    return true;
}


/**
 * @brief i2c send dma completion handler
 * 
 * @param hi2c the handle
 */
void i2c_send_dmaCplt_handler(I2C_Send_t *hi2c)
{
    if (user_dma_get_it_flag_tc(hi2c->DMA_Instance, hi2c->DMA_Channel))
    {
        user_dma_clear_it_flag_tc(hi2c->DMA_Instance, hi2c->DMA_Channel);

        if (hi2c->State == User_I2C_State_DATA)
        {
            /**
             *  DMA data transmission over, disable DMA request and wait until BTF set
             *  to end the whole transmission process.
             */
            hi2c->State = User_I2C_State_STOP;
            LL_I2C_DisableDMAReq_TX(hi2c->Instance);
            LL_DMA_DisableChannel(hi2c->DMA_Instance, hi2c->DMA_Channel);
            LL_DMA_DisableIT_TC(hi2c->DMA_Instance, hi2c->DMA_Channel);
        }
    }
    if (user_dma_get_it_flag_te(hi2c->DMA_Instance, hi2c->DMA_Channel)) {
        user_dma_clear_it_flag_te(hi2c->DMA_Instance, hi2c->DMA_Channel);
        SLOGW(TAG, "DMA channel %u failed.");
    }
}

/**
 * @brief i2c send transmission completion handler
 * 
 * @param hi2c the handle
 */
void i2c_send_it_handler(I2C_Send_t *hi2c)
{
    if (LL_I2C_IsActiveFlag_SB(hi2c->Instance))
    {
        /**
         *  Start condition has just generated, and the slave addr is going to be written
         *  to DR, and the SB flag is to be cleared (by write to DR).
         */
        LL_I2C_TransmitData8(hi2c->Instance, hi2c->SlvAddr);
        if (hi2c->State == User_I2C_State_START) {
            hi2c->State = User_I2C_State_ADDR;
        }
    }
    if (LL_I2C_IsActiveFlag_ADDR(hi2c->Instance))
    {
        /**
         *  Slave address has just been sent, and the data transfer is going to start.
         *  Clear ADDR flag and then start data transfer by DMA.
         */
        LL_I2C_ClearFlag_ADDR(hi2c->Instance);

        // DMA settings and beginning of the transmission
        if (hi2c->State == User_I2C_State_ADDR)
        {
            if (hi2c->SendMode == User_I2C_SendMode_WRITE)
            {
                i2c_send_dma_start(hi2c);
            }
            else
            {
                // If SendMode is MEMWRITE
                // enable buf interrupt and send the memaddr and wait for TXE
                // (!) Here we don't take read condition into consideration, which the LSB of SlvAddr is 1.
                if (LL_I2C_IsActiveFlag_MSL(hi2c->Instance))
                {
                    LL_I2C_EnableIT_BUF(hi2c->Instance);
                    LL_I2C_TransmitData8(hi2c->Instance, hi2c->MemAddr);
                    hi2c->State = User_I2C_State_MEMADDR;
                }
                else
                {
                    // the slv addr is wrong for its LSB = 1
                    i2c_send_abort(hi2c);
                }
            }
        }
        else
        {
            // An exception occured.
            i2c_send_abort(hi2c);
        }
    }
    if (LL_I2C_IsEnabledIT_BUF(hi2c->Instance) && LL_I2C_IsActiveFlag_TXE(hi2c->Instance))
    {
        if (hi2c->State == User_I2C_State_MEMADDR)
        {
            i2c_send_dma_start(hi2c);
        }
        else
        {
            // An exception occured.
            i2c_send_abort(hi2c);
        }
    }
    if (LL_I2C_IsActiveFlag_BTF(hi2c->Instance))
    {
        /**
         *  The whole transmission process is completed.
         *  Now we generate a stop condition to clear BTF flag.
         */
        if (hi2c->State == User_I2C_State_STOP)
        {
            LL_I2C_GenerateStopCondition(hi2c->Instance);
            // Disable some interrupts.
            LL_I2C_DisableIT_EVT(hi2c->Instance);
            LL_I2C_DisableIT_ERR(hi2c->Instance);
            hi2c->State = User_I2C_State_IDLE;

            // Call transfer completion callback
            if (sendCplt_cb != NULL) sendCplt_cb(hi2c);
        }
        else
        {
            // An exception occured.
            // Check if hi2c.len = 0
            i2c_send_abort(hi2c);
        }
    }
}


void i2c_error_handler(I2C_Send_t *hi2c)
{
    if (LL_I2C_IsActiveFlag_BERR(hi2c->Instance))
    {
        /**
         *  Unexpected bus erorr happens. Give up current transmission and
         *  reset the handle's state.
         */
        hi2c->ErrorCode = User_I2C_ErrorCode_BERR;
        LL_I2C_ClearFlag_BERR(hi2c->Instance);
        // i2c_send_abort(hi2c);
        SLOGE(TAG, "BERR(a misplaced Start or Stop condition) occured.");
    }
    if (LL_I2C_IsActiveFlag_AF(hi2c->Instance))
    {
        LL_I2C_ClearFlag_AF(hi2c->Instance);
        if (LL_I2C_IsActiveFlag_MSL(hi2c->Instance))
        {
            hi2c->ErrorCode = User_I2C_ErrorCode_ARLO;
            // In master mode, generate a stop condition.
            // i2c_send_abort(hi2c);
            SLOGE(TAG, "AF(Ack Failure) detected.");
        }
    }
    if (LL_I2C_IsActiveFlag_OVR(hi2c->Instance))
    {
        LL_I2C_ClearFlag_OVR(hi2c->Instance);
        hi2c->ErrorCode = User_I2C_ErrorCode_OVR;
        // i2c_send_abort(hi2c);
        SLOGE(TAG, "OVR(Overrun/underrun) err occurs.");
    }

    i2c_send_abort(hi2c);
    SLOGW(TAG, "Transmission is aborted.");
}
