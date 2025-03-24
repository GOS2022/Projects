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
//                                      (c) Ahmed Gazar, 2022
//
//*************************************************************************************************
//! @file       gos.c
//! @author     Ahmed Gazar
//! @date       2023-07-12
//! @version    1.9
//!
//! @brief      GOS source.
//! @details    For a more detailed description of this service, please refer to @ref gos.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2022-10-31    Ahmed Gazar     Initial version created
// 1.1        2022-11-14    Ahmed Gazar     +    Initialization error flag setting modified
// 1.2        2022-11-15    Ahmed Gazar     +    License added
// 1.3        2022-12-11    Ahmed Gazar     +    main function added here
//                                          *    gos_Init and gos_Start made local static functions
//                                          +    platform and user initializers added
// 1.4        2023-01-12    Ahmed Gazar     *    OS service initialization and user application
//                                               initialization moved to a new initializer task
// 1.5        2023-01-31    Ahmed Gazar     *    Initializer task renamed to system task
//                                          +    Periodic CPU statistics calculation added to
//                                               system task
// 1.6        2023-05-04    Ahmed Gazar     -    Lock and trigger service initialization removed
// 1.7        2023-06-17    Ahmed Gazar     +    Dump added
//                                          +    GOS_SYS_TASK_SLEEP_TIME added
// 1.8        2023-06-28    Ahmed Gazar     +    Dump ready signal invoking added
// 1.9        2023-07-12    Ahmed Gazar     +    gos_sysmonInit added to initializers
//*************************************************************************************************
//
// Copyright (c) 2022 Ahmed Gazar
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
#include <gos.h>
#include <gos_task.h>

gos_result_t gos_init (void_t)
{
    gos_result_t initResult = GOS_SUCCESS;
    
    // Print startup logo.
    gos_printStartupLogo();

    initResult &= gos_errorTraceInit("Kernel initialization", gos_kernelInit());
    initResult &= gos_errorTraceInit("Trace service initialization", gos_traceInit());
    initResult &= gos_errorTraceInit("Time service initialization", gos_timeInit());
    initResult &= gos_errorTraceInit("GCP service initialization", gos_gcpInit());
    initResult &= gos_errorTraceInit("Shell service initialization", gos_shellInit());

    return initResult;
}