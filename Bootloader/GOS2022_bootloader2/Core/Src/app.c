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
#include "trace_driver.h"

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
	/*
	 * Local variables.
	 */
	// Platform driver initialization result.
	gos_result_t platformDriverInitResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    // Low-level initialization.
    platformDriverInitResult &= trace_driverEnqueueTraceMessage(
    		"HAL library initialization",
			HAL_Init() == HAL_OK ? GOS_SUCCESS : GOS_ERROR
    );

    // Driver initialization.
    platformDriverInitResult &= trace_driverEnqueueTraceMessage("Driver initialization", driver_init());

    // Register kernel drivers.
    platformDriverInitResult &= trace_driverEnqueueTraceMessage("Kernel driver registration", gos_driverInit(&driverFunctions));

    SysTick->VAL = 0;
    SysTick->CTRL = 0b111;

    __enable_irq();

    // Initialize bootloader.
    platformDriverInitResult &= trace_driverEnqueueTraceMessage("PDH board-specific initialization", app_pdhBdSpecInit());
    platformDriverInitResult &= trace_driverEnqueueTraceMessage("APP bootloader initialization", app_bld_init());

    if (platformDriverInitResult != GOS_SUCCESS)
	{
    	platformDriverInitResult = GOS_ERROR;
	}

	return platformDriverInitResult;
}

/*
 * Function: gos_userApplicationInit
 */
gos_result_t gos_userApplicationInit (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t appInitResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    // Flush trace entries.
    trace_driverFlushTraceEntries();

	// Initialize device state manager for startup.
	appInitResult &= svl_dsmInit();

	if (appInitResult != GOS_SUCCESS)
	{
		appInitResult = GOS_ERROR;
	}

	return appInitResult;
}


