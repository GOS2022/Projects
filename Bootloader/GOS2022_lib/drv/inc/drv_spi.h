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
//! @file       drv_spi.h
//! @author     Ahmed Gazar
//! @date       2025-07-24
//! @version    1.1
//!
//! @brief      GOS2022 Library / SPI driver header.
//! @details    This component provides access to the SPI peripheries.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-03-19    Ahmed Gazar     Initial version created.
// 1.1        2025-07-24    Ahmed Gazar     +    Diagnostics implemented
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
#ifndef DRV_SPI_H
#define DRV_SPI_H

/*
 * Includes
 */
#include <gos.h>
#include <drv_dma.h>

/*
 * Type definitions
 */
/**
 * SPI periphery instance enum.
 */
typedef enum
{
    DRV_SPI_INSTANCE_1 = 0,                    //!< SPI1
    DRV_SPI_INSTANCE_2,                        //!< SPI2
    DRV_SPI_INSTANCE_3,                        //!< SPI3
    DRV_SPI_INSTANCE_4,                        //!< SPI4
    DRV_SPI_NUM_OF_INSTANCES                   //!< Number of SPI instances.
}drv_spiPeriphInstance_t;

/**
 * SPI descriptor type.
 */
typedef struct
{
    drv_spiPeriphInstance_t periphInstance;    //!< Periphery instance.
    u32_t                   mode;              //!< Mode.
    u32_t                   direction;         //!< Direction.
    u32_t                   dataSize;          //!< Data size.
    u32_t                   clkPolarity;       //!< Clock polarity.
    u32_t                   clkPhase;          //!< Clock phase.
    u32_t                   nss;               //!< NSS.
    u32_t                   baudRatePrescaler; //!< Baud rate prescaler.
    u32_t                   firstBit;          //!< First bit.
    u32_t                   tiMode;            //!< TI mode.
    u32_t                   crcCalculation;    //!< CRC calculation.
    u32_t                   crcPolynomial;     //!< CRC polynomial.
    drv_dmaDescriptor_t*    dmaConfigRx;       //!< RX DMA configuration.
    drv_dmaDescriptor_t*    dmaConfigTx;       //!< TX DMA configuration.
}drv_spiDescriptor_t;

/**
 * SPI diagnostic data type.
 */
typedef struct __attribute__((packed))
{
	u32_t  globalErrorFlags;                               //!< Global error flags.
	bool_t instanceInitialized [DRV_SPI_NUM_OF_INSTANCES]; //!< Instance initialized flags.
	u32_t  instanceErrorFlags  [DRV_SPI_NUM_OF_INSTANCES]; //!< Instance error flags.
}drv_spiDiag_t;

/*
 * Function prototypes
 */
/**
 * @brief     Initializes the registered SPI peripheries.
 * @details   Loops through the SPI configuration array and initializes peripheries
 *            defined by the user (externally).
 *
 * @return    Result of initialization.
 *
 * @retval    #GOS_SUCCESS Initialization successful.
 * @retval    #GOS_ERROR   Configuration array is NULL or some peripheries
 *                         could not be initialized.
 */
gos_result_t drv_spiInit (
        void_t
        );

/**
 * @brief     Initializes the requested SPI instance.
 * @details   Calls the HAL level initializer, initializes the
 *            mutexes and triggers.
 *
 * @param[in] spiInstanceIndex Instance index of SPI periphery.
 *
 * @return    Result of initialization.
 *
 * @retval    #GOS_SUCCESS Initialization successful.
 * @retval    #GOS_ERROR   One of the initialization steps failed.
 */
gos_result_t drv_spiInitInstance (
        u8_t spiInstanceIndex
        );

/**
 * @brief     De-initializes the requested SPI instance.
 * @details   Calls the HAL level de-initializer.
 *
 * @param[in] spiInstanceIndex Instance index of SPI periphery.
 *
 * @return    Result of de-initialization.
 *
 * @retval    #GOS_SUCCESS De-initialization successful.
 * @retval    #GOS_ERROR   Wrong instance or HAL error.
 */
gos_result_t drv_spiDeInitInstance (
		u8_t spiInstanceIndex
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
gos_result_t drv_spiGetDiagData (
		drv_spiDiag_t* pDiag
		);

// TODO
gos_result_t drv_spiTransmitBlocking (
        drv_spiPeriphInstance_t instance, u8_t* pData,
        u16_t                   size,     u32_t mutexTmo, u32_t transmitTmo
        );

// TODO
gos_result_t drv_spiReceiveBlocking (
        drv_spiPeriphInstance_t instance, u8_t* pBuffer,
        u16_t                   size,     u32_t mutexTmo, u32_t receiveTmo
        );

// TODO
gos_result_t drv_spiTransmitIT (
        drv_spiPeriphInstance_t instance, u8_t* pData,
        u16_t                   size,     u32_t mutexTmo, u32_t triggerTmo
        );

// TODO
gos_result_t drv_spiReceiveIT (
        drv_spiPeriphInstance_t instance, u8_t* pBuffer,
        u16_t                   size,     u32_t mutexTmo, u32_t triggerTmo
        );

// TODO
gos_result_t drv_spiTransmitDMA (
        drv_spiPeriphInstance_t instance, u8_t* pData,
        u16_t                   size,     u32_t mutexTmo, u32_t triggerTmo
        );

// TODO
gos_result_t drv_spiReceiveDMA (
        drv_spiPeriphInstance_t instance, u8_t* pBuffer,
        u16_t                   size,     u32_t mutexTmo, u32_t triggerTmo
        );

// TODO
gos_result_t drv_spiTransmitReceiveBlocking (
        drv_spiPeriphInstance_t instance, u8_t* pTxData,  u8_t* pRxData,
        u16_t                   size,     u32_t mutexTmo, u32_t transmitReceiveTmo
        );

// TODO
gos_result_t drv_spiTransmitReceiveIT (
        drv_spiPeriphInstance_t instance, u8_t* pTxData,  u8_t* pRxData,
        u16_t                   size,     u32_t mutexTmo, u32_t triggerTmo
        );

// TODO
gos_result_t drv_spiTransmitReceiveDMA (
        drv_spiPeriphInstance_t instance, u8_t* pTxData,  u8_t* pRxData,
        u16_t                   size,     u32_t mutexTmo, u32_t triggerTmo
        );
#endif
