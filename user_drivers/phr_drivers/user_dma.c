
#include "user_dma.h"

/**
 * @brief Get status of DMA transmission complete by channel number.
 * 
 * @param dma 		DMA Instance
 * @param channel 	DMA Channel
 * @return bool 	If DMA transmission interrupt of the respective channel occurs
 */
bool user_dma_get_it_flag_tc(DMA_TypeDef *dma, u32 channel)
{
    if (--channel > 6) return false;
    return (READ_BIT(dma->ISR, 0x00000002 << (channel << 2)) ? true : false);
}

/**
 * @brief Get TE flag by channel number.
 * 
 * @param dma 		DMA Instance
 * @param channel 	DMA Channel
 * @return bool 	If TE is set
 */
bool user_dma_get_it_flag_te(DMA_TypeDef *dma, u32 channel)
{
    if (--channel > 6) return false;
    return (READ_BIT(dma->ISR, 0x00000008 << (channel << 2)) ? true : false);
}

/**
 * @brief Clear the GIx, TCx, TEx, HTx flag of DMA of correspondent channel.
 * 
 * @param dma       DMA Instance
 * @param channel   DMA Channel
 */
void user_dma_clear_it_flag_gi(DMA_TypeDef *dma, u32 channel)
{
    if (--channel > 6) return;
    SET_BIT(dma->IFCR, (0x00000001 << (channel << 2)));
}

/**
 * @brief Clear the TCx flag of DMA of correspondent channel.
 * 
 * @param dma       DMA Instance
 * @param channel   DMA Channel
 */
void user_dma_clear_it_flag_tc(DMA_TypeDef *dma, u32 channel)
{
    if (--channel > 6) return;
    SET_BIT(dma->IFCR, (0x00000002 << (channel << 2)));
}

/**
 * @brief Clear the TEx flag of DMA of correspondent channel.
 * 
 * @param dma       DMA Instance
 * @param channel   DMA Channel
 */
void user_dma_clear_it_flag_te(DMA_TypeDef *dma, u32 channel)
{
    if (--channel > 6) return;
    SET_BIT(dma->IFCR, (0x00000008 << (channel << 2)));
}
