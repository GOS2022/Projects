/*
 * bsp_rtc_handler.c
 *
 *  Created on: Jan 30, 2025
 *      Author: Ahmed
 */

/*
 * Includes
 */
#include "app.h"
#include "gos_lib.h"
#include <drv_rtc.h>

#define RTC_BUFFER_SIZE ( 128u )
/**
 * RTC buffer for sysmon messages.
 */
GOS_STATIC u8_t  rtcBuffer [RTC_BUFFER_SIZE];

/*
 * Function prototypes
 */
GOS_STATIC void_t bsp_rtcHandlerTask (void_t);
GOS_STATIC void_t bsp_rtcHandlerTimeSetReqCallback (void_t);

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

/**
 * TODO
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t bspRtcTimeSetMsgDesc =
{
	.callback        = bsp_rtcHandlerTimeSetReqCallback,
	.messageId       = APP_SYSMON_MSG_RTC_SET_REQ,
	.payload         = (void_t*)rtcBuffer,
	.payloadSize     = sizeof(gos_time_t),
	.protocolVersion = 1u
};

/*
 * Function: bsp_rtcHandlerInit
 */
gos_result_t bsp_rtcHandlerInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	initResult &= gos_sysmonRegisterUserMessage(&bspRtcTimeSetMsgDesc);
	initResult &= gos_taskRegister(&bspRtcTask, NULL);

	if (initResult != GOS_SUCCESS)
	{
		initResult = GOS_ERROR;
	}
	else
	{
		// Nothing to do.
	}

	return initResult;
}

// TODO
GOS_STATIC void_t bsp_rtcHandlerTask (void_t)
{
	/*
	 * Local variables.
	 */
	gos_time_t time;

	/*
	 * Function code.
	 */
	for (;;)
	{
		// Periodically update system time from RTC
		// to keep it synchronized.
		(void_t) drv_rtcTimeGet(&time, NULL);
		(void_t) gos_timeSet(&time);

		// This is done every 10 seconds.
		(void_t) gos_taskSleep(10000);
	}
}

GOS_STATIC void_t bsp_rtcHandlerTimeSetReqCallback (void_t)
{
	/*
	 * Local variables.
	 */
	gos_time_t desiredTime = {0};
	u8_t       result;

	/*
	 * Function code.
	 */
	(void_t) memcpy((void_t*)&desiredTime, (void_t*)rtcBuffer, sizeof(desiredTime));

	if (drv_rtcTimeSet(&desiredTime, 3) == GOS_SUCCESS)
	{
		result = 40;
	}
	else
	{
		result = 99;
	}

	(void_t) gos_gcpTransmitMessage(
    		CFG_SYSMON_GCP_CHANNEL_NUM,
			APP_SYSMON_MSG_RTC_SET_RESP,
			(void_t*)&result,
			sizeof(u8_t),
			0xFFFF);
}
