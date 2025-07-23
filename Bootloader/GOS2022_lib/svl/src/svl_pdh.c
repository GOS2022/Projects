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
//                                      (c) Ahmed Gazar, 2025
//
//*************************************************************************************************
//! @file       svl_pdh.c
//! @author     Ahmed Gazar
//! @date       2025-01-30
//! @version    1.0
//!
//! @brief      GOS2022 Library / Project Data Handler source.
//! @details    For a more detailed description of this service, please refer to @ref svl_pdh.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-01-30    Ahmed Gazar     Initial version created.
//*************************************************************************************************
//
// Copyright (c) 2025 Ahmed Gazar
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
#include <svl_pdh.h>
#include <string.h>
#include <svl_sysmon.h>

/*
 * Macros
 */
/**
 * PDH buffer size.
 */
#define PDH_BUFFER_SIZE ( 1024 )

/*
 * Type definitions
 */
typedef enum
{
    SVL_PDH_SYSMON_MSG_SOFTWARE_INFO_GET_REQ  = 0x2001,
    SVL_PDH_SYSMON_MSG_SOFTWARE_INFO_GET_RESP = 0x2A01,
    SVL_PDH_SYSMON_MSG_HARDWARE_INFO_GET_REQ  = 0x2002,
    SVL_PDH_SYSMON_MSG_HARDWARE_INFO_GET_RESP = 0x2A02,
    SVL_PDH_SYSMON_MSG_WIFI_CONFIG_GET_REQ    = 0x2003,
    SVL_PDH_SYSMON_MSG_WIFI_CONFIG_GET_RESP   = 0x2A03,
    SVL_PDH_SYSMON_MSG_BLD_CONFIG_GET_REQ     = 0x2004,
    SVL_PDH_SYSMON_MSG_BLD_CONFIG_GET_RESP    = 0x2A04,
    SVL_PDH_SYSMON_MSG_SOFTWARE_INFO_SET_REQ  = 0x2005,
    SVL_PDH_SYSMON_MSG_SOFTWARE_INFO_SET_RESP = 0x2A05,
    SVL_PDH_SYSMON_MSG_HARDWARE_INFO_SET_REQ  = 0x2006,
    SVL_PDH_SYSMON_MSG_HARDWARE_INFO_SET_RESP = 0x2A06,
    SVL_PDH_SYSMON_MSG_WIFI_CONFIG_SET_REQ    = 0x2007,
    SVL_PDH_SYSMON_MSG_WIFI_CONFIG_SET_RESP   = 0x2A07,
    SVL_PDH_SYSMON_MSG_BLD_CONFIG_SET_REQ     = 0x2008,
    SVL_PDH_SYSMON_MSG_BLD_CONFIG_SET_RESP    = 0x2A08
}svl_pdhSysmonMsgId_t;

/*
 * Static variables
 */
/**
 * Read function pointer.
 */
GOS_STATIC svl_pdhReadWriteFunc_t pdhReadFunction = NULL;

/**
 * Write function pointer.
 */
GOS_STATIC svl_pdhReadWriteFunc_t pdhWriteFunction = NULL;

/**
 * PDH mutex.
 */
GOS_STATIC gos_mutex_t            pdhMutex;

/**
 * Initialized flag.
 */
GOS_STATIC bool_t                 pdhInited        = GOS_FALSE;

/**
 * PDH buffer for sysmon messages.
 */
GOS_STATIC u8_t                   pdhBuffer [PDH_BUFFER_SIZE];

/**
 * PDH software info message.
 */
GOS_STATIC svl_pdhSwInfo_t        swInfoMsg        = {0};

/**
 * PDH hardware info message.
 */
GOS_STATIC svl_pdhHwInfo_t        hwInfoMsg        = {0};

/**
 * PDH WiFi configuration message.
 */
GOS_STATIC svl_pdhWifiCfg_t       wifiCfgMsg       = {0};

/**
 * PDH bootloader configuration message.
 */
GOS_STATIC svl_pdhBldCfg_t        bldCfgMsg        = {0};

/*
 * External variables
 */
GOS_EXTERN GOS_CONST svl_sysmonServiceConfig_t sysmonConfig;

/*
 * Function prototypes
 */
GOS_STATIC void_t svl_pdhSoftwareInfoReqMsgReceived (gos_gcpChannelNumber_t gcpChannel);
GOS_STATIC void_t svl_pdhHardwareInfoReqMsgReceived (gos_gcpChannelNumber_t gcpChannel);
GOS_STATIC void_t svl_pdhWifiCfgReqMsgReceived      (gos_gcpChannelNumber_t gcpChannel);
GOS_STATIC void_t svl_pdhBldCfgReqMsgReceived       (gos_gcpChannelNumber_t gcpChannel);
GOS_STATIC void_t svl_pdhSoftwareInfoSetMsgReceived (gos_gcpChannelNumber_t gcpChannel);
GOS_STATIC void_t svl_pdhHardwareInfoSetMsgReceived (gos_gcpChannelNumber_t gcpChannel);
GOS_STATIC void_t svl_pdhWifiCfgSetMsgReceived      (gos_gcpChannelNumber_t gcpChannel);
GOS_STATIC void_t svl_pdhBldCfgSetMsgReceived       (gos_gcpChannelNumber_t gcpChannel);

/**
 * System monitoring software information request message.
 */
GOS_STATIC svl_sysmonUserMessageDescriptor_t softwareInfoReqMsg =
{
	.callback        = svl_pdhSoftwareInfoReqMsgReceived,
	.messageId       = SVL_PDH_SYSMON_MSG_SOFTWARE_INFO_GET_REQ,
	.payload         = NULL,
	.payloadSize     = 0u,
};

/**
 * System monitoring hardware information request message.
 */
GOS_STATIC svl_sysmonUserMessageDescriptor_t hardwareInfoReqMsg =
{
	.callback        = svl_pdhHardwareInfoReqMsgReceived,
	.messageId       = SVL_PDH_SYSMON_MSG_HARDWARE_INFO_GET_REQ,
	.payload         = NULL,
	.payloadSize     = 0u,
};

/**
 * WiFi configuration information request message.
 */
GOS_STATIC svl_sysmonUserMessageDescriptor_t wifiCfgReqMsg =
{
	.callback        = svl_pdhWifiCfgReqMsgReceived,
	.messageId       = SVL_PDH_SYSMON_MSG_WIFI_CONFIG_GET_REQ,
	.payload         = NULL,
	.payloadSize     = 0u,
};

/**
 * Bootloader configuration information request message.
 */
GOS_STATIC svl_sysmonUserMessageDescriptor_t bldCfgReqMsg =
{
	.callback        = svl_pdhBldCfgReqMsgReceived,
	.messageId       = SVL_PDH_SYSMON_MSG_BLD_CONFIG_GET_REQ,
	.payload         = NULL,
	.payloadSize     = 0u,
};

/**
 * System monitoring software information set message.
 */
GOS_STATIC svl_sysmonUserMessageDescriptor_t softwareInfoSetMsg =
{
	.callback        = svl_pdhSoftwareInfoSetMsgReceived,
	.messageId       = SVL_PDH_SYSMON_MSG_SOFTWARE_INFO_SET_REQ,
	.payload         = (void_t*)pdhBuffer,
	.payloadSize     = sizeof(svl_pdhSwInfo_t),
};

/**
 * System monitoring hardware information set message.
 */
GOS_STATIC svl_sysmonUserMessageDescriptor_t hardwareInfoSetMsg =
{
	.callback        = svl_pdhHardwareInfoSetMsgReceived,
	.messageId       = SVL_PDH_SYSMON_MSG_HARDWARE_INFO_SET_REQ,
	.payload         = (void_t*)pdhBuffer,
	.payloadSize     = sizeof(svl_pdhHwInfo_t),
};

/**
 * System monitoring WiFi configuration set message.
 */
GOS_STATIC svl_sysmonUserMessageDescriptor_t wifiCfgSetMsg =
{
	.callback        = svl_pdhWifiCfgSetMsgReceived,
	.messageId       = SVL_PDH_SYSMON_MSG_WIFI_CONFIG_SET_REQ,
	.payload         = (void_t*)pdhBuffer,
	.payloadSize     = sizeof(svl_pdhWifiCfg_t),
};

/**
 * System monitoring bootloader configuration set message.
 */
GOS_STATIC svl_sysmonUserMessageDescriptor_t bldCfgSetMsg =
{
	.callback        = svl_pdhBldCfgSetMsgReceived,
	.messageId       = SVL_PDH_SYSMON_MSG_BLD_CONFIG_SET_REQ,
	.payload         = (void_t*)pdhBuffer,
	.payloadSize     = sizeof(svl_pdhBldCfg_t),
};

/*
 * Function: svl_pdhInit
 */
gos_result_t svl_pdhInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
    GOS_CONCAT_RESULT(initResult, svl_sysmonRegisterUserMessage(&softwareInfoReqMsg));
    GOS_CONCAT_RESULT(initResult, svl_sysmonRegisterUserMessage(&hardwareInfoReqMsg));
    GOS_CONCAT_RESULT(initResult, svl_sysmonRegisterUserMessage(&wifiCfgReqMsg));
    GOS_CONCAT_RESULT(initResult, svl_sysmonRegisterUserMessage(&bldCfgReqMsg));
    GOS_CONCAT_RESULT(initResult, svl_sysmonRegisterUserMessage(&softwareInfoSetMsg));
    GOS_CONCAT_RESULT(initResult, svl_sysmonRegisterUserMessage(&hardwareInfoSetMsg));
    GOS_CONCAT_RESULT(initResult, svl_sysmonRegisterUserMessage(&wifiCfgSetMsg));
    GOS_CONCAT_RESULT(initResult, svl_sysmonRegisterUserMessage(&bldCfgSetMsg));

    GOS_CONCAT_RESULT(initResult, gos_mutexInit(&pdhMutex));

	pdhInited = GOS_TRUE;

	return initResult;
}

/*
 * Function: svl_pdhConfigure
 */
gos_result_t svl_pdhConfigure (svl_pdhCfg_t* pCfg)
{
	/*
	 * Local variables.
	 */
	gos_result_t cfgResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if (pCfg != NULL && pCfg->readFunction != NULL && pCfg->writeFunction != NULL)
	{
		pdhReadFunction  = pCfg->readFunction;
		pdhWriteFunction = pCfg->writeFunction;
		cfgResult        = GOS_SUCCESS;
	}
	else
	{
		// Error.
	}

	return cfgResult;
}

/*
 * Function: svl_pdhGetLibVersion
 */
gos_result_t svl_pdhGetLibVersion (svl_pdhSwVerInfo_t* pLibVer)
{
	/*
	 * Local variables.
	 */
	gos_result_t getResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if (pLibVer != NULL && pdhReadFunction != NULL)
	{
		(void_t) memset((void_t*)pLibVer, 0, sizeof(*pLibVer));

		pLibVer->major             = PDH_LIBVER_VERSION_MAJOR;
		pLibVer->minor             = PDH_LIBVER_VERSION_MINOR;
		pLibVer->build             = PDH_LIBVER_VERSION_BUILD;
		pLibVer->date.years        = PDH_LIBVER_VERSION_DATE_YEAR;
		pLibVer->date.months       = PDH_LIBVER_VERSION_DATE_MONTH;
		pLibVer->date.days         = PDH_LIBVER_VERSION_DATE_DAY;
		pLibVer->date.hours        = 0u;
		pLibVer->date.minutes      = 0u;
		pLibVer->date.seconds      = 0u;
		pLibVer->date.milliseconds = 0u;

		(void_t) strcpy(pLibVer->name,        PDH_LIBVER_NAME);
		(void_t) strcpy(pLibVer->author,      PDH_LIBVER_AUTHOR);
		(void_t) strcpy(pLibVer->description, PDH_LIBVER_DESCRIPTION);

		getResult = GOS_SUCCESS;
	}
	else
	{
		getResult = GOS_ERROR;
	}

	return getResult;
}

/*
 * Function: svl_pdhGetSwInfo
 */
GOS_INLINE gos_result_t svl_pdhGetSwInfo (svl_pdhSwInfo_t* pSwInfo)
{
	/*
	 * Local variables.
	 */
	gos_result_t getResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pSwInfo != NULL) && (pdhReadFunction != NULL) &&
		((pdhInited == GOS_FALSE) || (pdhInited == GOS_TRUE && gos_mutexLock(&pdhMutex, 5000) == GOS_SUCCESS)))
	{
		getResult = pdhReadFunction(PDH_ADDR_SW_INFO, (u8_t*)pSwInfo, sizeof(*pSwInfo));
		(void_t) gos_mutexUnlock(&pdhMutex);
	}
	else
	{
		getResult = GOS_ERROR;
	}

	return getResult;
}

/*
 * Function: svl_pdhGetHwInfo
 */
GOS_INLINE gos_result_t svl_pdhGetHwInfo (svl_pdhHwInfo_t* pHwInfo)
{
	/*
	 * Local variables.
	 */
	gos_result_t getResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pHwInfo != NULL) && (pdhReadFunction != NULL) &&
		((pdhInited == GOS_FALSE) || (pdhInited == GOS_TRUE && gos_mutexLock(&pdhMutex, 5000) == GOS_SUCCESS)))
	{
		getResult = pdhReadFunction(PDH_ADDR_HW_INFO, (u8_t*)pHwInfo, sizeof(*pHwInfo));
		(void_t) gos_mutexUnlock(&pdhMutex);
	}
	else
	{
		getResult = GOS_ERROR;
	}

	return getResult;
}

/*
 * Function: svl_pdhGetBldCfg
 */
GOS_INLINE gos_result_t svl_pdhGetBldCfg (svl_pdhBldCfg_t* pBldCfg)
{
	/*
	 * Local variables.
	 */
	gos_result_t getResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pBldCfg != NULL) && (pdhReadFunction != NULL) &&
	   ((pdhInited == GOS_FALSE) || (pdhInited == GOS_TRUE && gos_mutexLock(&pdhMutex, 5000) == GOS_SUCCESS)))
	{
		getResult = pdhReadFunction(PDH_ADDR_BLD_CFG, (u8_t*)pBldCfg, sizeof(*pBldCfg));
		(void_t) gos_mutexUnlock(&pdhMutex);
	}
	else
	{
		getResult = GOS_ERROR;
	}

	return getResult;
}

/*
 * Function: svl_pdhGetWifiCfg
 */
gos_result_t svl_pdhGetWifiCfg (svl_pdhWifiCfg_t* pWifiCfg)
{
	/*
	 * Local variables.
	 */
	gos_result_t getResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pWifiCfg != NULL) && (pdhReadFunction != NULL) &&
		((pdhInited == GOS_FALSE) || (pdhInited == GOS_TRUE && gos_mutexLock(&pdhMutex, 5000) == GOS_SUCCESS)))
	{
		getResult = pdhReadFunction(PDH_ADDR_WIFI_CFG, (u8_t*)pWifiCfg, sizeof(*pWifiCfg));
		(void_t) gos_mutexUnlock(&pdhMutex);
	}
	else
	{
		getResult = GOS_ERROR;
	}

	return getResult;
}

/*
 * Function: svl_pdhSetSwInfo
 */
GOS_INLINE gos_result_t svl_pdhSetSwInfo (svl_pdhSwInfo_t* pSwInfo)
{
	/*
	 * Local variables.
	 */
	gos_result_t setResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pSwInfo != NULL) && (pdhWriteFunction != NULL) &&
		((pdhInited == GOS_FALSE) || (pdhInited == GOS_TRUE && gos_mutexLock(&pdhMutex, 5000) == GOS_SUCCESS)))
	{
		setResult = pdhWriteFunction(PDH_ADDR_SW_INFO, (u8_t*)pSwInfo, sizeof(svl_pdhSwInfo_t));
		(void_t) gos_mutexUnlock(&pdhMutex);
	}
	else
	{
		setResult = GOS_ERROR;
	}

	return setResult;
}

/*
 * Function: svl_pdhSetHwInfo
 */
gos_result_t svl_pdhSetHwInfo (svl_pdhHwInfo_t* pHwInfo)
{
	/*
	 * Local variables.
	 */
	gos_result_t setResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pHwInfo != NULL) && (pdhWriteFunction != NULL) &&
		((pdhInited == GOS_FALSE) || (pdhInited == GOS_TRUE && gos_mutexLock(&pdhMutex, 5000) == GOS_SUCCESS)))
	{
		setResult = pdhWriteFunction(PDH_ADDR_HW_INFO, (u8_t*)pHwInfo, sizeof(*pHwInfo));
		(void_t) gos_mutexUnlock(&pdhMutex);
	}
	else
	{
		setResult = GOS_ERROR;
	}

	return setResult;
}

/*
 * Function: svl_pdhSetBldCfg
 */
GOS_INLINE gos_result_t svl_pdhSetBldCfg (svl_pdhBldCfg_t* pBldCfg)
{
	/*
	 * Local variables.
	 */
	gos_result_t setResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pBldCfg != NULL) && (pdhWriteFunction != NULL) &&
       ((pdhInited == GOS_FALSE) || (pdhInited == GOS_TRUE && gos_mutexLock(&pdhMutex, 5000) == GOS_SUCCESS)))
	{
		setResult = pdhWriteFunction(PDH_ADDR_BLD_CFG, (u8_t*)pBldCfg, sizeof(*pBldCfg));
		(void_t) gos_mutexUnlock(&pdhMutex);
	}
	else
	{
		setResult = GOS_ERROR;
	}

	return setResult;
}

/*
 * Function: svl_pdhSetWifiCfg
 */
gos_result_t svl_pdhSetWifiCfg (svl_pdhWifiCfg_t* pWifiCfg)
{
	/*
	 * Local variables.
	 */
	gos_result_t setResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pWifiCfg != NULL) && (pdhWriteFunction != NULL) &&
		((pdhInited == GOS_FALSE) || (pdhInited == GOS_TRUE && gos_mutexLock(&pdhMutex, 5000) == GOS_SUCCESS)))
	{
		setResult = pdhWriteFunction(PDH_ADDR_WIFI_CFG, (u8_t*)pWifiCfg, sizeof(*pWifiCfg));
		(void_t) gos_mutexUnlock(&pdhMutex);
	}
	else
	{
		setResult = GOS_ERROR;
	}

	return setResult;
}

/**
 * @brief   Handles the software info request message.
 * @details Sends out the software info via the sysmon GCP channel.
 *
 * @return  -
 */
GOS_STATIC void_t svl_pdhSoftwareInfoReqMsgReceived (gos_gcpChannelNumber_t gcpChannel)
{
	/*
	 * Function code.
	 */
	(void_t) svl_pdhGetSwInfo(&swInfoMsg);

	(void_t) gos_gcpTransmitMessage(
			gcpChannel,
			SVL_PDH_SYSMON_MSG_SOFTWARE_INFO_GET_RESP,
			(void_t*)&swInfoMsg,
			sizeof(swInfoMsg),
			0xFFFF);
}

/**
 * @brief   Handles the hardware info request message.
 * @details Sends out the hardware info via the sysmon GCP channel.
 *
 * @return  -
 */
GOS_STATIC void_t svl_pdhHardwareInfoReqMsgReceived (gos_gcpChannelNumber_t gcpChannel)
{
	/*
	 * Function code.
	 */
	(void_t) svl_pdhGetHwInfo(&hwInfoMsg);

	(void_t) gos_gcpTransmitMessage(
			gcpChannel,
			SVL_PDH_SYSMON_MSG_HARDWARE_INFO_GET_RESP,
			(void_t*)&hwInfoMsg,
			sizeof(hwInfoMsg),
			0xFFFF);
}

// TODO
GOS_STATIC void_t svl_pdhWifiCfgReqMsgReceived (gos_gcpChannelNumber_t gcpChannel)
{
	/*
	 * Function code.
	 */
	(void_t) svl_pdhGetWifiCfg(&wifiCfgMsg);

	(void_t) gos_gcpTransmitMessage(
			gcpChannel,
			SVL_PDH_SYSMON_MSG_WIFI_CONFIG_GET_RESP,
			(void_t*)&wifiCfgMsg,
			sizeof(wifiCfgMsg),
			0xFFFF);
}

// TODO
GOS_STATIC void_t svl_pdhBldCfgReqMsgReceived (gos_gcpChannelNumber_t gcpChannel)
{
	/*
	 * Function code.
	 */
	(void_t) svl_pdhGetBldCfg(&bldCfgMsg);

	(void_t) gos_gcpTransmitMessage(
			gcpChannel,
			SVL_PDH_SYSMON_MSG_BLD_CONFIG_GET_RESP,
			(void_t*)&bldCfgMsg,
			sizeof(bldCfgMsg),
			0xFFFF);
}

// TODO
GOS_STATIC void_t svl_pdhSoftwareInfoSetMsgReceived (gos_gcpChannelNumber_t gcpChannel)
{
	/*
	 * Function code.
	 */
	(void_t) memcpy((void_t*)&swInfoMsg, (void_t*)pdhBuffer, sizeof(svl_pdhSwInfo_t));

	(void_t) svl_pdhSetSwInfo(&swInfoMsg);
	(void_t) svl_pdhGetSwInfo(&swInfoMsg);

	(void_t) gos_gcpTransmitMessage(
			gcpChannel,
			SVL_PDH_SYSMON_MSG_SOFTWARE_INFO_SET_RESP,
			(void_t*)&swInfoMsg,
			sizeof(swInfoMsg),
			0xFFFF);
}

// TODO
GOS_STATIC void_t svl_pdhHardwareInfoSetMsgReceived (gos_gcpChannelNumber_t gcpChannel)
{
	/*
	 * Function code.
	 */
	(void_t) memcpy((void_t*)&hwInfoMsg, (void_t*)pdhBuffer, sizeof(svl_pdhHwInfo_t));

	(void_t) svl_pdhSetHwInfo(&hwInfoMsg);
	(void_t) svl_pdhGetHwInfo(&hwInfoMsg);

	(void_t) gos_gcpTransmitMessage(
			gcpChannel,
			SVL_PDH_SYSMON_MSG_HARDWARE_INFO_SET_RESP,
			(void_t*)&hwInfoMsg,
			sizeof(hwInfoMsg),
			0xFFFF);
}

// TODO
GOS_STATIC void_t svl_pdhWifiCfgSetMsgReceived (gos_gcpChannelNumber_t gcpChannel)
{
	/*
	 * Function code.
	 */
	(void_t) memcpy((void_t*)&wifiCfgMsg, (void_t*)pdhBuffer, sizeof(svl_pdhWifiCfg_t));

	(void_t) svl_pdhSetWifiCfg(&wifiCfgMsg);
	(void_t) svl_pdhGetWifiCfg(&wifiCfgMsg);

	(void_t) gos_gcpTransmitMessage(
			gcpChannel,
			SVL_PDH_SYSMON_MSG_WIFI_CONFIG_SET_RESP,
			(void_t*)&wifiCfgMsg,
			sizeof(wifiCfgMsg),
			0xFFFF);
}

// TODO
GOS_STATIC void_t svl_pdhBldCfgSetMsgReceived (gos_gcpChannelNumber_t gcpChannel)
{
	/*
	 * Function code.
	 */
	(void_t) memcpy((void_t*)&bldCfgMsg, (void_t*)pdhBuffer, sizeof(svl_pdhBldCfg_t));

	(void_t) svl_pdhSetBldCfg(&bldCfgMsg);
	(void_t) svl_pdhGetBldCfg(&bldCfgMsg);

	(void_t) gos_gcpTransmitMessage(
			gcpChannel,
			SVL_PDH_SYSMON_MSG_BLD_CONFIG_SET_RESP,
			(void_t*)&bldCfgMsg,
			sizeof(bldCfgMsg),
			0xFFFF);
}
