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

/**
 * Size of RTC buffer for sysmon messages.
 */
#define RTC_BUFFER_SIZE       ( 128u )

/**
 * Get time function index.
 */
#define RTC_GET_TIME_FUNC_IDX ( 0u )

/**
 * Get temperature function index.
 */
#define RTC_GET_TEMP_FUNC_IDX ( 1u )

/**
 * Set time function index.
 */
#define RTC_SET_TIME_FUNC_IDX ( 0u )

/**
 * RTC buffer for sysmon messages.
 */
GOS_STATIC u8_t  rtcBuffer [RTC_BUFFER_SIZE];

/**
 * RTC driver descriptor.
 */
GOS_STATIC drv_ds3231Descriptor_t rtcDesc =
{
	.i2cInstance     = DRV_I2C_INSTANCE_1,
	.deviceAddress   = 0x68,
	.readMutexTmo    = GOS_MUTEX_ENDLESS_TMO,
	.readTriggerTmo  = 300u,
	.writeMutexTmo   = GOS_MUTEX_ENDLESS_TMO,
	.writeTriggerTmo = 300u,
};

/*
 * Function prototypes
 */
GOS_STATIC void_t       bsp_rtcHandlerTask                  (void_t);
GOS_STATIC void_t       bsp_rtcHandlerTimeSetReqCallback    (void_t);
GOS_STATIC gos_result_t bsp_rtcHandlerGetTimeWrapper        (u8_t params, va_list args);
GOS_STATIC gos_result_t bsp_rtcHandlerGetTemperatureWrapper (u8_t params, va_list args);
GOS_STATIC gos_result_t bsp_rtcHandlerSetTimeWrapper        (u8_t params, va_list args);

/**
 * BSP RTC task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t bspRtcTask =
{
	.taskFunction	    = bsp_rtcHandlerTask,
	.taskName 		    = "bsp_rtc_handler",
	.taskStackSize 	    = 0x300,
	.taskPriority 	    = 10,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_USER
};

/**
 * Sysmon RTC time set message descriptor.
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t bspRtcTimeSetMsgDesc =
{
	.callback        = bsp_rtcHandlerTimeSetReqCallback,
	.messageId       = APP_SYSMON_MSG_RTC_SET_REQ,
	.payload         = (void_t*)rtcBuffer,
	.payloadSize     = sizeof(gos_time_t),
	.protocolVersion = 1u
};

/**
 * Standard device descriptor for RTC.
 */
GOS_STATIC svl_dhsDevice_t rtcDevice =
{
	.name                                  = "rtc",
	.description                           = "DS3231 RTC over I2C. Address: 0x68.",
	.pInitializer                          = drv_ds3231Init,
	.pDeviceDescriptor                     = (void_t*)&rtcDesc,
	.readFunctions[RTC_GET_TIME_FUNC_IDX]  = bsp_rtcHandlerGetTimeWrapper,
	.readFunctions[RTC_GET_TEMP_FUNC_IDX]  = bsp_rtcHandlerGetTemperatureWrapper,
	.writeFunctions[RTC_SET_TIME_FUNC_IDX] = bsp_rtcHandlerSetTimeWrapper,
	.enabled                               = GOS_TRUE,
	.errorTolerance                        = 100
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
	GOS_CONCAT_RESULT(initResult, gos_sysmonRegisterUserMessage(&bspRtcTimeSetMsgDesc));
	GOS_CONCAT_RESULT(initResult, gos_taskRegister(&bspRtcTask, NULL));
	GOS_CONCAT_RESULT(initResult, svl_dhsRegisterDevice(&rtcDevice));
	GOS_CONCAT_RESULT(initResult, svl_dhsForceInitialize(rtcDevice.deviceId));

	return initResult;
}

/*
 * Function: bsp_rtcHandlerGetTemperature
 */
gos_result_t bsp_rtcHandlerGetTemperature (u16_t* pTemp)
{
	/*
	 * Function code.
	 */
	return svl_dhsReadDevice(rtcDevice.deviceId, RTC_GET_TEMP_FUNC_IDX, 2, rtcDevice.pDeviceDescriptor, pTemp);
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
		if (svl_dhsReadDevice(rtcDevice.deviceId, RTC_GET_TIME_FUNC_IDX, 3, rtcDevice.pDeviceDescriptor, &time, NULL) == GOS_SUCCESS)
		{
			(void_t) gos_timeSet(&time);
		}

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

	if (svl_dhsWriteDevice(rtcDevice.deviceId, RTC_SET_TIME_FUNC_IDX, 3,
		rtcDevice.pDeviceDescriptor, &desiredTime, 3) == GOS_SUCCESS)
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

GOS_STATIC gos_result_t bsp_rtcHandlerGetTimeWrapper (u8_t params, va_list args)
{
	/*
	 * Local variables.
	 */
	void_t* pDevice;
	gos_time_t* pTime;
	u8_t* dow;

	/*
	 * Function code.
	 */
	pDevice = (void_t*)va_arg(args, int);
	pTime = (gos_time_t*)va_arg(args, int);
	dow = (u8_t*)va_arg(args, int);

	return drv_ds3231GetTime(pDevice, pTime, dow);
}

GOS_STATIC gos_result_t bsp_rtcHandlerGetTemperatureWrapper (u8_t params, va_list args)
{
	/*
	 * Local variables.
	 */
	void_t* pDevice;
	u16_t* pTemp;

	/*
	 * Function code.
	 */
	pDevice = (void_t*)va_arg(args, int);
	pTemp = (u16_t*)va_arg(args, int);

	return drv_ds3231GetTemperature(pDevice, pTemp);
}

GOS_STATIC gos_result_t bsp_rtcHandlerSetTimeWrapper (u8_t params, va_list args)
{
	/*
	 * Local variables.
	 */
	void_t* pDevice;
	gos_time_t* pTime;
	u8_t dow;

	/*
	 * Function code.
	 */
	pDevice = (void_t*)va_arg(args, int);
	pTime = (gos_time_t*)va_arg(args, int);
	dow = (u8_t)va_arg(args, int);

	return drv_ds3231SetTime(pDevice, pTime, dow);
}
