/*
 * uart_cfg.c
 *
 *  Created on: Feb 21, 2024
 *      Author: Gabor
 */
#include <app.h>
#include <gos_lib.h>

#include "pwm_driver.h"
#include "rcc_driver.h"

/**
 * TODO
 */
GOS_CONST svl_dsmInitPhaseDesc_t appInitPhaseConfig [] =
{
	{
		// Phase: SVL
		"Initializing SVL...",
		{
			{ "Sysmon initialization",              svl_sysmonInit              },
			{ "PDH initialization",                 svl_pdhInit                 },
			//{ "IPL initialization",                 svl_iplInit                 }, TODO
			{ "SDH initialization",                 svl_sdhInit                 },
			{ "MDI initialization",                 svl_mdiInit                 },
			{ "ERS initialization",                 svl_ersInit                 },
			{ "DHS initialization",                 svl_dhsInit                 }

		}
	},
	{
		// Phase: user tasks.
		"Initializing user tasks...",
		{
			//{ "IPL board-specific initialization",  app_iplBdSpecInit           },
			{ "PDH board-specific initialization",  app_pdhBdSpecInit           },
			{ "SDH board-specific initialization",  app_sdhBdSpecInit           },
			{ "MDI board-specific initialization",  app_mdiBdSpecInit           },
			{ "ERS board-specific initialization",  app_ersBdSpecInit           },
			{ "LED initialization",                 app_ledInit                 },
			{ "LCD initialization",                 app_lcdInit                 },
			{ "Debouncer initialization",           app_debouncerInit           },
			{ "RTC handler initialization",         bsp_rtcHandlerInit          },
			{ "Ex-IO handler initialization",       bsp_exioHandlerInit         },
			{ "TFT initialization",                 app_tftInit                 },
			{ "Ex-IO initialization",               app_exioInit                },
			{ "DHS test initialization",            app_dhsTestInit             }
		}
	}
};


/**
 * TODO
 */
u32_t appInitPhaseConfigSize = sizeof(appInitPhaseConfig);

/**
 * TODO
 */
GOS_CONST svl_dsmInitPhaseDesc_t platformInitPhaseConfig [] =
{
	{
		// Phase: DRV
		"Initializing drivers...",
		{
			{ "RCC driver initialization",           rcc_driver_init            },
			{ "PWM driver initialization",           pwm_driver_init            },
			{ "Timer driver initialization",         drv_tmrInit                },
			{ "System timer start",                  drv_systimerStart          },
			{ "GPIO driver initialization",          drv_gpioInit               },
			{ "I2C driver initialization",           drv_i2cInit                },
			{ "SPI driver initialization",           drv_spiInit                },
			{ "DMA driver initialization",           drv_dmaInit                },
			{ "UART driver initialization",          drv_uartInit               },
			{ "IT driver initialization",            drv_itInit                 },
			{ "ADC driver initialization",           drv_adcInit                },
			{ "SPI driver initialization",           drv_spiInit                }
		}
	},
};

/**
 * TODO
 */
u32_t platformInitPhaseConfigSize = sizeof(platformInitPhaseConfig);

/**
 * TODO
 */
GOS_CONST svl_dsmReaction_t reactionConfig [] = {};

/**
 * TODO
 */
u32_t reactionConfigSize = sizeof(reactionConfig);
