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

/*
 * Static function prototypes
 */
GOS_STATIC void_t app_ledTask (void_t);

/*
 * Static variables
 */
/**
 * LED task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t ledTaskDesc =
{
    .taskFunction       = app_ledTask,
    .taskStackSize      = 0x200,
    .taskPriority       = 0,
    .taskName           = "app_led_task",
    .taskPrivilegeLevel = GOS_TASK_PRIVILEGE_USER
};

/*
 * Function: app_ledInit
 */
gos_result_t app_ledInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t ledInitResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	ledInitResult &= gos_taskRegister(&ledTaskDesc, NULL);

	if (ledInitResult != GOS_SUCCESS)
	{
		ledInitResult = GOS_ERROR;
	}
	else
	{
		// Initialization OK.
	}

	return ledInitResult;
}

/**
 * TODO
 * @param
 * @return
 */
GOS_STATIC void_t app_ledTask (void_t)
{
	/*
	 * Local variables.
	 */
	bld_state_t bldState;

    /*
     * Function code.
     */
    for (;;)
    {
    	(void_t) bld_stateMachineGetState(&bldState);

    	switch (bldState)
    	{
    		case BLD_STATE_CONNECT_WAIT:
    		{
    	        drv_gpioTgglePin(IO_USER_LED);
    	        (void_t) gos_taskSleep(1000);
    			break;
    		}
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
    }
}
