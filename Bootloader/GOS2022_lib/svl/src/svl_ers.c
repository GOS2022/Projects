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
//! @file       svl_ers.c
//! @author     Ahmed Gazar
//! @date       2025-01-29
//! @version    1.0
//!
//! @brief      GOS2022 Library / Event Reporting Service source.
//! @details    For a more detailed description of this driver, please refer to @ref svl_ers.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-01-29    Ahmed Gazar     Initial version created.
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
#include <svl_ers.h>
#include <string.h>

/*
 * Macros
 */
/**
 * Size of ERS buffer.
 */
#define ERS_BUFFER_SIZE ( 1024u )

/*
 * Type definitions
 */
typedef enum
{
    SVL_ERS_SYSMON_MSG_EVENTS_GET_REQ = 0x4001,
    SVL_ERS_SYSMON_MSG_EVENTS_GET_RESP = 0x4A01,
    SVL_ERS_SYSMON_MSG_EVENTS_CLEAR_REQ = 0x4002,
    SVL_ERS_SYSMON_MSG_EVENTS_CLEAR_RESP = 0x4A02,
}svl_ersSysmonMsgId_t;

/*
 * Static variables
 */
/**
 * Read function pointer.
 */
GOS_STATIC svl_ersReadWriteFunc_t ersReadFunction = NULL;

/**
 * Write function pointer.
 */
GOS_STATIC svl_ersReadWriteFunc_t ersWriteFunction = NULL;

/**
 * Service mutex.
 */
GOS_STATIC gos_mutex_t            ersMutex;

/**
 * ERS buffer.
 */
GOS_STATIC u8_t                   ersBuffer [ERS_BUFFER_SIZE];

/**
 * Pre-defined event descriptors.
 */
GOS_EXTERN svl_ersEventDesc_t     ersEvents[];

/**
 * Size of the event descriptor array.
 */
GOS_EXTERN u32_t                  ersEventsSize;

/*
 * Function prototypes
 */
GOS_STATIC gos_result_t svl_ersSetNumOfEntries   (u32_t numOfEntries);
GOS_STATIC void_t       svl_ersEventsReqCallback (void_t);
GOS_STATIC void_t       svl_ersEventsClrCallback (void_t);

/**
 * Sysmon ERS events request message.
 */
gos_sysmonUserMessageDescriptor_t ersEventsRequestMsg =
{
	.callback        = svl_ersEventsReqCallback,
	.messageId       = SVL_ERS_SYSMON_MSG_EVENTS_GET_REQ,
	.payloadSize     = 0u,
	.protocolVersion = 1,
	.payload         = NULL
};

/**
 * Sysmon ERS events clear message.
 */
gos_sysmonUserMessageDescriptor_t ersEventsClearMsg =
{
	.callback        = svl_ersEventsClrCallback,
	.messageId       = SVL_ERS_SYSMON_MSG_EVENTS_CLEAR_REQ,
	.payloadSize     = 0u,
	.protocolVersion = 1,
	.payload         = NULL
};

/*
 * Function: svl_ersInit
 */
gos_result_t svl_ersInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	initResult &= gos_sysmonRegisterUserMessage(&ersEventsRequestMsg);
	initResult &= gos_sysmonRegisterUserMessage(&ersEventsClearMsg);
	initResult &= gos_mutexInit(&ersMutex);

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
 * Function: svl_ersConfigure
 */
gos_result_t svl_ersConfigure (svl_ersCfg_t* pCfg)
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
		ersReadFunction  = pCfg->readFunction;
		ersWriteFunction = pCfg->writeFunction;
		cfgResult        = GOS_SUCCESS;
	}
	else
	{
		// Error.
	}

	return cfgResult;
}

/*
 * Function: svl_ersCreate
 */
gos_result_t svl_ersCreate (u16_t index, u32_t trigger, u8_t* pEventData)
{
	/*
	 * Local variables.
	 */
	gos_result_t createResult = GOS_ERROR;
	u32_t        numOfEntries = 0u;
	u32_t        address      = 0u;

	/*
	 * Function code.
	 */
	if (ersWriteFunction != NULL && index < (u16_t)(ersEventsSize / sizeof(svl_ersEventDesc_t)))
	{
		(void_t) gos_timeGet(&ersEvents[index].timeStamp);
		ersEvents[index].trigger = trigger;

		if (pEventData != NULL)
		{
			(void_t) memcpy((void_t*)ersEvents[index].eventData, (void_t*)pEventData, SVL_ERS_DATA_SIZE);
		}
		else
		{
			// Event data not needed.
			(void_t) memset((void_t*)ersEvents[index].eventData, 0, SVL_ERS_DATA_SIZE);
		}

		// Get the number of events.
		if (svl_ersGetNumOfEntries(&numOfEntries)          == GOS_SUCCESS &&
			gos_mutexLock(&ersMutex, SVL_ERS_MUTEX_TMO_MS) == GOS_SUCCESS)
		{
			// Calculate next event address.
			address = SVL_ERS_ENTRY_START_ADDR + numOfEntries * sizeof(svl_ersEventDesc_t);

			// Write new fault.
			if (ersWriteFunction(address, (u8_t*)&ersEvents[index], sizeof(svl_ersEventDesc_t)) == GOS_SUCCESS)
			{
				// Update number of entries.
				numOfEntries++;

				createResult = svl_ersSetNumOfEntries(numOfEntries);
			}
			else
			{
				// ERS write failed.
			}

			(void_t) gos_mutexUnlock(&ersMutex);
		}
		else
		{
			// Mutex error or getting number of entries failed.
		}
	}
	else
	{
		// Index out of range.
	}

	return createResult;
}

/*
 * Function: svl_ersRead
 */
gos_result_t svl_ersRead (u16_t index, u8_t* pTarget)
{
	/*
	 * Local variables.
	 */
	gos_result_t readResult   = GOS_ERROR;
	u32_t        numOfEntries = 0u;
	u32_t        address      = 0u;

	/*
	 * Function code.
	 */
	if (pTarget                                        != NULL &&
		ersReadFunction                                != NULL &&
		svl_ersGetNumOfEntries(&numOfEntries)          == GOS_SUCCESS &&
		index                                          < numOfEntries &&
		gos_mutexLock(&ersMutex, SVL_ERS_MUTEX_TMO_MS) == GOS_SUCCESS)
	{
		address = SVL_ERS_ENTRY_START_ADDR + index * sizeof(svl_ersEventDesc_t);
		readResult = ersReadFunction(address, pTarget, sizeof(svl_ersEventDesc_t));
		(void_t) gos_mutexUnlock(&ersMutex);
	}
	else
	{
		// Error.
	}

	return readResult;
}

/*
 * Function: svl_ersGetNumOfEntries
 */
gos_result_t svl_ersGetNumOfEntries (u32_t* pNumOfEntries)
{
	/*
	 * Local variables.
	 */
	gos_result_t getNumOfEntriesResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if (ersReadFunction != NULL &&
		gos_mutexLock(&ersMutex, SVL_ERS_MUTEX_TMO_MS) == GOS_SUCCESS)
	{
		getNumOfEntriesResult = ersReadFunction(SVL_ERS_ENTRY_NUM_ADDR, (u8_t*)pNumOfEntries, sizeof(*pNumOfEntries));
	}
	else
	{
		// Read function is NULL.
	}

	(void_t) gos_mutexUnlock(&ersMutex);

	return getNumOfEntriesResult;
}

/*
 * Function: svl_ersClearEvents
 */
gos_result_t svl_ersClearEvents (void_t)
{
	return svl_ersSetNumOfEntries(0);
}

/**
 * @brief   Sets the number of entries.
 * @details TODO
 *
 * @param   numOfEntries : Desired number to be set.
 *
 * @return  Result of entry number setting.
 *
 * @retval  GOS_SUCCESS : Number setting successful.
 * @retval  GOS_ERROR   :
 */
GOS_STATIC gos_result_t svl_ersSetNumOfEntries (u32_t numOfEntries)
{
	/*
	 * Local variables.
	 */
	gos_result_t setNumOfEntriesResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if (ersWriteFunction != NULL)
	{
		setNumOfEntriesResult = ersWriteFunction(SVL_ERS_ENTRY_NUM_ADDR, (u8_t*)&numOfEntries, sizeof(numOfEntries));
	}
	else
	{
		// Read function is NULL.
	}

	return setNumOfEntriesResult;
}

/**
 * @brief   TODO
 * @details TODO
 *
 * @return  -
 */
GOS_STATIC void_t svl_ersEventsReqCallback (void_t)
{
	/*
	 * Local variables.
	 */
	u32_t numOfEvents = 0u;
	u16_t eventIdx    = 0u;

	/*
	 * Function code.
	 */
	(void_t) svl_ersGetNumOfEntries(&numOfEvents);

	for (eventIdx = 0u; eventIdx < numOfEvents; eventIdx++)
	{
		(void_t) svl_ersRead(eventIdx, ersBuffer + eventIdx * sizeof(svl_ersEventDesc_t));
	}

	(void_t) gos_gcpTransmitMessage(
    		CFG_SYSMON_GCP_CHANNEL_NUM,
			SVL_ERS_SYSMON_MSG_EVENTS_GET_RESP,
			(void_t*)ersBuffer,
			numOfEvents * sizeof(svl_ersEventDesc_t),
			0xFFFF);
}

/**
 * @brief   TODO
 * @details TODO
 *
 * @return  -
 */
GOS_STATIC void_t svl_ersEventsClrCallback (void_t)
{
	/*
	 * Local variables.
	 */
	u32_t eventNum = 0;

	/*
	 * Function code.
	 */
	(void_t) svl_ersGetNumOfEntries(&eventNum);
	(void_t) svl_ersClearEvents();
	(void_t) svl_ersGetNumOfEntries(&eventNum);

	(void_t) gos_gcpTransmitMessage(
    		CFG_SYSMON_GCP_CHANNEL_NUM,
			SVL_ERS_SYSMON_MSG_EVENTS_CLEAR_RESP,
			(void_t*)&eventNum,
			sizeof(eventNum),
			0xFFFF);
}
