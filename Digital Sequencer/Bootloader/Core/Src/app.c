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
#include "iodef.h"

#include <gos_lib.h>

GOS_STATIC gos_driver_functions_t driverFunctions =
{
	.traceDriverTransmitString       = drv_traceTransmit,
	.traceDriverTransmitStringUnsafe = drv_traceTransmitUnsafe,
	.timerDriverSysTimerGetValue     = drv_systimerGetValue,
};

/*
 * Function: svl_dsmPlatformInit
 */
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
    platformDriverInitResult &= trace_driverEnqueueTraceMessage(
    		"HAL library initialization",
			HAL_Init() == HAL_OK ? GOS_SUCCESS : GOS_ERROR
    );

    platformDriverInitResult &= driver_init();

	// Register kernel drivers.
	platformDriverInitResult &= gos_driverInit(&driverFunctions);

    GOS_CONVERT_RESULT(platformDriverInitResult);

    SysTick->VAL = 0;
    SysTick->CTRL = 0b111;

    __enable_irq();

    // Initialize bootloader.
    platformDriverInitResult &= trace_driverEnqueueTraceMessage("PDH board-specific initialization", app_pdhBdSpecInit());
    platformDriverInitResult &= trace_driverEnqueueTraceMessage("APP bootloader initialization", app_bld_init());

	return platformDriverInitResult;
}

/*
 * Function: svl_dsmApplicationInit
 */
gos_result_t svl_dsmApplicationInit (void_t)
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

	// Enable WiFi module.
	(void_t) drv_gpioWritePin(IO_WEMOS_RST, GPIO_STATE_HIGH);

	if (appInitResult != GOS_SUCCESS)
	{
		appInitResult = GOS_ERROR;
	}

	return appInitResult;
}


