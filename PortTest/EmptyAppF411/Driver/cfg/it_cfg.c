/*
 * it_cfg.c
 *
 *  Created on: Mar 7, 2024
 *      Author: Gabor
 */

#include <drv_it.h>
#include "stm32f4xx_hal.h"

/**
 * TODO
 */
GOS_CONST drv_itDescriptor_t itConfig [] =
{
	{
		.irqType        = USART2_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
};

/**
 * TODO
 */
u32_t itConfigSize = sizeof(itConfig);
