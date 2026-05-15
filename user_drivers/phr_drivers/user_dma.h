#ifndef __USER_DMA_H
#define __USER_DMA_H

#include "main.h"

bool user_dma_get_it_flag_tc(DMA_TypeDef *dma, u32 channel);
void user_dma_clear_it_flag_gi(DMA_TypeDef *dma, u32 channel);
void user_dma_clear_it_flag_tc(DMA_TypeDef *dma, u32 channel);
bool user_dma_get_it_flag_te(DMA_TypeDef *dma, u32 channel);
void user_dma_clear_it_flag_te(DMA_TypeDef *dma, u32 channel);

#endif // !__USER_DMA_H
