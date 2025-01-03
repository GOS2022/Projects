//*************************************************************************************************
//
//                            #####             #####             #####
//                          #########         #########         #########
//                         ##                ##       ##       ##
//                        ##                ##         ##        #####
//                        ##     #####      ##         ##           #####
//                         ##       ##       ##       ##                ##
//                          #########         #########         #########
//                            #####             #####             #####
//
//                                      (c) Ahmed Gazar, 2024
//
//*************************************************************************************************
//! @file       it_cfg.c
//! @author     Ahmed Gazar
//! @date       2024-05-03
//! @version    1.0
//!
//! @brief      Interrupt configuration source.
//! @details    This component contains the interrupt configuration parameters.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-05-03    Ahmed Gazar     Initial version created.
//*************************************************************************************************
//
// Copyright (c) 2024 Ahmed Gazar
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//*************************************************************************************************
/*
 * Includes
 */
#include <drv_it.h>
#include "stm32f4xx_hal.h"

/**
 * Interrupt configuration array.
 */
GOS_CONST drv_itDescriptor_t itConfig [] =
{
	{
		.irqType        = USART1_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = USART2_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = DMA1_Stream0_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = DMA1_Stream6_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
	{
		.irqType        = EXTI15_10_IRQn,
		.preemtPriority = 0,
		.subPriority    = 0
	},
};

/**
 * Interrupt configuration size.
 */
u32_t itConfigSize = sizeof(itConfig);
