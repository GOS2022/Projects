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
//! @file       drv_mcp3008.h
//! @author     Ahmed Gazar
//! @date       2025-09-10
//! @version    1.0
//!
//! @brief      GOS2022 Library / MCP3008 driver header.
//! @details    This component provides the interface for the MCP3008 ADC chip.
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
#ifndef DRV_MCP3008_H
#define DRV_MCP3008_H
/*
 * Includes
 */
#include <gos.h>
#include <drv_spi.h>

/*
 * Type definitions
 */
/**
 * MCP3008 descriptor type.
 */
typedef struct
{
    drv_spiPeriphInstance_t spiInstance;     //!< SPI instance.
    u8_t                    csPin;           //!< Chip select pin index.
    gos_mutex_t             deviceMutex;     //!< Device mutex.
    u32_t                   readMutexTmo;    //!< Read mutex timeout [ms].
    u32_t                   readTriggerTmo;  //!< Read trigger timeout [ms].
    u32_t                   writeMutexTmo;   //!< Write mutex timeout [ms].
    u32_t                   writeTriggerTmo; //!< Write trigger timeout [ms].
}drv_mcp3008Descriptor_t;

/*
 * Function prototypes
 */
/**
 * @brief   Initializes the device.
 * @details Initializes the device mutex.
 *
 * @param   pDevice Pointer to the device descriptor structure.
 *
 * @return  Result of initialization.
 *
 * @retval  #GOS_SUCCESS Initialization successful.
 * @retval  #GOS_ERROR   Mutex initialization failed.
 */
gos_result_t drv_mcp3008Init (void_t* pDevice);

/**
 * @brief   Reads the given channel of the device.
 * @details Initializes the device mutex.
 *
 * @param   pDevice Pointer to the device descriptor structure.
 *
 * @return  Result of initialization.
 *
 * @retval  #GOS_SUCCESS Initialization successful.
 * @retval  #GOS_ERROR   Mutex initialization failed.
 */
gos_result_t drv_mcp3008ReadChannel (void_t* pDevice, u8_t channel, u16_t* pValue);

#endif
