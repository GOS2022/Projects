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
			{ "IPL initialization",                 svl_iplInit                 }
		}
	},
	{
		// Phase: user tasks.
		"Initializing user tasks...",
		{
			{ "IPL board-specific initialization",  app_iplBdSpecInit           },
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
