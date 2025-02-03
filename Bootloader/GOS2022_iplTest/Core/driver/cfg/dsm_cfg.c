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
			{ "PDH initialization",                 svl_pdhInit                 },
			{ "IPL initialization",                 svl_iplInit                 },
			{ "SDH initialization",                 svl_sdhInit                 },
			{ "MDI initialization",                 svl_mdiInit                 },
			{ "ERS initialization",                 svl_ersInit                 },
		}
	},
	{
		// Phase: user tasks.
		"Initializing user tasks...",
		{
			{ "IPL board-specific initialization",  app_iplBdSpecInit           },
			{ "PDH board-specific initialization",  app_pdhBdSpecInit           },
			{ "SDH board-specific initialization",  app_sdhBdSpecInit           },
			{ "MDI board-specific initialization",  app_mdiBdSpecInit           },
			{ "ERS board-specific initialization",  app_ersBdSpecInit           },
			{ "LED initialization",                 app_ledInit                 },
			{ "LCD initialization",                 app_lcdInit                 },
			{ "Debouncer initialization",           app_debouncerInit           },
			{ "RTC handler initialization",         bsp_rtcHandlerInit          },
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
