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
//! @file       drv_24lc256.h
//! @author     Ahmed Gazar
//! @date       2024-05-09
//! @version    1.1
//!
//! @brief      GOS2022 Library / 24LC256 driver header.
//! @details    This component provides the interface for the 24LC256 EEPROM chip.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-04-15    Ahmed Gazar     Initial version created.
// 1.1        2024-05-09    Ahmed Gazar     +    Function descriptors added
//                                          +    Error handler interfaces added
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
#ifndef DRV_24LC256_H
#define DRV_24LC256_H
/*
 * Includes
 */
#include <gos.h>
#include <drv_i2c.h>

/*
 * Type definitions
 */
/**
 * 24LC256 descriptor type.
 */
typedef struct
{
    drv_i2cPeriphInstance_t i2cInstance;     //!< I2C instance.
    gos_mutex_t             deviceMutex;     //!< Device mutex.
    u16_t                   deviceAddress;   //!< Device address.
    u32_t                   readMutexTmo;    //!< Read mutex timeout [ms].
    u32_t                   readTriggerTmo;  //!< Read trigger timeout [ms].
    u32_t                   writeMutexTmo;   //!< Write mutex timeout [ms].
    u32_t                   writeTriggerTmo; //!< Write trigger timeout [ms].
    u32_t                   errorFlags;      //!< Error flags of the device [ms].
}drv_24lc256Descriptor_t;

/**
 * @brief   Initializes the device.
 * @details Initializes the device mutex.
 *
 * @param   pDevice : Pointer to the device descriptor structure.
 *
 * @return  Result of initialization.
 *
 * @retval  GOS_SUCCESS : Initialization successful.
 * @retval  GOS_ERROR   : Mutex initialization failed.
 */
gos_result_t drv_24lc256Init (
        void_t* pDevice
        );

/**
 * @brief   Gets the error flags.
 * @details Returns the error flags detected on the device.
 *
 * @param   pDevice     : Pointer to the device descriptor structure.
 * @param   pErrorFlags : Pointer to the variable where the error
 *                        flags shall be stored.
 *
 * @return  Result of flag getting.
 *
 * @retval  GOS_SUCCESS : Successful.
 * @retval  GOS_ERROR   : Device or target variable pointer is NULL.
 */
gos_result_t drv_24lc256GetErrorFlags (
        void_t* pDevice, u32_t* pErrorFlags
        );

/**
 * @brief   Clears the given error flags.
 * @details Clears the given error flags of the device.
 *
 * @param   pDevice    : Pointer to the device descriptor structure.
 * @param   errorFlags : Flags to be cleared.
 *
 * @return  Result of flag clearing.
 *
 * @retval  GOS_SUCCESS : Successful.
 * @retval  GOS_ERROR   : Device pointer is NULL.
 */
gos_result_t drv_24lc256ClearErrorFlags (
        void_t* pDevice, u32_t errorFlags
        );

/**
 * @brief   Reads from the device.
 * @details Reads the given number of bytes to the given buffer starting
 *          at the given address from the given device.
 *
 * @param   pDevice : Pointer to the device descriptor structure.
 *
 * @return  Result of reading.
 *
 * @retval  GOS_SUCCESS : Reading successful.
 * @retval  GOS_ERROR   : NULL pointer check or I2C operation failure.
 */
gos_result_t drv_24lc256Read (
        void_t* pDevice, u16_t address,
        u8_t*   pTarget, u16_t size
        );

/**
 * @brief   Writes to the device.
 * @details Writes the given number of bytes from the given buffer starting
 *          at the given address to the given device.
 *
 * @param   pDevice : Pointer to the device descriptor structure.
 *
 * @return  Result of writing.
 *
 * @retval  GOS_SUCCESS : Writing successful.
 * @retval  GOS_ERROR   : NULL pointer check or I2C operation failure.
 */
gos_result_t drv_24lc256Write (
        void_t* pDevice, u16_t address,
        u8_t*   pData,   u16_t size
        );
#endif
