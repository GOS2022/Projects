/*
 * app_bld.c
 *
 *  Created on: Dec 26, 2024
 *      Author: Gabor
 */
#include "app.h"
#include "gos_lib.h"

/*
 * Static variables
 */
GOS_STATIC svl_pdhBldCfg_t bldCfg;
GOS_STATIC svl_pdhSwInfo_t swInfo;

GOS_STATIC svl_pdhSwVerInfo_t bldVerInfo =
{
	.author      = "Ahmed Ibrahim Gazar",
	.name        = "GOS2022_bld_stm32f4",
	.description = "GOS2022 bootloader for STM32F4 devices.",
	.major       = 1,
	.minor       = 0,
	.build       = 4,
	.date        =
	{
		.years   = 2025,
		.months  = GOS_TIME_JANUARY,
		.days    = 2
	}
};

/*
 * Function: app_bld_init
 */
gos_result_t app_bld_init (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	if (bld_initData(&bldVerInfo) == GOS_SUCCESS)
	{
		(void_t) bld_initConfig();
		(void_t) svl_pdhGetBldCfg(&bldCfg);
		(void_t) svl_pdhGetSwInfo(&swInfo);
		(void_t) bld_printConfig();

		if ((bldCfg.updateMode == GOS_TRUE) || (bldCfg.installRequested == GOS_TRUE) ||
			(bldCfg.waitForConnectionOnStartup == GOS_TRUE && bldCfg.startupCounter == 0u))
		{
			// Increase startup counter (so next startup will skip boot mode enter).
			bldCfg.startupCounter++;
			(void_t) svl_pdhSetBldCfg(&bldCfg);

			// IPL shall only be initialized if wireless update is requested.
			if (bldCfg.wirelessUpdate == GOS_TRUE)
			{
				(void_t) svl_iplInit();
			}
			else
			{
				// Nothing to do.
			}
		}
		else
		{
			// Reset startup counter so next startup will enter boot mode if startup connection wait
			// flag is set.
			bldCfg.startupCounter = 0u;
			(void_t) svl_pdhSetBldCfg(&bldCfg);

			// Check and jump to application.
			if (bld_checkApplication(&swInfo.appBinaryInfo, GOS_FALSE) == GOS_SUCCESS)
			{
				initResult = bld_jumpToApplication();
			}
			else
			{
				// Application is corrupted or missing.
				// Switch to boot update mode.
				(void_t) gos_traceTraceFormattedUnsafe("Switching to update mode...\r\n\r\n");
				bldCfg.updateMode = GOS_TRUE;
				(void_t) svl_pdhSetBldCfg(&bldCfg);
			}
		}
	}
	else
	{
		// Flash read error.
		initResult = GOS_ERROR;
	}

	return initResult;
}
