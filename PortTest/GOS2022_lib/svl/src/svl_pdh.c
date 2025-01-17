/*
 * Includes
 */
#include <svl_pdh.h>
#include <string.h>

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

/*
 * Function prototypes
 */
GOS_STATIC void_t svl_pdhSoftwareInfoReqMsgReceived (void_t);
GOS_STATIC void_t svl_pdhHardwareInfoReqMsgReceived (void_t);

/**
 * System monitoring software information request message.
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t softwareInfoReqMsg =
{
	.callback        = svl_pdhSoftwareInfoReqMsgReceived,
	.messageId       = 0x2000,
	.payload         = NULL,
	.payloadSize     = 0u,
	.protocolVersion = 1u
};

/**
 * System monitoring hardware information request message.
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t hardwareInfoReqMsg =
{
	.callback        = svl_pdhHardwareInfoReqMsgReceived,
	.messageId       = 0x2001,
	.payload         = NULL,
	.payloadSize     = 0u,
	.protocolVersion = 1u
};

/*
 * Function: svl_pdhInit
 */
gos_result_t svl_pdhInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	initResult = gos_sysmonRegisterUserMessage(&softwareInfoReqMsg);
	initResult &= gos_sysmonRegisterUserMessage(&hardwareInfoReqMsg);
	initResult &= gos_mutexInit(&pdhMutex);

	pdhInited = GOS_TRUE;

	if (initResult != GOS_SUCCESS)
	{
		initResult = GOS_ERROR;
	}
	else
	{
		// OK.
	}

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
		setResult = pdhWriteFunction(PDH_ADDR_SW_INFO, (u8_t*)pSwInfo, sizeof(*pSwInfo));
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
GOS_STATIC void_t svl_pdhSoftwareInfoReqMsgReceived (void_t)
{
	/*
	 * Local variables.
	 */
	svl_pdhSwInfo_t swInfoMsg = {0};

	/*
	 * Function code.
	 */
	(void_t) svl_pdhGetSwInfo(&swInfoMsg);

	(void_t) gos_gcpTransmitMessage(
    		CFG_SYSMON_GCP_CHANNEL_NUM,
			0xB000,
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
GOS_STATIC void_t svl_pdhHardwareInfoReqMsgReceived (void_t)
{
	/*
	 * Local variables.
	 */
	svl_pdhHwInfo_t hwInfoMsg = {0};

	/*
	 * Function code.
	 */
	(void_t) svl_pdhGetHwInfo(&hwInfoMsg);

	(void_t) gos_gcpTransmitMessage(
    		CFG_SYSMON_GCP_CHANNEL_NUM,
			0xB001,
			(void_t*)&hwInfoMsg,
			sizeof(hwInfoMsg),
			0xFFFF);
}
