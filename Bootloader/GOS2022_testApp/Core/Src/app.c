/*
 * app.c
 *
 *  Created on: Sep 15, 2022
 *      Author: Ahmed Ibrahim Gazar
 */

#include <stdio.h>
#include "app.h"
#include "drv.h"
#include "iodef.h"

#include <gos_lib.h>

GOS_STATIC gos_driver_functions_t driverFunctions =
{
	.traceDriverTransmitString       = drv_traceTransmit,
	.traceDriverTransmitStringUnsafe = drv_traceTransmitUnsafe,
	.timerDriverSysTimerGetValue     = drv_systimerGetValue,
};

/*
 * Function: gos_platformDriverInit
 */
//gos_result_t gos_platformDriverInit (void_t)
gos_result_t svl_dsmPlatformInit (void_t)
{
	/*
	 * Local variables.
	 */
	// Platform driver initialization result.
	gos_result_t platformDriverInitResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	// Driver init.
	if (HAL_Init() != HAL_OK)
	{
		platformDriverInitResult = GOS_ERROR;
	}

	platformDriverInitResult &= drv_traceEnqueueTraceMessage("Register systick hook", GOS_FALSE, gos_kernelRegisterSysTickHook(/*sysTickHook*/HAL_IncTick));

	// Register kernel drivers.
	platformDriverInitResult &= gos_driverInit(&driverFunctions);

    GOS_CONVERT_RESULT(platformDriverInitResult);

    SysTick->VAL = 0;
    SysTick->CTRL = 0b111;

    __enable_irq();

	return platformDriverInitResult;
}

/*
 * Function: gos_userApplicationInit
 */
//gos_result_t gos_userApplicationInit (void_t)
gos_result_t svl_dsmApplicationInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t appInitResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	// Initialize device state manager for startup.
	//appInitResult &= svl_dsmInit();

	// Welcome message.
	(void_t) gos_traceTrace(GOS_FALSE, "Welcome to the GOS test application!");

	// Enable WiFi module.
	(void_t) drv_gpioWritePin(IO_WEMOS_RST, GPIO_STATE_HIGH);

	if (appInitResult != GOS_SUCCESS)
	{
		appInitResult = GOS_ERROR;
	}

	return appInitResult;
}
