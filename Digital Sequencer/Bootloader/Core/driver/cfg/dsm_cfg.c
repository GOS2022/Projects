// TODO
#include <app.h>
#include <gos_lib.h>

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
			{ "BLD initialization",                 bld_init                    },
			{ "PDH initialization",                 svl_pdhInit                 },
			{ "SDH initialization",                 svl_sdhInit                 },
			{ "DHS initialization",                 svl_dhsInit                 }
		}
	},
	{
		// Phase: user tasks.
		"Initializing user tasks...",
		{
			//{ "PDH board-specific initialization",  app_pdhBdSpecInit           },
			{ "SDH board-specific initialization",  app_sdhBdSpecInit           },
			{ "BLD task initialization",            app_bldTaskInit             },
			//{ "APP bootloader initialization",      app_bld_init                }
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
GOS_CONST svl_dsmInitPhaseDesc_t platformInitPhaseConfig [] = {};
/*{
	{
		// Phase: DRV
		"Initializing drivers...",
		{
			{ "RCC driver initialization",           rcc_driver_init            },
			{ "Timer driver initialization",         drv_tmrInit                },
			{ "System timer start",                  drv_systimerStart          },
			{ "GPIO driver initialization",          drv_gpioInit               },
			{ "I2C driver initialization",           drv_i2cInit                },
			{ "SPI driver initialization",           drv_spiInit                },
			{ "DMA driver initialization",           drv_dmaInit                },
			{ "UART driver initialization",          drv_uartInit               },
			{ "IT driver initialization",            drv_itInit                 }
		}
	},
};*/

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

