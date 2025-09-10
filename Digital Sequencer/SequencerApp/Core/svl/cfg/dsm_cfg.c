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
//                                      (c) Ahmed Gazar, 2025
//
//*************************************************************************************************
//! @file       dsm_cfg.c
//! @author     Ahmed Gazar
//! @date       2025-09-10
//! @version    1.0
//!
//! @brief      Digital Step Sequencer / SVL Configuration / DSM configuration source.
//! @details    This file contains the configuration parameters of the DSM service.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-09-10    Ahmed Gazar     Initial version created.
//*************************************************************************************************
//
// Copyright (c) 2025 Ahmed Gazar
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
#include <gos_lib.h>
#include "bsp_io_handler.h"
#include "bsp_adc_handler.h"
#include "app_control.h"

/*
 * Global variables
 */
/**
 * Application initialization phase configuration.
 */
GOS_CONST svl_dsmInitPhaseDesc_t appInitPhaseConfig [] =
{
	{
		"Initializing SVL...",
		{
			{ "Sysmon initialization",              svl_sysmonInit              },
		}
	},
	{
		"Initializing user tasks...",
		{
			{ "IO handler initialization",          bsp_ioHandlerInit           },
			{ "ADC handler initialization",         bsp_adcHandlerInit          },
			{" APP control initialization",         app_controlInit             },
		}
	}
};

/**
 * Application initialization phase configuration size (needed for component).
 */
u32_t appInitPhaseConfigSize = sizeof(appInitPhaseConfig);

/**
 * Platform initialization phase configuration.
 */
GOS_CONST svl_dsmInitPhaseDesc_t platformInitPhaseConfig [] =
{
	{
		"Initializing drivers...",
		{
			{ "RCC driver initialization",           drv_rccInit                },
			{ "Timer driver initialization",         drv_tmrInit                },
			{ "System timer start",                  drv_systimerStart          },
			{ "GPIO driver initialization",          drv_gpioInit               },
			{ "I2C driver initialization",           drv_i2cInit                },
			{ "DMA driver initialization",           drv_dmaInit                },
			{ "UART driver initialization",          drv_uartInit               },
			{ "IT driver initialization",            drv_itInit                 },
		}
	},
};

/**
 * Platform initialization phase configuration size (needed for component).
 */
u32_t platformInitPhaseConfigSize = sizeof(platformInitPhaseConfig);

/**
 * Reaction configuration.
 */
GOS_CONST svl_dsmReaction_t reactionConfig [] = {};

/**
 * Reaction configuration size (needed for component).
 */
u32_t reactionConfigSize = sizeof(reactionConfig);
