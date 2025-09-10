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
//! @file       bsp_tmr_handler.h
//! @author     Ahmed Gazar
//! @date       2025-09-10
//! @version    1.0
//!
//! @brief      Digital Step Sequencer / Board Support Package / Timer handler header.
//! @details    This component is the main control logic for DSS.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-09-10    Ahmed Gazar     Initial version created.
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
#ifndef BSP_TMR_HANDLER_H
#define BSP_TMR_HANDLER_H
/*
 * Includes
 */
#include "gos_lib.h"

/**
 * @brief   Starts the step timer.
 * @details Calls the start driver function of the
 *          configured step timer instance.
 *
 * @return  Success of starting the timer.
 *
 * @retval  #GOS_SUCCESS Starting the driver successful.
 * @retval  #GOS_ERROR   Starting the timer failed.
 */
gos_result_t bsp_tmrHandlerStartStepTimer (void_t);

/**
 * @brief   Stops the step timer.
 * @details Calls the stop driver function of the
 *          configured step timer instance.
 *
 * @return  Success of stopping the timer.
 *
 * @retval  #GOS_SUCCESS Stopping the driver successful.
 * @retval  #GOS_ERROR   Stopping the timer failed.
 */
gos_result_t bsp_tmrHandlerStopStepTimer (void_t);

#endif
