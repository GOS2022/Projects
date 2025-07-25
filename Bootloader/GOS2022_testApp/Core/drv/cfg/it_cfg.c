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
		.irqType        = UART5_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = DMA1_Stream0_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = DMA2_Stream7_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = USART1_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = TIM3_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = TIM2_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = UART4_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = DMA1_Stream2_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = I2C1_EV_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = SPI2_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = DMA2_Stream4_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = ADC_IRQn,
		.preemtPriority = 1,
		.subPriority    = 0
	},
	{
		.irqType        = EXTI15_10_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = SPI3_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = DMA1_Stream7_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
};

/**
 * TODO
 */
u32_t itConfigSize = sizeof(itConfig);
