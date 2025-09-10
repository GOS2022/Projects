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
//! @file       app_init.c
//! @author     Ahmed Gazar
//! @date       2025-09-10
//! @version    1.0
//!
//! @brief      Digital Step Sequencer / Application / Initializer source.
//! @details    This component contains the platform and application initialization.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-09-09    Ahmed Gazar     Initial version created.
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
#include <gos_lib.h>

/*
 * Static variables
 */
/**
 * GOS driver functions for trace and system timer.
 */
GOS_STATIC gos_driver_functions_t driverFunctions =
{
	.traceDriverTransmitString       = drv_traceTransmit,
	.traceDriverTransmitStringUnsafe = drv_traceTransmitUnsafe,
	.timerDriverSysTimerGetValue     = drv_systimerGetValue,
};

/*
 * Function prototypes
 */
GOS_STATIC void_t app_faultHandler (gos_faultType_t faultType);

/*
 * Function: svl_dsmPlatformInit
 */
gos_result_t svl_dsmPlatformInit (void_t)
{
	/*
	 * Local variables.
	 */
	// Platform driver initialization result.
	gos_result_t platformDriverInitResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	// Driver init.
    platformDriverInitResult &= drv_traceEnqueueTraceMessage(
    		"HAL library initialization",
			GOS_FALSE,
			HAL_Init() == HAL_OK ? GOS_SUCCESS : GOS_ERROR
    );

	platformDriverInitResult &= drv_traceEnqueueTraceMessage("Register systick hook", GOS_FALSE, gos_kernelRegisterSysTickHook(HAL_IncTick));

	// Register kernel drivers.
	platformDriverInitResult &= gos_driverInit(&driverFunctions);

    GOS_CONVERT_RESULT(platformDriverInitResult);

    SysTick->VAL = 0;
    SysTick->CTRL = 0b111;

    __enable_irq();

	return platformDriverInitResult;
}

/*
 * Function: svl_dsmApplicationInit
 */
gos_result_t svl_dsmApplicationInit (void_t)
{
	/*
	 * Function code.
	 */
	(void_t) gos_kernelRegisterFaultHook(app_faultHandler);
	return gos_traceTrace(GOS_FALSE, "Welcome to the Digital Step Sequencer application!");
}

/**
 * TODO
 * @param faultType
 * @return
 */
GOS_STATIC void_t app_faultHandler (gos_faultType_t faultType)
{
	GOS_NOP;
	HAL_NVIC_SystemReset();
}
