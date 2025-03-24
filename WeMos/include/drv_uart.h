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
//! @file       drv_uart.h
//! @author     Ahmed Gazar
//! @date       2024-04-24
//! @version    1.1
//!
//! @brief      GOS2022 Library / UART driver header.
//! @details    This component provides access to the UART peripheries.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-02-21    Ahmed Gazar     Initial version created.
// 1.1        2024-04-24    Ahmed Gazar     +    Error reporting added
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
#ifndef DRV_UART_H
#define DRV_UART_H

/*
 * Includes
 */
extern "C" {
#include <gos.h>
}

/*
 * Type definitions
 */

/*
 * Function prototypes
 */
/**
 * @brief   Initializes the registered UART peripheries.
 * @details Loops through the UART configuration array and initializes peripheries
 *          defined by the user (externally).
 *
 * @return  Result of initialization.
 *
 * @retval  GOS_SUCCESS : Initialization successful.
 * @retval  GOS_ERROR   : Configuration array is NULL or some peripheries
 *                        could not be initialized.
 */
gos_result_t drv_uartInit (
        void_t
        );

// TODO
gos_result_t drv_uartTransmitBlocking (
        char_t* message
        );

// TODO
gos_result_t drv_uartReceiveBlocking (
        char_t* pData
        );
#endif
