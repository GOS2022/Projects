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
//! @file       svl_dsm.c
//! @author     Ahmed Gazar
//! @date       2025-03-26
//! @version    1.1
//!
//! @brief      GOS2022 Library / Device State Manager source.
//! @details    For a more detailed description of this service, please refer to @ref svl_dsm.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-07-16    Ahmed Gazar     Initial version created.
// 1.1        2025-03-26    Ahmed Gazar     *    Startup info print made atomic
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
#include <svl_dsm.h>
#include <svl_pdh.h>
#include <gos_trace_driver.h>
#include <drv_trace.h>
#include <string.h>

/**
 * Platform initialization phase configuration array.
 */
GOS_EXTERN GOS_CONST svl_dsmInitPhaseDesc_t platformInitPhaseConfig [];

/**
 * Application initialization phase configuration array.
 */
GOS_EXTERN GOS_CONST svl_dsmInitPhaseDesc_t appInitPhaseConfig [];

/**
 * Reaction configuration array.
 */
GOS_EXTERN GOS_CONST svl_dsmReaction_t      reactionConfig [];

/**
 * Number of the platform initialization phases (shall be defined by user).
 */
GOS_EXTERN u32_t                            platformInitPhaseConfigSize;

/**
 * Number of the application initialization phases (shall be defined by user).
 */
GOS_EXTERN u32_t                            appInitPhaseConfigSize;

/**
 * Size of the reaction configuration (shall be defined by user).
 */
GOS_EXTERN u32_t                            reactionConfigSize;

/**
 * Device actual state.
 */
GOS_STATIC svl_dsmState_t                   deviceState     = DSM_STATE_STARTUP;

/**
 * Device previous state.
 */
GOS_STATIC svl_dsmState_t                   devicePrevState = DSM_STATE_STARTUP;

/*
 * Static function prototypes.
 */
GOS_STATIC void_t svl_dsmDaemonTask (void_t);

/**
 * DSM daemon task descriptor.
 */
gos_taskDescriptor_t svlDsmDaemonDesc =
{
	.taskFunction	    = svl_dsmDaemonTask,
	.taskName		    = "svl_dsm_daemon",
	.taskStackSize 	    = SVL_DSM_DAEMON_STACK_SIZE,
	.taskPriority 	    = 0u,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_KERNEL
};

/*
 * Function: svl_dsmInit
 */
gos_result_t svl_dsmInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t dsmInitResult = GOS_ERROR;
	u8_t         initIdx       = 0u;
	u8_t         initBlockIdx  = 0u;

	/*
	 * Function code.
	 */
	if (appInitPhaseConfig != NULL && appInitPhaseConfigSize > 0u)
	{
		// Register DSM task.
		dsmInitResult = gos_errorTraceInit("DSM initialization", gos_taskRegister(&svlDsmDaemonDesc, NULL));

		// Loop through all init phases.
		for (initIdx = 0u; initIdx < appInitPhaseConfigSize / sizeof(svl_dsmInitPhaseDesc_t); initIdx++)
		{
			(void_t) gos_traceDriverTransmitString_Unsafe("\r\n");
			(void_t) gos_traceDriverTransmitString_Unsafe(appInitPhaseConfig[initIdx].phaseName);
			(void_t) gos_traceDriverTransmitString_Unsafe("\r\n");

			for (initBlockIdx = 0u; initBlockIdx < SVL_DSM_MAX_INITIALIZERS; initBlockIdx++)
			{
				if (appInitPhaseConfig[initIdx].initBlock[initBlockIdx].pInitializer != NULL)
				{
					dsmInitResult &= gos_errorTraceInit(
							appInitPhaseConfig[initIdx].initBlock[initBlockIdx].description,
							appInitPhaseConfig[initIdx].initBlock[initBlockIdx].pInitializer());
				}
				else
				{
					// End of initializers.
					break;
				}
			}
		}
	}
	else
	{
		// Configuration is empty.
	}

	GOS_CONVERT_RESULT(dsmInitResult);

	return dsmInitResult;
}

/*
 * Function: svl_dsmPrintOSInfo
 */
gos_result_t svl_dsmPrintOSInfo (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t printResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "\r\n"TRACE_BG_BLUE_START"OS Info"TRACE_FORMAT_RESET"\r\n"));
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Version: %02u.%02u\r\n", GOS_VERSION_MAJOR, GOS_VERSION_MINOR));

	return printResult;
}

/*
 * Function: svl_dsmPrintLibInfo
 */
gos_result_t svl_dsmPrintLibInfo (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t       printResult = GOS_SUCCESS;
	svl_pdhSwVerInfo_t libVer      = {0};

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(printResult, svl_pdhGetLibVersion(&libVer));
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "\r\n"TRACE_BG_BLUE_START"Library Info"TRACE_FORMAT_RESET"\r\n"));
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Name:        \t%s\r\n", libVer.name));
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Description: \t%s\r\n", libVer.description));
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Version:     \t%02u.%02u.%02u\r\n", libVer.major, libVer.minor, libVer.build));
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Date:        \t%4u-%02u-%02u\r\n", libVer.date.years, libVer.date.months, libVer.date.days));
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Author:      \t%s\r\n", libVer.author));

	return printResult;
}

/*
 * Function: svl_dsmPrintHwInfo
 */
gos_result_t svl_dsmPrintHwInfo (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t    printResult = GOS_SUCCESS;
	svl_pdhHwInfo_t hwInfo      = {0};

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(printResult, svl_pdhGetHwInfo(&hwInfo));

	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "\r\n"TRACE_BG_BLUE_START"Hardware Info"TRACE_FORMAT_RESET"\r\n"));
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Board name:    \t%s\r\n", hwInfo.boardName));
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Author:        \t%s\r\n", hwInfo.author));
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Description:   \t%s\r\n", hwInfo.description));
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Revision:      \t%s\r\n", hwInfo.revision));
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Serial number: \t%s\r\n", hwInfo.serialNumber));
	GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Date:          \t%04hu-%02u-%02u\r\n",
			hwInfo.date.years,
			hwInfo.date.months,
			hwInfo.date.days
			));

	return printResult;
}

/*
 * Function: svl_dsmPrintAppInfo
 */
gos_result_t svl_dsmPrintAppInfo (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t    printResult = GOS_SUCCESS;
	svl_pdhSwInfo_t swInfo      = {0};

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(printResult, svl_pdhGetSwInfo(&swInfo));

	if (strcmp(swInfo.bldSwVerInfo.name, "") != 0)
	{
		GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "\r\n"TRACE_BG_BLUE_START"Bootloader Info"TRACE_FORMAT_RESET"\r\n"));
		GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Name:        \t%s\r\n", swInfo.bldSwVerInfo.name));
		GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Description: \t%s\r\n", swInfo.bldSwVerInfo.description));
		GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Version:     \t%02u.%02u.%02u\r\n", swInfo.bldSwVerInfo.major, swInfo.bldSwVerInfo.minor, swInfo.bldSwVerInfo.build));
		GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Date:        \t%4u-%02u-%02u\r\n", swInfo.bldSwVerInfo.date.years, swInfo.bldSwVerInfo.date.months, swInfo.bldSwVerInfo.date.days));
		GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Author:      \t%s\r\n", swInfo.bldSwVerInfo.author));
	}
	else
	{
		// Bootloader does not exist.
	}

	if (strcmp(swInfo.appSwVerInfo.name, "") != 0)
	{
		GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "\r\n"TRACE_BG_BLUE_START"Application Info"TRACE_FORMAT_RESET"\r\n"));
		GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Name:        \t%s\r\n", swInfo.appSwVerInfo.name));
		GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Description: \t%s\r\n", swInfo.appSwVerInfo.description));
		GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Version:     \t%02u.%02u.%02u\r\n", swInfo.appSwVerInfo.major, swInfo.appSwVerInfo.minor, swInfo.appSwVerInfo.build));
		GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Date:        \t%4u-%02u-%02u\r\n", swInfo.appSwVerInfo.date.years, swInfo.appSwVerInfo.date.months, swInfo.appSwVerInfo.date.days));
		GOS_CONCAT_RESULT(printResult, gos_traceTraceFormatted(GOS_FALSE, "Author:      \t%s\r\n", swInfo.appSwVerInfo.author));
	}
	else
	{
		// Application does not exist.
	}

	return printResult;
}

/*
 * Function: svl_dsmWaitForState
 */
GOS_INLINE void_t svl_dsmWaitForState (svl_dsmState_t requiredState)
{
	/*
	 * Function code.
	 */
	while (deviceState != requiredState)
	{
		(void_t) gos_taskSleep(20);
	}
}

/*
 * Function: svl_dsmSetState
 */
GOS_INLINE void_t svl_dsmSetState (svl_dsmState_t requiredState)
{
	/*
	 * Function code.
	 */
	devicePrevState = deviceState;
	deviceState     = requiredState;
}

/*
 * Function: svl_dsmPlatformInit
 */
__attribute__((weak)) gos_result_t svl_dsmPlatformInit (void_t)
{
    /*
     * Function code.
     */
	(void_t) gos_errorHandler(GOS_ERROR_LEVEL_USER_WARNING, __func__, __LINE__, "Platform initializer not implemented!");

	return GOS_SUCCESS;
}

/*
 * Function: svl_dsmApplicationInit
 */
__attribute__((weak)) gos_result_t svl_dsmApplicationInit (void_t)
{
    /*
     * Function code.
     */
	(void_t) gos_errorHandler(GOS_ERROR_LEVEL_USER_WARNING, __func__, __LINE__, "Application initializer not implemented!");

	return GOS_SUCCESS;
}

/*
 * Function: gos_platformDriverInit
 */
gos_result_t gos_platformDriverInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult   = GOS_ERROR;
	u8_t         initIdx      = 0u;
	u8_t         initBlockIdx = 0u;
	char_t       tempBuff [48];

    /*
     * Function code.
     */
	initResult = svl_dsmPlatformInit();

	if (platformInitPhaseConfig != NULL && platformInitPhaseConfigSize > 0u)
	{
		// Loop through all init phases.
		for (initIdx = 0u; initIdx < platformInitPhaseConfigSize / sizeof(svl_dsmInitPhaseDesc_t); initIdx++)
		{
			(void_t) sprintf(tempBuff, "\r\n%s\r\n", platformInitPhaseConfig[initIdx].phaseName);
			(void_t) drv_traceEnqueueTraceMessage(tempBuff, GOS_TRUE, GOS_SUCCESS);

			for (initBlockIdx = 0u; initBlockIdx < SVL_DSM_MAX_INITIALIZERS; initBlockIdx++)
			{
				if (platformInitPhaseConfig[initIdx].initBlock[initBlockIdx].pInitializer != NULL)
				{
					initResult &= drv_traceEnqueueTraceMessage(
							platformInitPhaseConfig[initIdx].initBlock[initBlockIdx].description,
							GOS_FALSE,
							platformInitPhaseConfig[initIdx].initBlock[initBlockIdx].pInitializer());
				}
				else
				{
					// End of initializers.
					break;
				}
			}
		}
	}
	else
	{
		// Configuration is empty.
	}

	GOS_CONVERT_RESULT(initResult);

    return initResult;
}

/*
 * Function: gos_userApplicationInit
 */
gos_result_t gos_userApplicationInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_ERROR;

    /*
     * Function code.
     */
	// Flush collected traces.
	drv_traceFlushTraceEntries();

	initResult = svl_dsmInit();

	// Final step is to call application initializer.
	initResult &= svl_dsmApplicationInit();

	GOS_CONVERT_RESULT(initResult);

    return initResult;
}

/**
 * @brief   DSM daemon task.
 * @details During initialization, this task starts as the highest
 *          priority task and print all the related software info
 *          on the trace output. After this initialization phase,
 *          the task changes its priority to become a background task,
 *          and polls for change in the state. If a state-change occurs,
 *          it activates the related reactions - if any.
 *
 * @return  -
 */
GOS_STATIC void_t svl_dsmDaemonTask (void_t)
{
	/*
	 * Local variables.
	 */
	u8_t reactionIdx = 0u;

	/*
	 * Function code.
	 */
	// Disable scheduling to keep printed info consistent.
	GOS_DISABLE_SCHED

	// Print out OS info.
	(void_t) svl_dsmPrintOSInfo();

	// Print out library info.
	(void_t) svl_dsmPrintLibInfo();

	// Print out Hardware Info.
	(void_t) svl_dsmPrintHwInfo();

	// Print out Application Info.
	(void_t) svl_dsmPrintAppInfo();

	// Enable scheduling, data printed.
	GOS_ENABLE_SCHED

	// Change priority.
	(void_t) gos_taskSetPriority(svlDsmDaemonDesc.taskId, SVL_DSM_DAEMON_PRIO);

	// Startup ready.
	svl_dsmSetState(DSM_STATE_STARTUP_READY);

	for (;;)
	{
		// Check for state change.
		if (devicePrevState != deviceState && reactionConfig != NULL && reactionConfigSize > 0u)
		{
			for (reactionIdx = 0u; reactionIdx < reactionConfigSize / sizeof(svl_dsmReaction_t); reactionIdx++)
			{
				if (reactionConfig[reactionIdx].prevState == devicePrevState &&
					reactionConfig[reactionIdx].currState == deviceState &&
					reactionConfig[reactionIdx].pReaction != NULL)
				{
					// Execute reaction.
					reactionConfig[reactionIdx].pReaction();
				}
				else
				{
					// No match.
				}
			}

			// Resolve device state change.
			devicePrevState = deviceState;
		}
		else
		{
			// State change not detected.
		}

		(void_t) gos_taskSleep(SVL_DSM_DAEMON_POLL_MS);
	}
}
