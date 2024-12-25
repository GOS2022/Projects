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
//! @date       2024-07-16
//! @version    1.0
//!
//! @brief      GOS2022 Library / Device State Manager source.
//! @details    For a more detailed description of this driver, please refer to @ref svl_dsm.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-07-16    Ahmed Gazar     Initial version created.
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

/**
 * TODO
 */
GOS_EXTERN GOS_CONST svl_dsmInitPhaseDesc_t initPhaseConfig [];

/**
 * TODO
 */
GOS_EXTERN GOS_CONST svl_dsmReaction_t      reactionConfig [];

/**
 * TODO (shall be defined by user).
 */
GOS_EXTERN u32_t                            initPhaseConfigSize;

/**
 * TODO
 */
GOS_EXTERN u32_t                            reactionConfigSize;

/**
 * TODO
 */
GOS_STATIC svl_dsmState_t                   deviceState     = DSM_STATE_STARTUP;

/**
 * TODO
 */
GOS_STATIC svl_dsmState_t                   devicePrevState = DSM_STATE_STARTUP;

/*
 * Static function prototypes.
 */
GOS_STATIC void_t svl_dsmTask (void_t);

/**
 * TODO
 */
gos_taskDescriptor_t svlDsmDaemonDesc =
{
	.taskFunction	    = svl_dsmTask,
	.taskName		    = "svl_dsm_daemon",
	.taskStackSize 	    = 0x600,
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
	gos_result_t dsmInitResult = GOS_SUCCESS;
	u8_t         initIdx       = 0u;
	u8_t         initBlockIdx  = 0u;

	/*
	 * Function code.
	 */
	if (initPhaseConfig != NULL && initPhaseConfigSize > 0u)
	{
		// Register DSM task.
		dsmInitResult &= gos_taskRegister(&svlDsmDaemonDesc, NULL);

		// Loop through all init phases.
		for (initIdx = 0u; initIdx < initPhaseConfigSize / sizeof(svl_dsmInitPhaseDesc_t); initIdx++)
		{
			(void_t) gos_traceDriverTransmitString_Unsafe("\r\n");
			(void_t) gos_traceDriverTransmitString_Unsafe(initPhaseConfig[initIdx].phaseName);
			(void_t) gos_traceDriverTransmitString_Unsafe("\r\n");

			for (initBlockIdx = 0u; initBlockIdx < SVL_DSM_MAX_INITIALIZERS; initBlockIdx++)
			{
				if (initPhaseConfig[initIdx].initBlock[initBlockIdx].pInitializer != NULL)
				{
					dsmInitResult &= gos_errorTraceInit(
							initPhaseConfig[initIdx].initBlock[initBlockIdx].description,
							initPhaseConfig[initIdx].initBlock[initBlockIdx].pInitializer());
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

	if (dsmInitResult != GOS_SUCCESS)
	{
		dsmInitResult = GOS_ERROR;
	}
	else
	{
		// Success.
	}

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
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "\r\n"TRACE_BG_BLUE_START"OS Info"TRACE_FORMAT_RESET"\r\n");
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Version: %02u.%02u\r\n", GOS_VERSION_MAJOR, GOS_VERSION_MINOR);

	if (printResult != GOS_SUCCESS)
	{
		printResult = GOS_ERROR;
	}
	else
	{
		// Success.
	}

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
	printResult &= svl_pdhGetLibVersion(&libVer);

	printResult &= gos_traceTraceFormatted(GOS_FALSE, "\r\n"TRACE_BG_BLUE_START"Library Info"TRACE_FORMAT_RESET"\r\n");
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Name:        \t%s\r\n", libVer.name);
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Description: \t%s\r\n", libVer.description);
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Version:     \t%02u.%02u.%02u\r\n", libVer.major, libVer.minor, libVer.build);
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Date:        \t%4u-%02u-%02u\r\n", libVer.date.years, libVer.date.months, libVer.date.days);
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Author:      \t%s\r\n", libVer.author);

	if (printResult != GOS_SUCCESS)
	{
		printResult = GOS_ERROR;
	}
	else
	{
		// Success.
	}

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
	printResult &= svl_pdhGetHwInfo(&hwInfo);

	printResult &= gos_traceTraceFormatted(GOS_FALSE, "\r\n"TRACE_BG_BLUE_START"Hardware Info"TRACE_FORMAT_RESET"\r\n");
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Board name:    %s\r\n", hwInfo.boardName);
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Author:        %s\r\n", hwInfo.author);
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Description:   %s\r\n", hwInfo.description);
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Revision:      %s\r\n", hwInfo.revision);
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Serial number: %s\r\n", hwInfo.serialNumber);
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Date:          %04hu-%02u-%02u\r\n",
			hwInfo.date.years,
			hwInfo.date.months,
			hwInfo.date.days
			);

	if (printResult != GOS_SUCCESS)
	{
		printResult = GOS_ERROR;
	}
	else
	{
		// Success.
	}

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
	printResult &= svl_pdhGetSwInfo(&swInfo);

	printResult &= gos_traceTraceFormatted(GOS_FALSE, "\r\n"TRACE_BG_BLUE_START"Application Info"TRACE_FORMAT_RESET"\r\n");
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Name:        \t%s\r\n", swInfo.appSwVerInfo.name);
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Description: \t%s\r\n", swInfo.appSwVerInfo.description);
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Version:     \t%02u.%02u.%02u\r\n", swInfo.appSwVerInfo.major, swInfo.appSwVerInfo.minor, swInfo.appSwVerInfo.build);
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Date:        \t%4u-%02u-%02u\r\n", swInfo.appSwVerInfo.date.years, swInfo.appSwVerInfo.date.months, swInfo.appSwVerInfo.date.days);
	printResult &= gos_traceTraceFormatted(GOS_FALSE, "Author:      \t%s\r\n", swInfo.appSwVerInfo.author);

	if (printResult != GOS_SUCCESS)
	{
		printResult = GOS_ERROR;
	}
	else
	{
		// Success.
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
		gos_taskSleep(20);
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
	deviceState = requiredState;
}

/**
 * TODO
 * @param
 * @return
 */
GOS_STATIC void_t svl_dsmTask (void_t)
{
	/*
	 * Local variables.
	 */
	u8_t reactionIdx = 0u;

	/*
	 * Function code.
	 */
	// Print out OS info.
	(void_t) svl_dsmPrintOSInfo();

	// Print out library info.
	(void_t) svl_dsmPrintLibInfo();

	// Print out Hardware Info.
	(void_t) svl_dsmPrintHwInfo();

	// Print out Application Info.
	(void_t) svl_dsmPrintAppInfo();

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
