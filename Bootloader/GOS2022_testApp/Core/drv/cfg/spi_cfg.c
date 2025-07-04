/*
 * spi_cfg.c
 *
 *  Created on: Mar 19, 2024
 *      Author: Gabor
 */

#include "drv_spi.h"
#include "stm32f4xx_hal.h"
#include "cfg_def.h"

GOS_EXTERN drv_dmaDescriptor_t dmaConfig [];

/**
 * TODO
 */
GOS_CONST drv_spiDescriptor_t spiConfig [] =
{
	{
		.periphInstance    = DRV_SPI_INSTANCE_2,
		.mode              = SPI_MODE_MASTER,
		.direction         = SPI_DIRECTION_2LINES,
		.dataSize          = SPI_DATASIZE_8BIT,
		.clkPolarity       = SPI_POLARITY_LOW,
		.clkPhase          = SPI_PHASE_1EDGE,
		.nss               = SPI_NSS_SOFT,
		.baudRatePrescaler = SPI_BAUDRATEPRESCALER_8,
		.firstBit          = SPI_FIRSTBIT_MSB,
		.tiMode            = SPI_TIMODE_DISABLE,
		.crcCalculation    = SPI_CRCCALCULATION_DISABLE,
		.crcPolynomial     = 10
	},
	{
		.periphInstance    = DRV_SPI_INSTANCE_3,
		.mode              = SPI_MODE_MASTER,
		.direction         = SPI_DIRECTION_2LINES,
		.dataSize          = SPI_DATASIZE_8BIT,
		.clkPolarity       = SPI_POLARITY_LOW,
		.clkPhase          = SPI_PHASE_1EDGE,
		.nss               = SPI_NSS_SOFT,
		.baudRatePrescaler = SPI_BAUDRATEPRESCALER_4,
		.firstBit          = SPI_FIRSTBIT_MSB,
		.tiMode            = SPI_TIMODE_DISABLE,
		.crcCalculation    = SPI_CRCCALCULATION_DISABLE,
		.crcPolynomial     = 10,
		.dmaConfigTx       = &dmaConfig[DMA_CFG_SPI3]
	}
};

/**
 * TODO
 */
u32_t spiConfigSize = sizeof(spiConfig);
