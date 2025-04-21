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
//! @file       drv.h
//! @author     Ahmed Gazar
//! @date       2024-02-07
//! @version    1.0
//!
//! @brief      GOS2022 Library / Driver Layer header.
//! @details    This module is used to provide the driver information such as: version, author,
//!             name, description, and date.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-02-07    Ahmed Gazar     Initial version created.
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
#ifndef DRIVER_H
#define DRIVER_H
/*
 * Includes
 */
// TODO: used for layer includes.
#include <drv_24lc256.h>
#include <drv_25lc256.h>
#include <drv_25lc640.h>
#include <drv_w25q64.h>
#include <drv_ds3231.h>
#include <drv_adc.h>
#include <drv_crc.h>
#include <drv_dac.h>
#include <drv_dma.h>
#include <drv_error.h>
#include <drv_flash.h>
#include <drv_gpio.h>
#include <drv_i2c.h>
#include <drv_it.h>
#include <drv_lcd.h>
#include <drv_mcp23017.h>
#include <drv_shell.h>
#include <drv_spi.h>
#include <drv_sysmon.h>
#include <drv_systimer.h>
#include <drv_tmr.h>
#include <drv_trace.h>
#include <drv_uart.h>

#endif
