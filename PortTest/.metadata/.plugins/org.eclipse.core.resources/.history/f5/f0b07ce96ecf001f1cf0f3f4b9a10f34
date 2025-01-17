/*
 * app.c
 *
 *  Created on: Sep 15, 2022
 *      Author: Gabor
 */

#include <stdio.h>
#include "app.h"
#include "driver.h"
#include "drv.h"

#include <gos_lib.h>

GOS_STATIC gos_driver_functions_t driverFunctions =
{
	.traceDriverTransmitString       = drv_traceTransmit,
	.traceDriverTransmitStringUnsafe = drv_traceTransmitUnsafe,
	.shellDriverTransmitString       = drv_shellTransmitString,
	.shellDriverReceiveChar          = drv_shellReceiveChar,
	.sysmonDriverTransmit            = drv_sysmonTransmit,
	.sysmonDriverReceive             = drv_sysmonReceive,
	.timerDriverSysTimerGetValue     = drv_systimerGetValue,
};

/*
 * Function: gos_platformDriverInit
 */
gos_result_t gos_platformDriverInit (void_t)
{
	// Platform driver initialization result.
	gos_result_t platformDriverInitResult = GOS_SUCCESS;

	// Driver init.
	if (HAL_Init() != HAL_OK)
	{
		platformDriverInitResult = GOS_ERROR;
	}

	platformDriverInitResult = driver_init();

	// Register kernel drivers.
	platformDriverInitResult &= gos_driverInit(&driverFunctions);

    if (platformDriverInitResult != GOS_SUCCESS)
	{
    	platformDriverInitResult = GOS_ERROR;
	}

    SysTick->VAL = 0;
    SysTick->CTRL = 0b111;

    __enable_irq();

	return platformDriverInitResult;
}

/*
 * Function: gos_userApplicationInit
 */
gos_result_t gos_userApplicationInit (void_t)
{
	gos_result_t appInitResult = GOS_SUCCESS;

	// Initialize device state manager for startup.
	appInitResult &= svl_dsmInit();

	// Welcome message.
	(void_t) gos_traceTrace(GOS_FALSE, "Welcome to IPL test application!");

	if (appInitResult != GOS_SUCCESS)
	{
		appInitResult = GOS_ERROR;
	}

	return appInitResult;
}
