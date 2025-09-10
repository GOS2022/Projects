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
//! @file       app_control.h
//! @author     Ahmed Gazar
//! @date       2025-09-09
//! @version    1.0
//!
//! @brief      Digital Step Sequencer / Application / Control header.
//! @details    This component is the main control logic for DSS.
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
#ifndef APP_CONTROL_H
#define APP_CONTROL_H
/*
 * Includes
 */
#include "gos_lib.h"

/*
 * Function prototypes
 */
/**
 * @brief     Initializes the component.
 * @details   Registers the IO callbacks, the control task, and initializes
 *            the internal trigger.
 *
 * @return    Result of initialization.
 *
 * @retval    #GOS_SUCCESS Initialization successful.
 * @retval    #GOS_ERROR   Configuration array is NULL or some peripheries
 *                         could not be initialized.
 */
gos_result_t app_controlInit (void_t);

/**
 * @brief     Callback for step trigger.
 * @details   Hardware timer interrupt callback.
 *
 * @return    -
 */
void_t app_controlStepTriggerCallback (void_t);

#endif
