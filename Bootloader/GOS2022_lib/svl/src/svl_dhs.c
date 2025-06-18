//*************************************************************************************************
//
//                            #####             #####             #####
//                          #########         #########         #########
//                         ##                ##       ##       ##
//                        ##                ##         ##        #####
//                        ##     #####      ##         ##           #####
//                         ##       ##       ##       ##                ##
//                          #########         #########         #########
//                            #####             #####             #####
//
//                                      (c) Ahmed Gazar, 2024
//
//*************************************************************************************************
//! @file       svl_dhs.c
//! @author     Ahmed Gazar
//! @date       2024-04-13
//! @version    1.0
//!
//! @brief      GOS2022 Library / Device Handler Service source.
//! @details    For a more detailed description of this service, please refer to @ref svl_dhs.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-04-13    Ahmed Gazar     Initial version created.
//*************************************************************************************************
//
// Copyright (c) 2024 Ahmed Gazar
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//*************************************************************************************************
/*
 * Includes
 */
#include <string.h>
#include <svl_dhs.h>
#include <svl_sysmon.h>

/*
 * Macros
 */
/**
 * Device ID offset.
 */
#define DHS_DEV_ID_OFFSET ( 0x1000 )

/**
 * DHS binary buffer size.
 */
#define DHS_BUFFER_SIZE   ( sizeof(svl_dhsDevice_t) + 16u )

/*
 * Type definitions
 */
/**
 * DHS error types.
 */
typedef enum
{
	DHS_ERROR_NONE  = 0x0,  //!< No error.
	DHS_ERROR_INIT  = 0x1,  //!< Initialization error.
	DHS_ERROR_READ  = 0x2,  //!< Read error.
	DHS_ERROR_WRITE = 0x3   //!< Write error.
}svl_dhsDeviceError_t;

/**
 * DHS sysmon message IDs.
 */
typedef enum
{
    SVL_DHS_SYSMON_MSG_DEVICE_NUM_REQ        = 0x5001, //!< DHS sysmon message ID for device number request.
    SVL_DHS_SYSMON_MSG_DEVICE_NUM_RESP       = 0x5A01, //!< DHS sysmon message ID for device number response.
    SVL_DHS_SYSMON_MSG_DEVICE_INFO_REQ       = 0x5002, //!< DHS sysmon message ID for device info request.
    SVL_DHS_SYSMON_MSG_DEVICE_INFO_RESP      = 0x5A02, //!< DHS sysmon message ID for device info response.
}svl_dhsSysmonMsgId_t;

/*
 * Static variables
 */
/**
 * Internal device array.
 */
GOS_STATIC svl_dhsDevice_t devices [SVL_DHS_MAX_DEVICES];

/**
 * SDH buffer for sysmon messages.
 */
GOS_STATIC u8_t            dhsBuffer [DHS_BUFFER_SIZE];

/**
 * Device number counter.
 */
GOS_STATIC u8_t            numOfDevices = 0u;

/*
 * Function prototypes
 */
GOS_STATIC void_t svl_dhsDaemon (void_t);
GOS_STATIC void_t svl_dhsSysmonDeviceNumReqCallback  (gos_gcpChannelNumber_t gcpChannel);
GOS_STATIC void_t svl_dhsSysmonDeviceInfoReqCallback (gos_gcpChannelNumber_t gcpChannel);

/**
 * DHS daemon task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t dhsDaemonDesc =
{
    .taskFunction       = svl_dhsDaemon,
    .taskStackSize      = 0x400,
    .taskPriority       = SVL_DHS_DAEMON_PRIO,
    .taskName           = "svl_dhs_daemon",
    .taskPrivilegeLevel = GOS_TASK_PRIVILEGE_SUPERVISOR
};

/**
 * Sysmon device number request.
 */
GOS_STATIC svl_sysmonUserMessageDescriptor_t sysmonDeviceNumReqMsg =
{
    .callback        = svl_dhsSysmonDeviceNumReqCallback,
    .messageId       = SVL_DHS_SYSMON_MSG_DEVICE_NUM_REQ,
    .payload         = NULL,
    .payloadSize     = 0u,
};

/**
 * Sysmon device info request.
 */
GOS_STATIC svl_sysmonUserMessageDescriptor_t sysmonDeviceInfoReqMsg =
{
    .callback        = svl_dhsSysmonDeviceInfoReqCallback,
    .messageId       = SVL_DHS_SYSMON_MSG_DEVICE_INFO_REQ,
    .payload         = (void_t*)dhsBuffer,
    .payloadSize     = sizeof(u16_t),
};

/*
 * Function: svl_dhsInit
 */
gos_result_t svl_dhsInit (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t initResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    GOS_CONCAT_RESULT(initResult, gos_taskRegister(&dhsDaemonDesc, NULL));
    GOS_CONCAT_RESULT(initResult, svl_sysmonRegisterUserMessage(&sysmonDeviceNumReqMsg));
    GOS_CONCAT_RESULT(initResult, svl_sysmonRegisterUserMessage(&sysmonDeviceInfoReqMsg));

    return initResult;
}

/*
 * Function: svl_dhsRegisterDevice
 */
gos_result_t svl_dhsRegisterDevice (svl_dhsDevice_t* pDevice)
{
	/*
	 * Local variables.
	 */
	gos_result_t registerResult = GOS_ERROR;
	u8_t         index          = 0u;
	u8_t         index2         = 0u;
	u8_t         numOfWriteFunc = 0u;
	u8_t         numOfReadFunc  = 0u;

	/*
	 * Function code.
	 */
	if ((pDevice != NULL) && (pDevice->pDeviceDescriptor != NULL) && (pDevice->pInitializer != NULL))
	{
		for (index = 0u; index < SVL_DHS_MAX_DEVICES; index++)
		{
			if (devices[index].deviceId == 0u)
			{
				(void_t) memcpy((void_t*)&devices[index], (void_t*)pDevice, sizeof(svl_dhsDevice_t));
				pDevice->deviceId = (DHS_DEV_ID_OFFSET + index);
				devices[index].deviceState = DHS_STATE_UNINITIALIZED;
				devices[index].deviceId = (DHS_DEV_ID_OFFSET + index);
				devices[index].errorCode = 0u;
				devices[index].errorCounter = 0u;

				for (index2 = 0u; index2 < SVL_DHS_READ_FUNC_MAX_NUM; index2++)
				{
					if (devices[index].readFunctions[index2] != NULL)
					{
						numOfReadFunc++;
					}
					else
					{
						break;
					}
				}

				for (index2 = 0u; index2 < SVL_DHS_WRITE_FUNC_MAX_NUM; index2++)
				{
					if (devices[index].writeFunctions[index2] != NULL)
					{
						numOfWriteFunc++;
					}
					else
					{
						break;
					}
				}

				if ((numOfReadFunc > 0) && (numOfWriteFunc > 0))
				{
					devices[index].deviceType = DHS_TYPE_READWRITE;
				}
				else if (numOfReadFunc > 0)
				{
					devices[index].deviceType = DHS_TYPE_READONLY;
				}
				else if (numOfWriteFunc > 0)
				{
					devices[index].deviceType = DHS_TYPE_WRITEONLY;
				}
				else
				{
					devices[index].deviceType = DHS_TYPE_VIRTUAL;
				}

				registerResult = GOS_SUCCESS;
				numOfDevices++;
				break;
			}
			else
			{
				// Continue.
			}
		}
	}
	else
	{
		// NULL pointer error.
	}

	return registerResult;
}

/*
 * Function: svl_dhsUnregisterDevice
 */
gos_result_t svl_dhsUnregisterDevice (svl_dhsDevId_t devId)
{
	/*
	 * Local variables.
	 */
	gos_result_t unregisterResult = GOS_ERROR;
	u8_t         index            = (u8_t)(devId - DHS_DEV_ID_OFFSET);

	/*
	 * Function code.
	 */
	if ((index < numOfDevices) && (devices[index].pDeviceDescriptor != NULL))
	{
		(void_t) memset((void_t*)&devices[index], 0, sizeof(svl_dhsDevice_t));
		unregisterResult = GOS_SUCCESS;
		numOfDevices--;
	}
	else
	{
		// Device does not exist.
	}

	return unregisterResult;
}

/*
 * Function: svl_dhsForceInitialize
 */
gos_result_t svl_dhsForceInitialize (svl_dhsDevId_t devId)
{
	/*
	 * Local variables.
	 */
	gos_result_t forceInitResult = GOS_ERROR;
	u8_t         index           = (u8_t)(devId - DHS_DEV_ID_OFFSET);

	/*
	 * Function code.
	 */
	if ((index < numOfDevices) && (devices[index].pDeviceDescriptor != NULL) &&
		(devices[index].pInitializer != NULL))
	{
		if (devices[index].pInitializer(devices[index].pDeviceDescriptor) != GOS_SUCCESS)
		{
			// Device initialization failed.
			devices[index].errorCounter++;
			devices[index].errorCode |= DHS_ERROR_INIT;
		}
		else
		{
			// Device initialized successfully.
			devices[index].deviceState = DHS_STATE_HEALTHY;
			forceInitResult = GOS_SUCCESS;
		}
	}
	else
	{
		// Device does not exist.
	}

	return forceInitResult;
}

/*
 * Function: svl_dhsWriteDevice
 */
gos_result_t svl_dhsWriteDevice (svl_dhsDevId_t devId, u8_t functionIdx, u8_t length, ...)
{
	/*
	 * Local variables.
	 */
	va_list      args;
	gos_result_t writeResult = GOS_ERROR;
	u8_t         index       = (u8_t)(devId - DHS_DEV_ID_OFFSET);

	/*
	 * Function code.
	 */
	if ((index < SVL_DHS_MAX_DEVICES) && (devices[index].writeFunctions[functionIdx] != NULL) &&
		(devices[index].deviceState != DHS_STATE_UNINITIALIZED) && (devices[index].enabled == GOS_TRUE) &&
		(devices[index].errorCounter <= devices[index].errorTolerance))
	{
		va_start(args, length);

		if (devices[index].writeFunctions[functionIdx](length, args) == GOS_ERROR)
		{
			devices[index].errorCode   |= DHS_ERROR_WRITE;
			devices[index].deviceState = DHS_STATE_ERROR;
			devices[index].errorCounter++;
		}
		else
		{
			devices[index].writeCounter++;
			writeResult = GOS_SUCCESS;
		}

		va_end(args);
	}
	else
	{
		// Wrong request.
	}

	return writeResult;
}

/*
 * Function: svl_dhsReadDevice
 */
gos_result_t svl_dhsReadDevice (svl_dhsDevId_t devId, u8_t functionIdx, u8_t length, ...)
{
	/*
	 * Local variables.
	 */
	va_list      args;
	gos_result_t readResult = GOS_ERROR;
	u8_t         index      = (u8_t)(devId - DHS_DEV_ID_OFFSET);

	/*
	 * Function code.
	 */
	if ((index < SVL_DHS_MAX_DEVICES) && (devices[index].readFunctions[functionIdx] != NULL) &&
		(devices[index].deviceState != DHS_STATE_UNINITIALIZED) && (devices[index].enabled == GOS_TRUE) &&
		(devices[index].errorCounter <= devices[index].errorTolerance))
	{
		va_start(args, length);

		if (devices[index].readFunctions[functionIdx](length, args) == GOS_ERROR)
		{
			devices[index].errorCode   |= DHS_ERROR_READ;
			devices[index].deviceState = DHS_STATE_ERROR;
			devices[index].errorCounter++;
		}
		else
		{
			devices[index].readCounter++;
			readResult = GOS_SUCCESS;
		}

		va_end(args);
	}
	else
	{
		// Wrong request.
	}

	return readResult;
}

/*
 * Function: svl_dhsGetNumOfDevices
 */
gos_result_t svl_dhsGetNumOfDevices (u8_t* pNumber)
{
	/*
	 * Local variables.
	 */
	gos_result_t getResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if (pNumber != NULL)
	{
		*pNumber = numOfDevices;
		getResult = GOS_SUCCESS;
	}
	else
	{
		// NULL pointer error.
	}

	return getResult;
}

/*
 * Function: svl_dhsGetIndex
 */
gos_result_t svl_dhsGetIndex (svl_dhsDevId_t deviceId, u8_t* pIndex)
{
	/*
	 * Local variables.
	 */
	gos_result_t getResult = GOS_ERROR;
	u8_t         tempIndex = (u8_t)(deviceId - DHS_DEV_ID_OFFSET);

	/*
	 * Function code.
	 */
	if ((pIndex != NULL) && (tempIndex < numOfDevices))
	{
		*pIndex = tempIndex;
	}
	else
	{
		// NULL pointer or wrong index.
	}

	return getResult;
}

/*
 * Function: svl_dhsGetDeviceData
 */
gos_result_t svl_dhsGetDeviceData (u8_t index, svl_dhsDevice_t* pDevice)
{
	/*
	 * Local variables.
	 */
	gos_result_t getResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pDevice != NULL) && (index < numOfDevices))
	{
		(void_t) memcpy((void_t*)pDevice, (void_t*)&devices[index], sizeof(svl_dhsDevice_t));
		getResult = GOS_SUCCESS;
	}
	else
	{
		// NULL pointer or wrong index.
	}

	return getResult;
}

/*
 * Function svl_dhsEnableDevice
 */
gos_result_t svl_dhsEnableDevice (svl_dhsDevId_t devId)
{
	/*
	 * Local variables.
	 */
	gos_result_t enableResult = GOS_ERROR;
	u8_t         index       = (u8_t)(devId - DHS_DEV_ID_OFFSET);

	/*
	 * Function code.
	 */
	if ((index < numOfDevices) && devices[index].enabled != GOS_TRUE)
	{
		devices[index].enabled = GOS_TRUE;
		enableResult = GOS_SUCCESS;
	}
	else
	{
		// Wrong ID or device already enabled.
	}

	return enableResult;
}

/*
 * Function svl_dhsDisableDevice
 */
gos_result_t svl_dhsDisableDevice (svl_dhsDevId_t devId)
{
	/*
	 * Local variables.
	 */
	gos_result_t disableResult = GOS_ERROR;
	u8_t         index       = (u8_t)(devId - DHS_DEV_ID_OFFSET);

	/*
	 * Function code.
	 */
	if ((index < numOfDevices) && devices[index].enabled != GOS_FALSE)
	{
		devices[index].enabled = GOS_FALSE;
		disableResult = GOS_SUCCESS;
	}
	else
	{
		// Wrong ID or device already disabled.
	}

	return disableResult;
}

/**
 * @brief   DHS daemon task.
 * @details Initializes the registered devices that have not been force-initialized.

 * @return  -
 */
GOS_STATIC void_t svl_dhsDaemon (void_t)
{
	/*
	 * Local variables.
	 */
	u8_t index = 0u;

	/*
	 * Function code.
	 */
	// Initialize devices.
	for (index = 0u; index < SVL_DHS_MAX_DEVICES; index++)
	{
		if ((devices[index].pInitializer != NULL) && (devices[index].deviceState == DHS_STATE_UNINITIALIZED))
		{
			if (devices[index].pInitializer(devices[index].pDeviceDescriptor) != GOS_SUCCESS)
			{
				// Device initialization failed.
				devices[index].errorCounter++;
				devices[index].errorCode |= DHS_ERROR_INIT;
			}
			else
			{
				// Device initialized successfully.
				devices[index].deviceState = DHS_STATE_HEALTHY;
			}
		}
		else
		{
			// Continue.
		}
	}

	for (;;)
	{
		gos_taskSleep(10000);
	}
}

/**
 * @brief   Sysmon device number request callback.
 * @details Sends out the number of registered devices.
 *
 * @return  -
 */
GOS_STATIC void_t svl_dhsSysmonDeviceNumReqCallback  (gos_gcpChannelNumber_t gcpChannel)
{
	/*
	 * Local variables.
	 */
	u16_t numOfDev = (u16_t)numOfDevices;

	/*
	 * Function code.
	 */
	(void_t) memcpy((void_t*)dhsBuffer, (void_t*)&numOfDev, sizeof(u16_t));
    (void_t) gos_gcpTransmitMessage(
    		gcpChannel,
            SVL_DHS_SYSMON_MSG_DEVICE_NUM_RESP,
            (void_t*)dhsBuffer,
            sizeof(u16_t),
            0xFFFF);
}

/**
 * @brief   Sysmon device info request callback.
 * @details Sends out the device information of the requested device.
 *
 * @return  -
 */
GOS_STATIC void_t svl_dhsSysmonDeviceInfoReqCallback (gos_gcpChannelNumber_t gcpChannel)
{
	/*
	 * Local variables.
	 */
	u16_t devIndex;
	svl_dhsDevice_t deviceData;

	/*
	 * Function code.
	 */
	(void_t) memcpy((void_t*)&devIndex, (void_t*)dhsBuffer, sizeof(u16_t));
	(void_t) svl_dhsGetDeviceData(devIndex, &deviceData);
	(void_t) memcpy((void_t*)dhsBuffer, (void_t*)&deviceData, sizeof(svl_dhsDevice_t));

    (void_t) gos_gcpTransmitMessage(
    		gcpChannel,
            SVL_DHS_SYSMON_MSG_DEVICE_INFO_RESP,
            (void_t*)dhsBuffer,
            sizeof(svl_dhsDevice_t),
            0xFFFF);
}
