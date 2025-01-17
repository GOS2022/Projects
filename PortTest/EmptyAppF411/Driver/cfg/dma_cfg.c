/*
 * dma_cfg.h
 *
 *  Created on: Feb 22, 2024
 *      Author: Gabor
 */

#ifndef DRIVER_INC_DMA_CFG_C_
#define DRIVER_INC_DMA_CFG_C_

#include <drv_dma.h>
#include <cfg_def.h>
#include "stm32f4xx_hal.h"

/**
 * TODO
 */
drv_dmaDescriptor_t dmaConfig [] =
{

};

/**
 * TODO
 */
u32_t dmaConfigSize = sizeof(dmaConfig);

/**
 * TODO
 */
drv_dmaDescriptor_t* pDMADescriptorLut [DRV_DMA_X_STREAM_NUM] =
{
	[DRV_DMA_1_STREAM_0] = NULL,
	[DRV_DMA_1_STREAM_1] = NULL,
	[DRV_DMA_1_STREAM_2] = NULL,
	[DRV_DMA_1_STREAM_3] = NULL,
	[DRV_DMA_1_STREAM_4] = NULL,
	[DRV_DMA_1_STREAM_5] = NULL,
	[DRV_DMA_1_STREAM_6] = NULL,
	[DRV_DMA_1_STREAM_7] = NULL,

	[DRV_DMA_2_STREAM_0] = NULL,
	[DRV_DMA_2_STREAM_1] = NULL,
	[DRV_DMA_2_STREAM_2] = NULL,
	[DRV_DMA_2_STREAM_3] = NULL,
	[DRV_DMA_2_STREAM_4] = NULL,
	[DRV_DMA_2_STREAM_5] = NULL,
	[DRV_DMA_2_STREAM_6] = NULL,
	[DRV_DMA_2_STREAM_7] = NULL,
};

#endif /* DRIVER_INC_DMA_CFG_C_ */
