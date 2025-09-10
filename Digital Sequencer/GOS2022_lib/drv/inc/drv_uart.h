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
//! @date       2025-07-23
//! @version    1.3
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
// 1.2        2025-06-18    Ahmed Gazar     +    System monitoring separate instances added
// 1.3        2025-07-23    Ahmed Gazar     +    drv_uartDiag_t and drv_uartGetDiagData added
//                                          -    drv_uartGetErrorFlags and drv_uartClearErrorFlags
//                                               removed
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
#include <gos.h>
#include <drv_dma.h>

/*
 * Type definitions
 */
/**
 * UART periphery instance enum.
 */
typedef enum
{
    DRV_UART_INSTANCE_1 = 0,                 //!< USART1
    DRV_UART_INSTANCE_2,                     //!< USART2
    DRV_UART_INSTANCE_3,                     //!< USART3
    DRV_UART_INSTANCE_4,                     //!< UART4
    DRV_UART_INSTANCE_5,                     //!< UART5
    DRV_UART_INSTANCE_6,                     //!< USART6
    DRV_UART_NUM_OF_INSTANCES,               //!< Number of UART instances.
    DRV_UART_INSTANCE_INVALID = 0xFF
}drv_uartPeriphInstance_t;

/**
 * UART service instance enum.
 */
typedef enum
{
    DRV_UART_SYSMON_WIRED_INSTANCE = 0,      //!< System monitoring wired instance.
	DRV_UART_SYSMON_WIRELESS_INSTANCE,       //!< System monitoring wireless instance.
    DRV_UART_SHELL_INSTANCE,                 //!< Shell instance.
    DRV_UART_TRACE_INSTANCE                  //!< Trace instance.
}drv_uartServiceInstance_t;

/**
 * UART descriptor type.
 */
typedef struct
{
    drv_uartPeriphInstance_t periphInstance; //!< Periphery instance.
    u32_t                    baudRate;       //!< Baud rate.
    u32_t                    wordLength;     //!< Word length.
    u32_t                    stopBits;       //!< Stop bits.
    u32_t                    parity;         //!< Parity.
    u32_t                    mode;           //!< Mode.
    u32_t                    hwFlowControl;  //!< Hardware flow control.
    u32_t                    overSampling;   //!< Over-sampling.
    drv_dmaDescriptor_t*     dmaConfigRx;    //!< RX DMA configuration.
    drv_dmaDescriptor_t*     dmaConfigTx;    //!< TX DMA configuration.
}drv_uartDescriptor_t;

/**
 * UART service timeout configuration type.
 */
typedef struct
{
    u32_t traceMutexTmo;                     //!< Trace mutex lock timeout.
    u32_t traceTriggerTmo;                   //!< Trace trigger wait timeout.
    u32_t traceUnsafeTransmitTmo;            //!< Trace unsafe transmit timeout.
    u32_t shellRxMutexTmo;                   //!< Shell receive mutex lock timeout.
    u32_t shellRxTriggerTmo;                 //!< Shell receive trigger wait timeout.
    u32_t shellTxMutexTmo;                   //!< Shell transmit mutex lock timeout.
    u32_t shellTxTriggerTmo;                 //!< Shell transmit trigger wait timeout.
    u32_t sysmonRxMutexTmo;                  //!< Sysmon receive mutex lock timeout.
    u32_t sysmonRxTriggerTmo;                //!< Sysmon receive trigger wait timeout.
    u32_t sysmonTxMutexTmo;                  //!< Sysmon transmit mutex lock timeout.
    u32_t sysmonTxTriggerTmo;                //!< Sysmon transmit trigger wait timeout.
}drv_uartServiceTimeoutConfig_t;

/**
 * UART diagnostic data type.
 */
typedef struct __attribute__((packed))
{
	u32_t  globalErrorFlags;                                //!< Global error flags.
	bool_t instanceInitialized [DRV_UART_NUM_OF_INSTANCES]; //!< Instance initialized flags.
	u32_t  instanceErrorFlags  [DRV_UART_NUM_OF_INSTANCES]; //!< Instance error flags.
	u32_t  parityErrorCntr     [DRV_UART_NUM_OF_INSTANCES]; //!< Parity error counters.
	u32_t  noiseErrorCntr      [DRV_UART_NUM_OF_INSTANCES]; //!< Noise error counters.
	u32_t  overrunErrorCntr    [DRV_UART_NUM_OF_INSTANCES]; //!< Overrun error counters.
	u32_t  framingErrorCntr    [DRV_UART_NUM_OF_INSTANCES]; //!< Framing error counters.
}drv_uartDiag_t;

/*
 * Function prototypes
 */
/**
 * @brief     Initializes the registered UART peripheries.
 * @details   Loops through the UART configuration array and initializes peripheries
 *            defined by the user (externally).
 *
 * @return    Result of initialization.
 *
 * @retval    #GOS_SUCCESS Initialization successful.
 * @retval    #GOS_ERROR   Configuration array is NULL or some peripheries
 *                         could not be initialized.
 */
gos_result_t drv_uartInit (
        void_t
        );

/**
 * @brief     Initializes the requested UART instance.
 * @details   Calls the HAL level initializer, initializes the RX and TX
 *            mutexes and triggers.
 *
 * @param[in] uartInstanceIndex Instance index of UART periphery.
 *
 * @return    Result of initialization.
 *
 * @retval    #GOS_SUCCESS Initialization successful.
 * @retval    #GOS_ERROR   One of the initialization steps failed.
 */
gos_result_t drv_uartInitInstance (
        u8_t uartInstanceIndex
        );

/**
 * @brief     De-initializes the requested UART instance.
 * @details   Calls the HAL level de-initializer.
 *
 * @param[in] uartInstanceIndex Instance index of UART periphery.
 *
 * @return    Result of de-initialization.
 *
 * @retval    #GOS_SUCCESS De-initialization successful.
 * @retval    #GOS_ERROR   Wrong instance or HAL error.
 */
gos_result_t drv_uartDeInitInstance (
        u8_t uartInstanceIndex
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
gos_result_t drv_uartGetDiagData (
		drv_uartDiag_t* pDiag
		);

// TODO
gos_result_t drv_uartTransmitBlocking (
        drv_uartPeriphInstance_t instance, u8_t* message,
        u16_t                    size,     u32_t timeout
        );

// TODO
gos_result_t drv_uartReceiveBlocking (
        drv_uartPeriphInstance_t instance, u8_t* pData,
        u16_t                    size,     u32_t timeout
        );

// TODO
gos_result_t drv_uartTransmitDMA (
        drv_uartPeriphInstance_t instance,  u8_t* message,
        u16_t                    size,      u32_t mutexTmo,
        u32_t                    triggerTmo
        );

// TODO
gos_result_t drv_uartReceiveDMA (
        drv_uartPeriphInstance_t instance,  u8_t* message,
        u16_t                    size,      u32_t mutexTmo,
        u32_t                    triggerTmo
        );

// TODO
gos_result_t drv_uartTransmitIT (
        drv_uartPeriphInstance_t instance,  u8_t* message,
        u16_t                    size,      u32_t mutexTmo,
        u32_t                    triggerTmo
        );

// TODO
gos_result_t drv_uartReceiveIT (
        drv_uartPeriphInstance_t instance,  u8_t* message,
        u16_t                    size,      u32_t mutexTmo,
        u32_t                    triggerTmo
        );

#endif
