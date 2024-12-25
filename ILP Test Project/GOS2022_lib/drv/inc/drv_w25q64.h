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
//! @file       drv_w25q64.h
//! @author     Ahmed Gazar
//! @date       2024-12-24
//! @version    1.0
//!
//! @brief      GOS2022 Library / W25Q64 driver header.
//! @details    This component provides the interface for the W25Q64 EEPROM chip.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-12-24    Ahmed Gazar     Initial version created.
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
#ifndef DRV_W25Q64_H
#define DRV_W25Q64_H
/*
 * Includes
 */
#include <gos.h>
#include <drv_spi.h>

/*
 * Macros
 */
/**
 * Page size in bytes.
 */
#define W25Q64_PAGE_SIZE  ( 256u )

/**
 * Number of pages.
 */
#define W25Q64_PAGE_NUM   ( 32768u )

/*
 * Type definitions
 */
/**
 * W25Q64 descriptor type.
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
    u32_t                   errorFlags;      //!< Error flags of the device [ms].
}drv_w25q64Descriptor_t;

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
gos_result_t drv_w25q64Init (
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
gos_result_t drv_w25q64GetErrorFlags (
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
gos_result_t drv_w25q64ClearErrorFlags (
        void_t* pDevice, u32_t errorFlags
        );

// TODO
void_t drv_w25q64Reset (void_t* pDevice);

// TODO
void_t drv_w25q64ReadId (void_t* pDevice, u32_t* pId);


/**
 * @brief   Reads from the device.
 * @details Reads the given number of bytes to the given buffer starting
 *          at the given address from the given device.
 *
 * @param   pDevice : Pointer to the device descriptor structure.
 * @param   address : Address to start reading from.
 * @param   pData   : Target buffer.
 * @param   size    : Number of bytes to read.
 *
 * @return  Result of reading.
 *
 * @retval  GOS_SUCCESS : Reading successful.
 * @retval  GOS_ERROR   : NULL pointer check or I2C operation failure.
 */
void_t drv_w25q64ReadData (void_t* pDevice, u32_t address, u8_t* pData, u32_t size);

/**
 * @brief   Writes to the device.
 * @details Writes the given number of bytes from the given buffer starting
 *          at the given address to the given device.
 *
 * @param   pDevice : Pointer to the device descriptor structure.
 * @param   address : Address to start writing at.
 * @param   pData   : Data buffer.
 * @param   size    : Number of bytes to write.
 *
 * @return  Result of writing.
 *
 * @retval  GOS_SUCCESS : Writing successful.
 * @retval  GOS_ERROR   : NULL pointer check or I2C operation failure.
 */
void_t drv_w25q64WriteData (void_t* pDevice, u32_t address, /*u32_t startPage, u8_t offset,*/ u8_t* pData, u32_t size);

#endif
