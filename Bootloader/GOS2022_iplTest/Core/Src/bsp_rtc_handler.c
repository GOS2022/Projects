/*
 * bsp_rtc_handler.c
 *
 *  Created on: Jan 30, 2025
 *      Author: Ahmed
 */

/*
 * Includes
 */
#include <drv_rtc.h>
#include "app.h"
#include "mdi_def.h"
#include "gos_lib.h"

/*
 * Extern variables
 */
GOS_EXTERN svl_mdiVariable_t mdiVariables [];

/*
 * Function prototypes
 */
GOS_STATIC void_t bsp_rtcHandlerTask (void_t);

/**
 * BSP RTC task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t bspRtcTask =
{
	.taskFunction	    = bsp_rtcHandlerTask,
	.taskName 		    = "bsp_rtc_handler",
	.taskStackSize 	    = 0x300,
	.taskPriority 	    = 20,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_USER
};

/*
 * Function: bsp_rtcHandlerInit
 */
gos_result_t bsp_rtcHandlerInit (void_t)
{
	/*
	 * Function code.
	 */
	return gos_taskRegister(&bspRtcTask, NULL);
}

// TODO
GOS_STATIC void_t bsp_rtcHandlerTask (void_t)
{
	/*
	 * Local variables.
	 */
	gos_time_t time;
	u8_t       dow;
	u16_t      temp;

	/*
	 * Function code.
	 */
	// Initialize system time from RTC.
	(void_t) drv_rtcTimeGet(&time, &dow);
	(void_t) gos_timeSet(&time);

	for (;;)
	{
		// Periodically update RTC in case system time
		// is synchronized from sysmon.
		(void_t) gos_timeGet(&time);
		(void_t) drv_rtcTimeSet(&time, dow);

		// Update RTC temperature in MDI.
		drv_rtcTempGet(&temp);
		mdiVariables[MDI_RTC_TEMP].value.floatValue = (float_t)(temp / 100.0f);

		(void_t) gos_taskSleep(1000);
	}
}
