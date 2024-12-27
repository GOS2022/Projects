/*
 * spi_cfg.c
 *
 *  Created on: Mar 19, 2024
 *      Author: Gabor
 */

#include <cfg_def.h>
#include "drv_spi.h"

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
	}
};

/**
 * TODO
 */
u32_t spiConfigSize = sizeof(spiConfig);
