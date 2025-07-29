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
//! @file       drv_i2c.h
//! @author     Ahmed Gazar
//! @date       2025-07-27
//! @version    1.1
//!
//! @brief      GOS2022 Library / I2C driver header.
//! @details    This component provides access to the I2C peripheries.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-03-15    Ahmed Gazar     Initial version created.
// 1.1        2025-07-27    Ahmed Gazar     +    Diagnostics implemented
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
#ifndef DRV_I2C_H
#define DRV_I2C_H

/*
 * Includes
 */
#include <gos.h>
#include <drv_dma.h>

/*
 * Type definitions
 */
/**
 * I2C periphery instance enum.
 */
typedef enum
{
    DRV_I2C_INSTANCE_1 = 0,                 //!< I2C1
    DRV_I2C_INSTANCE_2,                     //!< I2C2
    DRV_I2C_INSTANCE_3,                     //!< I2C3
    DRV_I2C_NUM_OF_INSTANCES                //!< Number of I2C instances.
}drv_i2cPeriphInstance_t;

/**
 * I2C descriptor type.
 */
typedef struct
{
    drv_i2cPeriphInstance_t periphInstance;  //!< Periphery instance.
    u32_t                   clockSpeed;      //!< Clock speed.
    u32_t                   dutyCycle;       //!< Duty cycle.
    u32_t                   ownAddress1;     //!< Own address.
    u32_t                   addressingMode;  //!< Addressing mode.
    u32_t                   dualAddressMode; //!< Dual address mode.
    u32_t                   ownAddress2;     //!< Own address 2.
    u32_t                   generalCallMode; //!< General call mode.
    u32_t                   noStretchMode;   //!< No stretch mode.
    drv_dmaDescriptor_t*    dmaConfigRx;     //!< RX DMA configuration.
    drv_dmaDescriptor_t*    dmaConfigTx;     //!< TX DMA configuration.
}drv_i2cDescriptor_t;

/**
 * I2C diagnostic data type.
 */
typedef struct __attribute__((packed))
{
	u32_t  globalErrorFlags;                               //!< Global error flags.
	bool_t instanceInitialized [DRV_I2C_NUM_OF_INSTANCES]; //!< Instance initialized flags.
	u32_t  instanceErrorFlags  [DRV_I2C_NUM_OF_INSTANCES]; //!< Instance error flags.
}drv_i2cDiag_t;

/*
 * Function prototypes
 */
/**
 * @brief     Initializes the registered I2C peripheries.
 * @details   Loops through the I2C configuration array and initializes peripheries
 *            defined by the user (externally).
 *
 * @return    Result of initialization.
 *
 * @retval    #GOS_SUCCESS Initialization successful.
 * @retval    #GOS_ERROR   Configuration array is NULL or some peripheries
 *                         could not be initialized.
 */
gos_result_t drv_i2cInit (
        void_t
        );

/**
 * @brief     Initializes the requested I2C instance.
 * @details   Calls the HAL level initializer, initializes the
 *            mutexes and triggers.
 *
 * @param[in] i2cInstanceIndex Instance index of I2C periphery.
 *
 * @return    Result of initialization.
 *
 * @retval    #GOS_SUCCESS Initialization successful.
 * @retval    #GOS_ERROR   One of the initialization steps failed.
 */
gos_result_t drv_i2cInitInstance (
		u8_t i2cInstanceIndex
        );

/**
 * @brief     De-initializes the requested I2C instance.
 * @details   Calls the HAL level de-initializer.
 *
 * @param[in] i2cInstanceIndex Instance index of I2C periphery.
 *
 * @return    Result of de-initialization.
 *
 * @retval    #GOS_SUCCESS De-initialization successful.
 * @retval    #GOS_ERROR   Wrong instance or HAL error.
 */
gos_result_t drv_i2cDeInitInstance (
		u8_t i2cInstanceIndex
		);

/**
 * @brief      Retrieves the diagnostic data of the driver.
 * @details    Copies the diagnostic data to the target structure.
 *
 * @param[out] pDiag Pointer to a diagnostic data structure to store the
 *             diagnostic data in.
 *
 * @return     Result of data retrieval.
 *
 * @retval     #GOS_SUCCESS Diagnostic data copied successfully.
 * @retval     #GOS_ERROR   NULL pointer error.
 */
gos_result_t drv_i2cGetDiagData (
		drv_i2cDiag_t* pDiag
		);

// TODO
gos_result_t drv_i2cMemWrite (
        drv_i2cPeriphInstance_t instance, u16_t address, u16_t memAddress, u16_t memAddressSize,
        u8_t*                   data,     u16_t size,    u32_t mutexTmo,   u32_t triggerTmo
        );

// TODO
gos_result_t drv_i2cMemRead (
        drv_i2cPeriphInstance_t instance, u16_t address, u16_t memAddress, u16_t memAddressSize,
        u8_t*                   data,     u16_t size,    u32_t mutexTmo,   u32_t triggerTmo
        );

// TODO
gos_result_t drv_i2cTransmitBlocking (
        drv_i2cPeriphInstance_t instance, u16_t address,  u8_t* pData,
        u16_t                   size,     u32_t mutexTmo, u32_t transmitTmo
        );

// TODO
gos_result_t drv_i2cReceiveBlocking (
        drv_i2cPeriphInstance_t instance, u16_t address,  u8_t* pBuffer,
        u16_t                   size,     u32_t mutexTmo, u32_t receiveTmo
        );

// TODO
gos_result_t drv_i2cTransmitIT (
        drv_i2cPeriphInstance_t instance, u16_t address,  u8_t* pData,
        u16_t                   size,     u32_t mutexTmo, u32_t triggerTmo
        );

// TODO
gos_result_t drv_i2cReceiveIT (
        drv_i2cPeriphInstance_t instance, u16_t address,  u8_t* pBuffer,
        u16_t                   size,     u32_t mutexTmo, u32_t triggerTmo
        );

// TODO
gos_result_t drv_i2cTransmitDMA (
        drv_i2cPeriphInstance_t instance, u16_t address,  u8_t* pData,
        u16_t                   size,     u32_t mutexTmo, u32_t triggerTmo
        );

// TODO
gos_result_t drv_i2cReceiveDMA (
        drv_i2cPeriphInstance_t instance, u16_t address,  u8_t* pBuffer,
        u16_t                   size,     u32_t mutexTmo, u32_t triggerTmo
        );
#endif
