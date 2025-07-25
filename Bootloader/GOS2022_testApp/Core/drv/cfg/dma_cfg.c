/*
 * dma_cfg.h
 *
 *  Created on: Feb 22, 2024
 *      Author: Gabor
 */

#ifndef DRIVER_INC_DMA_CFG_C_
#define DRIVER_INC_DMA_CFG_C_

#include <drv_dma.h>
#include "stm32f4xx_hal.h"
#include "cfg_def.h"

/**
 * TODO
 */
drv_dmaDescriptor_t dmaConfig [] =
{
	// DMA1 stream 0 -> UART5 RX
	[DMA_CFG_UART5_RX] =
	{
		.dmaStream           = DRV_DMA_1_STREAM_0,
	    .channel             = DMA_CHANNEL_4,
	    .direction           = DMA_PERIPH_TO_MEMORY,
	    .periphInc           = DMA_PINC_DISABLE,
	    .memInc              = DMA_MINC_ENABLE,
	    .periphDataAlignment = DMA_PDATAALIGN_BYTE,
	    .memDataAlignment    = DMA_MDATAALIGN_BYTE,
	    .mode                = DMA_NORMAL,
	    .priority            = DMA_PRIORITY_LOW,
	    .fifoMode            = DMA_FIFOMODE_DISABLE,
	},
	// DMA2 stream 7 -> USART1 TX
	[DMA_CFG_USART1] =
	{
		.dmaStream           = DRV_DMA_2_STREAM_7,
		.channel             = DMA_CHANNEL_4,
		.direction           = DMA_MEMORY_TO_PERIPH,
		.periphInc           = DMA_PINC_DISABLE,
		.memInc              = DMA_MINC_ENABLE,
		.periphDataAlignment = DMA_PDATAALIGN_BYTE,
		.memDataAlignment    = DMA_MDATAALIGN_BYTE,
		.mode                = DMA_NORMAL,
		.priority            = DMA_PRIORITY_LOW,
		.fifoMode            = DMA_FIFOMODE_DISABLE,
	},
	// DMA1 stream 2 -> UART4 RX
	[DMA_CFG_UART4_RX] =
	{
		.dmaStream           = DRV_DMA_1_STREAM_2,
		.channel             = DMA_CHANNEL_4,
		.direction           = DMA_PERIPH_TO_MEMORY,
	    .periphInc           = DMA_PINC_DISABLE,
	    .memInc              = DMA_MINC_ENABLE,
	    .periphDataAlignment = DMA_PDATAALIGN_BYTE,
	    .memDataAlignment    = DMA_MDATAALIGN_BYTE,
	    .mode                = DMA_NORMAL,
	    .priority            = DMA_PRIORITY_LOW,
	    .fifoMode            = DMA_FIFOMODE_DISABLE,
	},
	// DMA2 stream 4 -> ADC1
	[DMA_CFG_ADC1] =
	{
		.dmaStream           = DRV_DMA_2_STREAM_4,
		.channel             = DMA_CHANNEL_0,
		.direction           = DMA_PERIPH_TO_MEMORY,
	    .periphInc           = DMA_PINC_DISABLE,
	    .memInc              = DMA_MINC_ENABLE,
	    .periphDataAlignment = DMA_PDATAALIGN_HALFWORD,
	    .memDataAlignment    = DMA_MDATAALIGN_HALFWORD,
	    .mode                = DMA_NORMAL,
	    .priority            = DMA_PRIORITY_LOW,
	    .fifoMode            = DMA_FIFOMODE_DISABLE,
	},
	// DMA1 stream 7 -> SPI3 TX
	[DMA_CFG_SPI3] =
	{
		.dmaStream           = DRV_DMA_1_STREAM_7,
		.channel             = DMA_CHANNEL_0,
		.direction           = DMA_MEMORY_TO_PERIPH,
		.periphInc           = DMA_PINC_DISABLE,
		.memInc              = DMA_MINC_ENABLE,
		.periphDataAlignment = DMA_PDATAALIGN_BYTE,
		.memDataAlignment    = DMA_MDATAALIGN_BYTE,
		.mode                = DMA_NORMAL,
		.priority            = DMA_PRIORITY_LOW,
		.fifoMode            = DMA_FIFOMODE_DISABLE,
	},
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
	[DRV_DMA_1_STREAM_0] = &dmaConfig[DMA_CFG_UART5_RX],
	[DRV_DMA_1_STREAM_1] = NULL,
	[DRV_DMA_1_STREAM_2] = &dmaConfig[DMA_CFG_UART4_RX],
	[DRV_DMA_1_STREAM_3] = NULL,
	[DRV_DMA_1_STREAM_4] = NULL,
	[DRV_DMA_1_STREAM_5] = NULL,
	[DRV_DMA_1_STREAM_6] = NULL,
	[DRV_DMA_1_STREAM_7] = &dmaConfig[DMA_CFG_SPI3],

	[DRV_DMA_2_STREAM_0] = NULL,
	[DRV_DMA_2_STREAM_1] = NULL,
	[DRV_DMA_2_STREAM_2] = NULL,
	[DRV_DMA_2_STREAM_3] = NULL,
	[DRV_DMA_2_STREAM_4] = &dmaConfig[DMA_CFG_ADC1],
	[DRV_DMA_2_STREAM_5] = NULL,
	[DRV_DMA_2_STREAM_6] = NULL,
	[DRV_DMA_2_STREAM_7] = &dmaConfig[DMA_CFG_USART1],
};

#endif /* DRIVER_INC_DMA_CFG_C_ */
