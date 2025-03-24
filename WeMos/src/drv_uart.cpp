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
//! @file       drv_uart.c
//! @author     Ahmed Gazar
//! @date       2024-04-24
//! @version    1.1
//!
//! @brief      GOS2022 Library / UART driver source.
//! @details    For a more detailed description of this driver, please refer to @ref drv_uart.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-02-01    Ahmed Gazar     Initial version created.
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
/*
 * Includes
 */
#include <drv_uart.h>
#include "SoftwareSerial.h"

/*
 * Macros
 */
#define UART_TX      D1
#define UART_RX      D2

/*
 * Static variables
 */
static SoftwareSerial uart(UART_RX, UART_TX); // RX, TX

/*
 * Function: drv_uartInit
 */
gos_result_t drv_uartInit (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t uartDriverInitResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    uart.begin(57600);

    return uartDriverInitResult;
}

/*
 * Function: drv_uartTransmitBlocking
 */
gos_result_t drv_uartTransmitBlocking (
        char_t* message
        )
{
    /*
     * Local variables.
     */
    gos_result_t uartTransmitResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    uart.print(message);

    return uartTransmitResult;
}

/*
 * Function: drv_uartReceiveBlocking
 */
gos_result_t drv_uartReceiveBlocking (
        char_t* pData
        )
{
    /*
     * Local variables.
     */
    gos_result_t uartReceiveResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (uart.available() > 0)
    {
        *pData = (char_t)uart.read();
        uartReceiveResult = GOS_SUCCESS;
    }

    return uartReceiveResult;
}