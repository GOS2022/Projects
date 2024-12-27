/*
 * uart_cfg.c
 *
 *  Created on: Feb 21, 2024
 *      Author: Gabor
 */
#include <app.h>
#include <gos_lib.h>

/**
 * TODO
 */
GOS_CONST svl_dsmInitPhaseDesc_t initPhaseConfig [] =
{
	{
		// Phase: SVL
		"Initializing SVL...",
		{
			{ "Bootloader handler initialization",  svl_bldHandlerInit          },
			{ "PDH initialization",                 svl_pdhInit                 },
			{ "IPL initialization",                 svl_iplInit                 },
			{ "SDH initialization",                 svl_sdhInit                 }
		}
	},
	{
		// Phase: user tasks.
		"Initializing user tasks...",
		{
			{ "IPL board-specific initialization",  app_iplBdSpecInit           },
			{ "PDH board-specific initialization",  app_pdhBdSpecInit           },
			{ "SDH board-specific initialization",  app_sdhBdSpecInit           },
			{ "LED initialization",                 APP_LedInit                 },
		}
	}
};


/**
 * TODO
 */
u32_t initPhaseConfigSize = sizeof(initPhaseConfig);

/**
 * TODO
 */
GOS_CONST svl_dsmReaction_t reactionConfig [] = {};

/**
 * TODO
 */
u32_t reactionConfigSize = sizeof(reactionConfig);
