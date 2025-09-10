/*
 * app_led.c
 *
 *  Created on: Sep 15, 2022
 *      Author: Gabor
 */

#include "app.h"
#include "drv_gpio.h"
#include "iodef.h"
#include "bld.h"
#include "drv_rtc.h"

/*
 * Static function prototypes
 */
GOS_STATIC void_t app_bldTask (void_t);

/*
 * Static variables
 */
/**
 * LED task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t bldTaskDesc =
{
    .taskFunction       = app_bldTask,
    .taskStackSize      = 0x400,
    .taskPriority       = 0,
    .taskName           = "app_led_task",
    .taskPrivilegeLevel = GOS_TASK_PRIVILEGE_USER
};

/*
 * Function: app_bldTaskInit
 */
gos_result_t app_bldTaskInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t bldTaskInitResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	bldTaskInitResult &= gos_taskRegister(&bldTaskDesc, NULL);

	if (bldTaskInitResult != GOS_SUCCESS)
	{
		bldTaskInitResult = GOS_ERROR;
	}
	else
	{
		// Initialization OK.
	}

	return bldTaskInitResult;
}

GOS_STATIC gos_time_t time;

/**
 * TODO
 * @param
 * @return
 */
GOS_STATIC void_t app_bldTask (void_t)
{
	/*
	 * Local variables.
	 */
	bld_state_t bldState;
	u32_t       sysTicks = gos_kernelGetSysTicks();

	// Initial setting.
	drv_rtcTimeGet(&time, NULL);
	gos_timeSet(&time);

    /*
     * Function code.
     */
    for (;;)
    {
    	(void_t) bld_stateMachineGetState(&bldState);

    	switch (bldState)
    	{
    		case BLD_STATE_WAIT:
    		{
    	        (void_t) drv_gpioTgglePin(IO_USER_LED);
    	        (void_t) gos_taskSleep(500);
    			break;
    		}
    		case BLD_STATE_INSTALL:
    		{
    	        (void_t) drv_gpioTgglePin(IO_USER_LED);
    	        (void_t) gos_taskSleep(250);
    	        break;
    		}
    		default:
    		{
    	        (void_t) drv_gpioTgglePin(IO_USER_LED);
    	        (void_t) gos_taskSleep(100);
    	        break;
    		}
    	}

    	if ((gos_kernelGetSysTicks() - sysTicks) > 1000)
    	{
    		sysTicks = gos_kernelGetSysTicks();
    		drv_rtcTimeGet(&time, NULL);
    		gos_timeSet(&time);
    	}
    }
}
