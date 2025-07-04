/*
 * app_dhsTest.c
 *
 *  Created on: Apr 8, 2025
 *      Author: Ahmed
 */
#include "app.h"

typedef struct
{
	u8_t dummy;
}app_virtualDeviceDesc_t;

typedef enum
{
	WRITE_FUNCTION_TYPE_1 = 0,
	WRITE_FUNCTION_TYPE_2 = 1,
	READ_FUNCTION_TYPE_1 = 0
}app_writeFunc_t;

GOS_STATIC app_virtualDeviceDesc_t virtualDeviceDesc = { .dummy = 10 };

GOS_STATIC void_t       app_dhsTestTask       (void_t);
GOS_STATIC gos_result_t app_dhsTestInitDevice (void_t* pDevice);
GOS_STATIC gos_result_t app_dhsWriteFunction1 (u8_t params, va_list args);
GOS_STATIC gos_result_t app_dhsWriteFunction2 (u8_t params, va_list args);
GOS_STATIC gos_result_t app_dhsReadFunction (u8_t params, va_list args);
GOS_STATIC gos_result_t app_dhsDriverFunc1 (u8_t param1, u8_t param2);
GOS_STATIC gos_result_t app_dhsDriverFunc2 (char_t param1, u8_t param2, bool_t param3);
GOS_STATIC gos_result_t app_dhsDriverFunc3 (char_t* param1, u8_t* param2);

GOS_STATIC gos_taskDescriptor_t app_dhsTestTaskDesc =
{
	.taskFunction	    = app_dhsTestTask,
	.taskName 		    = "app_dhsTestTask",
	.taskStackSize 	    = 0x400,
	.taskPriority 	    = 30,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_USER
};

svl_dhsDevice_t testVirtualDevice =
{
	.name                                  = "virtual_device",
	.description                           = "Test virtual device.",
	.pInitializer                          = app_dhsTestInitDevice,
	.pDeviceDescriptor                     = (void_t*)&virtualDeviceDesc,
	.writeFunctions[WRITE_FUNCTION_TYPE_1] = app_dhsWriteFunction1,
	.writeFunctions[WRITE_FUNCTION_TYPE_2] = app_dhsWriteFunction2,
	.readFunctions[READ_FUNCTION_TYPE_1]   = app_dhsReadFunction,
	.enabled                               = GOS_TRUE,
	.errorTolerance                        = 100
};

gos_result_t app_dhsTestInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t dhsTestInitRes = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(dhsTestInitRes, gos_taskRegister(&app_dhsTestTaskDesc, NULL));
	GOS_CONCAT_RESULT(dhsTestInitRes, svl_dhsRegisterDevice(&testVirtualDevice));

	return dhsTestInitRes;
}

GOS_STATIC void_t app_dhsTestTask (void_t)
{
	char_t readChar;
	u8_t readNum;
	for (;;)
	{
		svl_dhsWriteDevice(testVirtualDevice.deviceId, WRITE_FUNCTION_TYPE_1, 2, 15, 16);
		svl_dhsWriteDevice(testVirtualDevice.deviceId, WRITE_FUNCTION_TYPE_2, 3, 'x', 12, GOS_TRUE);
		svl_dhsReadDevice(testVirtualDevice.deviceId, READ_FUNCTION_TYPE_1, 2, &readChar, &readNum);
		gos_taskSleep(1000);
	}
}

GOS_STATIC gos_result_t app_dhsTestInitDevice (void_t* pDevice)
{
	return GOS_SUCCESS;
}

GOS_STATIC gos_result_t app_dhsWriteFunction1 (u8_t params, va_list args)
{
	u8_t param1, param2;

	param1 = (u8_t)va_arg(args, int);
	param2 = (u8_t)va_arg(args, int);

	return app_dhsDriverFunc1(param1, param2);
}

GOS_STATIC gos_result_t app_dhsWriteFunction2 (u8_t params, va_list args)
{
	char_t param1;
	u8_t param2;
	bool_t param3;

	param1 = (char_t)va_arg(args, int);
	param2 = (u8_t)va_arg(args, int);
	param3 = (bool_t)va_arg(args, int);

	return app_dhsDriverFunc2(param1, param2, param3);
}

GOS_STATIC gos_result_t app_dhsReadFunction (u8_t params, va_list args)
{
	char_t* param1;
	u8_t* param2;

	param1 = (char_t*)va_arg(args, int);
	param2 = (u8_t*)va_arg(args, int);

	return app_dhsDriverFunc3(param1, param2);
}

GOS_STATIC gos_result_t app_dhsDriverFunc1 (u8_t param1, u8_t param2)
{
	return GOS_SUCCESS;
}

GOS_STATIC gos_result_t app_dhsDriverFunc2 (char_t param1, u8_t param2, bool_t param3)
{
	return GOS_ERROR;
}

GOS_STATIC gos_result_t app_dhsDriverFunc3 (char_t* param1, u8_t* param2)
{
	*param1 = 'y';
	*param2 = 36;
	return GOS_SUCCESS;
}
