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
//! @file       drv_lcd.h
//! @author     Ahmed Gazar
//! @date       2025-04-21
//! @version    1.0
//!
//! @brief      GOS2022 Library / 16x2 LCD display driver header.
//! @details    This component provides the interface for 16x2 LCD display.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-04-21    Ahmed Gazar     Initial version created.
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
#ifndef DRV_LCD_H
#define DRV_LCD_H
/*
 * Includes
 */
#include <gos.h>
#include <drv_i2c.h>

/*
 * Type definitions
 */
typedef struct
{
    drv_i2cPeriphInstance_t i2cInstance;
    u16_t                   deviceAddress;
    u32_t                   writeMutexTmo;
    u32_t                   writeTriggerTmo;
}drv_lcdDescriptor_t;

// TODO
gos_result_t drv_lcdInit (void_t* pDevice);
// TODO
gos_result_t drv_lcdWriteString (void_t* pDevice, u8_t line, const char_t* str);
// TODO
gos_result_t drv_lcdSetCursor (void_t* pDevice, u8_t row, u8_t col);
// TODO
gos_result_t drv_lcdClear (void_t* pDevice);

#endif
