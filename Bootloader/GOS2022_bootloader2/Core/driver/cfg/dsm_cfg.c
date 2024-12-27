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
		// Phase: SVL.
		"Initializing SVL...",
		{
			{ "BLD initialization",                 bld_init                    },
			{ "PDH initialization",                 svl_pdhInit                 },
			{ "SDH initialization",                 svl_sdhInit                 }
		}
	},
	{
		// Phase: user tasks.
		"Initializing user tasks...",
		{
			{ "IPL board-specific initialization",  app_iplBdSpecInit           },
			{ "SDH board-specific initialization",  app_sdhBdSpecInit           },
			{ "LED task initialization",            app_ledInit                 },
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
