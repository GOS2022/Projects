/*
 * adc_cfg.c
 *
 *  Created on: Apr 12, 2024
 *      Author: Gabor
 */

#include <cfg_def.h>
#include "drv_adc.h"

GOS_EXTERN drv_dmaDescriptor_t dmaConfig [];

/**
 * TODO
 */
GOS_CONST drv_adcDescriptor_t adcConfig [] =
{
	{
		.periphInstance        = DRV_ADC_INSTANCE_1,
		.clockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4,
		.continuousConvMode    = DISABLE,
		.dataAlign             = ADC_DATAALIGN_RIGHT,
		.discontinuousConvMode = DISABLE,
		.resolution            = ADC_RESOLUTION_12B,
		.scanConvMode          = ENABLE,
		.externalTrigConv      = ADC_SOFTWARE_START,
		.externalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE,
		.nbrOfConversion       = 4,
		.dmaContinuousRequests = DISABLE,
		.eocSelection          = ADC_EOC_SINGLE_CONV,
		//.dmaConfig             = &dmaConfig[DMA_CFG_ADC1]
	}
};

/**
 * TODO
 */
GOS_CONST drv_adcChannelDesc_t adcChannelConfig [] =
{
	{
		.periphInstance        = DRV_ADC_INSTANCE_1,
		.channel               = ADC_CHANNEL_TEMPSENSOR,
		.rank                  = 1,
		.samplingTime          = ADC_SAMPLETIME_84CYCLES
	},
	{
		.periphInstance        = DRV_ADC_INSTANCE_1,
		.channel               = ADC_CHANNEL_VREFINT,
		.rank                  = 1,
		.samplingTime          = ADC_SAMPLETIME_15CYCLES
	},
	{
		.periphInstance        = DRV_ADC_INSTANCE_1,
		.channel               = ADC_CHANNEL_10,
		.rank                  = 1,
		.samplingTime          = ADC_SAMPLETIME_15CYCLES
	},
	{
		.periphInstance        = DRV_ADC_INSTANCE_1,
		.channel               = ADC_CHANNEL_13,
		.rank                  = 1,
		.samplingTime          = ADC_SAMPLETIME_15CYCLES
	},
};

/**
 * TODO
 */
u32_t adcConfigSize = sizeof(adcConfig);

/**
 * TODO
 */
u32_t adcChannelConfigSize = sizeof(adcChannelConfig);
