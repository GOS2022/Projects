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
//! @file       drv_ds3231.h
//! @author     Ahmed Gazar
//! @date       2025-04-09
//! @version    1.0
//!
//! @brief      GOS2022 Library / DS3231 real time clock driver header.
//! @details    This component provides the interface for the DS3231 real time clock chip.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-04-09    Ahmed Gazar     Initial version created.
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
#ifndef DRV_DS3231_H
#define DRV_DS3231_H
/*
 * Includes
 */
#include <gos.h>
#include <drv_i2c.h>

/*
 * Type definitions
 */
/**
 * DS3231 device descriptor.
 */
typedef struct
{
    drv_i2cPeriphInstance_t i2cInstance;
    u16_t                   deviceAddress;
    u32_t                   readMutexTmo;
    u32_t                   readTriggerTmo;
    u32_t                   writeMutexTmo;
    u32_t                   writeTriggerTmo;
}drv_ds3231Descriptor_t;

// TODO
gos_result_t drv_ds3231Init (void_t* pDevice);

// TODO
gos_result_t drv_ds3231GetTime (void_t* pDevice, gos_time_t* pTime, u8_t* dow);

// TODO
gos_result_t drv_ds3231SetTime (void_t* pDevice, gos_time_t* pTime, u8_t dow);

// TODO
gos_result_t drv_ds3231GetTemperature (void_t* pDevice, u16_t* pTemp);

#endif
