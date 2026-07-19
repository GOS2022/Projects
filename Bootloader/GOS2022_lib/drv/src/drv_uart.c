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
//! @date       2026-07-17
//! @version    1.5
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
// 1.3        2025-07-22    Ahmed Gazar     *    Error reporting replaced by diagnostics
// 1.4        2026-07-16    Ahmed Gazar     *    RX IT rework, major fixes in logic
// 1.5        2026-07-17    Ahmed Gazar     *    Major fixes in driver logic
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
#include <drv_error.h>
#include <drv_uart.h>
#include <string.h>
#include "stm32f4xx_hal.h"

/*
 * Macros
 */
/**
 * UART RX buffer size.
 */
#define DRV_UART_RX_BUFFER_SIZE (512u)

/*
 * Type definitions
 */
/**
 * RX modes.
 */
typedef enum
{
	DRV_UART_RX_MODE_NONE = 0u,
	DRV_UART_RX_MODE_IT   = 1u,
	DRV_UART_RX_MODE_DMA  = 3u
}drv_uart_rx_mode_t;

/*
 * Static variables
 */
/**
 * UART instance lookup table.
 */
GOS_STATIC USART_TypeDef* uartInstanceLut [] =
{
    [DRV_UART_INSTANCE_1] = USART1,
    [DRV_UART_INSTANCE_2] = USART2,
    [DRV_UART_INSTANCE_3] = USART3,
    [DRV_UART_INSTANCE_4] = UART4,
    [DRV_UART_INSTANCE_5] = UART5,
    [DRV_UART_INSTANCE_6] = USART6,
};

/**
 * UART handles.
 */
GOS_STATIC UART_HandleTypeDef huarts              [DRV_UART_NUM_OF_INSTANCES];

/**
 * UART RX mutexes.
 */
GOS_STATIC gos_mutex_t        uartRxMutexes       [DRV_UART_NUM_OF_INSTANCES];

/**
 * UART TX mutexes.
 */
GOS_STATIC gos_mutex_t        uartTxMutexes       [DRV_UART_NUM_OF_INSTANCES];

/**
 * UART RX ready triggers.
 */
GOS_STATIC gos_trigger_t      uartRxReadyTriggers [DRV_UART_NUM_OF_INSTANCES];

/**
 * UART TX ready triggers.
 */
GOS_STATIC gos_trigger_t      uartTxReadyTriggers [DRV_UART_NUM_OF_INSTANCES];

/**
 * UART diagnostics.
 */
GOS_STATIC drv_uartDiag_t     uartDiag = {0};

/**
 * UART RX operation result tracking.
 */
GOS_STATIC volatile gos_result_t uartRxOpResult[DRV_UART_NUM_OF_INSTANCES];

/**
 * UART TX operation result tracking.
 */
GOS_STATIC volatile gos_result_t uartTxOpResult[DRV_UART_NUM_OF_INSTANCES];

/**
 * UART RX active operation flag.
 */
GOS_STATIC volatile bool_t uartRxOpActive[DRV_UART_NUM_OF_INSTANCES];

/**
 * UART TX active operation flag.
 */
GOS_STATIC volatile bool_t uartTxOpActive[DRV_UART_NUM_OF_INSTANCES];

/**
 * UART RX buffer.
 */
GOS_STATIC u8_t  uartRxBuffer[DRV_UART_NUM_OF_INSTANCES][DRV_UART_RX_BUFFER_SIZE];

/**
 * UART RX buffer head array for ring buffer.
 */
GOS_STATIC volatile u16_t uartRxBufferHead[DRV_UART_NUM_OF_INSTANCES];

/**
 * UART RX buffer tail array for ring buffer.
 */
GOS_STATIC volatile u16_t uartRxBufferTail[DRV_UART_NUM_OF_INSTANCES];

/**
 * UART RX buffer counters.
 */
GOS_STATIC volatile u16_t uartRxBufferCount[DRV_UART_NUM_OF_INSTANCES];

/**
 * UART RX byte array.
 */
GOS_STATIC u8_t  uartRxOneByte [DRV_UART_NUM_OF_INSTANCES];

/* Track current RX mode per instance */
GOS_STATIC volatile drv_uart_rx_mode_t uartRxMode [DRV_UART_NUM_OF_INSTANCES];

/*
 * External variables
 */
/**
 * UART configuration array (shall be defined by user).
 */
GOS_EXTERN GOS_CONST drv_uartDescriptor_t     uartConfig [];

/**
 * UART configuration array size (shall be defined by user).
 */
GOS_EXTERN u32_t                              uartConfigSize;

/**
 * UART service configuration (shall be defined by user).
 */
GOS_EXTERN GOS_CONST drv_uartPeriphInstance_t uartServiceConfig [];

/**
 * UART service timeout configuration.
 */
GOS_EXTERN drv_uartServiceTimeoutConfig_t     uartServiceTmoConfig;

/**
 * TODO
 */
GOS_STATIC GOS_INLINE void_t drv_uartRxBufferInit(drv_uartPeriphInstance_t instance)
{
	/*
	 * Function code.
	 */
    uartRxBufferHead[instance]  = 0u;
    uartRxBufferTail[instance]  = 0u;
    uartRxBufferCount[instance] = 0u;
    uartRxOpActive[instance]    = GOS_FALSE;
    uartRxMode[instance]        = DRV_UART_RX_MODE_NONE;
}

/**
 * TODO
 */
GOS_STATIC GOS_INLINE bool_t drv_uartRxBufferIsFull (drv_uartPeriphInstance_t instance)
{
	/*
	 * Function code.
	 */
    return (uartRxBufferCount[instance] >= DRV_UART_RX_BUFFER_SIZE) ? GOS_TRUE : GOS_FALSE;
}

/**
 * TODO
 */
GOS_STATIC GOS_INLINE bool_t drv_uartRxBufferIsEmpty (drv_uartPeriphInstance_t instance)
{
    return (uartRxBufferCount[instance] == 0u) ? GOS_TRUE : GOS_FALSE;
}

/**
 * TODO
 */
GOS_STATIC GOS_INLINE void_t drv_uartRxBufferPush (drv_uartPeriphInstance_t instance, u8_t byte)
{
	/*
	 * Function code.
	 */
    if (drv_uartRxBufferIsFull(instance) == GOS_FALSE)
    {
        uartRxBuffer[instance][uartRxBufferHead[instance]] = byte;
        uartRxBufferHead[instance] = (uartRxBufferHead[instance] + 1u) % DRV_UART_RX_BUFFER_SIZE;
        uartRxBufferCount[instance]++;
    }
    else
    {
        /* Buffer full: overwrite oldest byte to keep most recent data and record overflow */
        uartDiag.overrunErrorCntr[instance]++; /* count receive overflows */
        /* advance tail to drop oldest byte */
        uartRxBufferTail[instance] = (uartRxBufferTail[instance] + 1u) % DRV_UART_RX_BUFFER_SIZE;
        /* store new byte at head and advance head */
        uartRxBuffer[instance][uartRxBufferHead[instance]] = byte;
        uartRxBufferHead[instance] = (uartRxBufferHead[instance] + 1u) % DRV_UART_RX_BUFFER_SIZE;
        /* uartRxBufferCount stays at capacity */
    }
}

/**
 * TODO
 */
GOS_STATIC GOS_INLINE u8_t drv_uartRxBufferPop (drv_uartPeriphInstance_t instance)
{
	/*
	 * Local variables.
	 */
    u8_t byte = uartRxBuffer[instance][uartRxBufferTail[instance]];

	/*
	 * Function code.
	 */
    uartRxBufferTail[instance] = (uartRxBufferTail[instance] + 1u) % DRV_UART_RX_BUFFER_SIZE;
    uartRxBufferCount[instance]--;
    return byte;
}

/**
 * TODO
 */
GOS_STATIC GOS_INLINE u16_t drv_uartRxBufferAvailable (drv_uartPeriphInstance_t instance)
{
	/*
	 * Function code.
	 */
    return uartRxBufferCount[instance];
}

/**
 * TODO
 */
GOS_STATIC GOS_INLINE gos_result_t drv_uartStartRxBackground (drv_uartPeriphInstance_t instance)
{
	/*
	 * Function code.
	 */
    if (uartRxOpActive[instance] == GOS_FALSE)
    {
        if (HAL_UART_Receive_IT(&huarts[instance], &uartRxOneByte[instance], 1) == HAL_OK)
        {
            uartRxOpActive[instance] = GOS_TRUE;
            uartRxMode[instance] = DRV_UART_RX_MODE_IT;
            return GOS_SUCCESS;
        }

        DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_IT_HAL);
        uartRxOpActive[instance] = GOS_FALSE;
        uartRxMode[instance] = DRV_UART_RX_MODE_NONE;
        return GOS_ERROR;
    }

    return GOS_SUCCESS;
}

/**
 * TODO
 */
GOS_STATIC GOS_INLINE void_t drv_uartStopRxBackground (drv_uartPeriphInstance_t instance)
{
    if (uartRxOpActive[instance] == GOS_TRUE)
    {
        /* Synchronous stop avoids abort-IT race with immediate next RX mode. */
        (void_t)HAL_UART_AbortReceive(&huarts[instance]);
    }

    uartRxOpActive[instance] = GOS_FALSE;
    uartRxMode[instance]     = DRV_UART_RX_MODE_NONE;
}

/*
 * Function: drv_uartInit
 */
gos_result_t drv_uartInit (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t uartDriverInitResult = GOS_SUCCESS;
    u8_t         uartIdx              = 0u;

    /*
     * Function code.
     */
    if (uartConfig != NULL)
    {
        for (uartIdx = 0u; uartIdx < uartConfigSize / sizeof(drv_uartDescriptor_t); uartIdx++)
        {
            GOS_CONCAT_RESULT(uartDriverInitResult, drv_uartInitInstance(uartIdx));
        }
    }
    else
    {
        // Configuration array is NULL pointer.
        DRV_ERROR_SET(uartDiag.globalErrorFlags, DRV_ERROR_UART_CFG_ARRAY_NULL);
        uartDriverInitResult = GOS_ERROR;
    }

    return uartDriverInitResult;
}

/*
 * Function: drv_uartInitInstance
 */
gos_result_t drv_uartInitInstance (u8_t uartInstanceIndex)
{
    /*
     * Local variables.
     */
    gos_result_t             uartInitResult = GOS_ERROR;
    drv_uartPeriphInstance_t instance       = 0u;

    /*
     * Function code.
     */
    if (uartConfig != NULL)
    {
        if (uartInstanceIndex < (uartConfigSize / sizeof(drv_uartDescriptor_t)))
        {
            instance = uartConfig[uartInstanceIndex].periphInstance;

            huarts[instance].Instance          = uartInstanceLut[instance];
            huarts[instance].Init.BaudRate     = uartConfig[uartInstanceIndex].baudRate;
            huarts[instance].Init.WordLength   = uartConfig[uartInstanceIndex].wordLength;
            huarts[instance].Init.StopBits     = uartConfig[uartInstanceIndex].stopBits;
            huarts[instance].Init.Parity       = uartConfig[uartInstanceIndex].parity;
            huarts[instance].Init.Mode         = uartConfig[uartInstanceIndex].mode;
            huarts[instance].Init.HwFlowCtl    = uartConfig[uartInstanceIndex].hwFlowControl;
            huarts[instance].Init.OverSampling = uartConfig[uartInstanceIndex].overSampling;

            drv_uartRxBufferInit(instance);
            if (HAL_UART_Init   (&huarts[instance])              == HAL_OK      &&
                gos_mutexInit   (&uartRxMutexes[instance])       == GOS_SUCCESS &&
                gos_mutexInit   (&uartTxMutexes[instance])       == GOS_SUCCESS &&
                gos_triggerInit (&uartRxReadyTriggers[instance]) == GOS_SUCCESS &&
                gos_triggerInit (&uartTxReadyTriggers[instance]) == GOS_SUCCESS &&
                gos_triggerReset(&uartRxReadyTriggers[instance]) == GOS_SUCCESS &&
                gos_triggerReset(&uartTxReadyTriggers[instance]) == GOS_SUCCESS
                )
            {
                if (drv_uartStartRxBackground(instance) == GOS_SUCCESS)
                {
                    uartInitResult = GOS_SUCCESS;
                    uartDiag.instanceInitialized[instance] = GOS_TRUE;
                }
                else
                {
                    DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_IT_HAL);
                }
            }
            else
            {
                // Init error.
                DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_INSTANCE_INIT);
            }
        }
        else
        {
            // Index is out of array boundary.
            DRV_ERROR_SET(uartDiag.globalErrorFlags, DRV_ERROR_UART_INDEX_OUT_OF_BOUND);
        }
    }
    else
    {
        // Configuration is NULL.
        DRV_ERROR_SET(uartDiag.globalErrorFlags, DRV_ERROR_UART_CFG_ARRAY_NULL);
    }

    return uartInitResult;
}

/*
 * Function: drv_uartDeInitInstance
 */
gos_result_t drv_uartDeInitInstance (u8_t uartInstanceIndex)
{
    /*
     * Local variables.
     */
    gos_result_t             uartDeInitResult = GOS_ERROR;
    drv_uartPeriphInstance_t instance         = 0u;

    /*
     * Function code.
     */
    if (uartConfig != NULL)
    {
        if (uartInstanceIndex < (uartConfigSize / sizeof(drv_uartDescriptor_t)))
        {
            instance = uartConfig[uartInstanceIndex].periphInstance;

            if (HAL_UART_DeInit(&huarts[instance]) == HAL_OK)
            {
                uartDeInitResult = GOS_SUCCESS;
            }
            else
            {
                // De-init error.
                DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_INSTANCE_DEINIT);
            }
        }
        else
        {
            // Index is out of array boundary.
            DRV_ERROR_SET(uartDiag.globalErrorFlags, DRV_ERROR_UART_INDEX_OUT_OF_BOUND);
        }
    }
    else
    {
        // Configuration is NULL.
        DRV_ERROR_SET(uartDiag.globalErrorFlags, DRV_ERROR_UART_CFG_ARRAY_NULL);
    }

    return uartDeInitResult;
}

/*
 * Function: drv_uartGetDiagData
 */
gos_result_t drv_uartGetDiagData (drv_uartDiag_t* pDiag)
{
    /*
     * Local variables.
     */
    gos_result_t uartGetDiagResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (pDiag != NULL)
    {
    	(void_t) memcpy((void_t*)pDiag, (void_t*)&uartDiag, sizeof(drv_uartDiag_t));
    	uartGetDiagResult = GOS_SUCCESS;
    }
    else
    {
        // NULL pointer.
    }

    return uartGetDiagResult;
}

/*
 * Function: drv_uartTransmitBlocking
 */
GOS_INLINE gos_result_t drv_uartTransmitBlocking (
        drv_uartPeriphInstance_t instance, u8_t* message,
        u16_t                    size,     u32_t timeout
        )
{
    /*
     * Local variables.
     */
    gos_result_t uartTransmitResult = GOS_ERROR;
    bool_t       lockTaken          = GOS_FALSE;

    /*
     * Function code.
     */
    if ((instance >= DRV_UART_NUM_OF_INSTANCES) || (message == NULL) || (size == 0u))
    {
        return GOS_ERROR;
    }

    if (gos_mutexLock(&uartTxMutexes[instance], timeout) == GOS_SUCCESS)
    {
        lockTaken = GOS_TRUE;

        //GOS_DISABLE_SCHED
        /* Do NOT abort whole UART: it stops background RX. */
        if (HAL_UART_Transmit(&huarts[instance], message, size, timeout) == HAL_OK)
        {
            uartTransmitResult = GOS_SUCCESS;
        }
        else
        {
            DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_TX_BLOCKING);
        }
        //GOS_ENABLE_SCHED
    }

    if (lockTaken == GOS_TRUE)
    {
        (void_t)gos_mutexUnlock(&uartTxMutexes[instance]);
    }

    return uartTransmitResult;
}

/*
 * Function: drv_uartReceiveBlocking
 */
GOS_INLINE gos_result_t drv_uartReceiveBlocking (
        drv_uartPeriphInstance_t instance, u8_t* pData,
        u16_t                    size,     u32_t timeout
        )
{
    gos_result_t uartReceiveResult = GOS_ERROR;

    if (gos_mutexLock(&uartRxMutexes[instance], timeout) == GOS_SUCCESS)
    {
        drv_uartStopRxBackground(instance);

        GOS_DISABLE_SCHED

        if (HAL_UART_Abort  (&huarts[instance])                       == HAL_OK &&
            HAL_UART_Receive(&huarts[instance], pData, size, timeout) == HAL_OK)
        {
            uartReceiveResult = GOS_SUCCESS;
        }
        else
        {
            DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_BLOCKING);
        }
        GOS_ENABLE_SCHED

        (void_t) gos_mutexUnlock(&uartRxMutexes[instance]);

        /* restart background IT RX for continuous reception use-cases */
        (void_t) drv_uartStartRxBackground(instance);
    }
    else
    {
        DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_BLOCKING);
    }

    return uartReceiveResult;
}

/*
 * Function: drv_uartTransmitDMA
 */
GOS_INLINE gos_result_t drv_uartTransmitDMA (
        drv_uartPeriphInstance_t instance, u8_t* message,
        u16_t                    size,     u32_t mutexTmo,
        u32_t                    triggerTmo
        )
{
    gos_result_t uartTransmitResult = GOS_ERROR;
    bool_t       lockTaken          = GOS_FALSE;

    if (gos_mutexLock(&uartTxMutexes[instance], mutexTmo) == GOS_SUCCESS)
    {
        lockTaken = GOS_TRUE;

        if (triggerTmo > 0u)
        {
            (void_t) gos_triggerReset(&uartTxReadyTriggers[instance]);
            uartTxOpResult[instance] = GOS_ERROR;
            uartTxOpActive[instance] = GOS_TRUE;
        }

        if (HAL_UART_Transmit_DMA(&huarts[instance], message, size) == HAL_OK)
        {
            DRV_ERROR_CLEAR(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_TX_DMA_HAL);

            if (triggerTmo > 0u)
            {
                if (gos_triggerWait(&uartTxReadyTriggers[instance], 1, triggerTmo) == GOS_SUCCESS &&
                    gos_triggerReset(&uartTxReadyTriggers[instance])               == GOS_SUCCESS &&
                    uartTxOpResult[instance]                                       == GOS_SUCCESS)
                {
                    uartTransmitResult = GOS_SUCCESS;
                    DRV_ERROR_CLEAR(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_TX_DMA_TRIG);
                }
                else
                {
                    (void_t) HAL_UART_Abort_IT(&huarts[instance]);
                    DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_TX_DMA_TRIG);
                }
            }
            else
            {
                uartTransmitResult = GOS_SUCCESS;
            }
        }
        else
        {
            (void_t) HAL_UART_Abort_IT(&huarts[instance]);
            DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_TX_DMA_HAL);
        }

        uartTxOpActive[instance] = GOS_FALSE;
    }
    else
    {
        DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_TX_DMA_MUTEX);
    }

    if (lockTaken)
    {
        (void_t) gos_mutexUnlock(&uartTxMutexes[instance]);
    }

    return uartTransmitResult;
}

/*
 * Function: drv_uartReceiveDMA
 */
GOS_INLINE gos_result_t drv_uartReceiveDMA (
        drv_uartPeriphInstance_t instance, u8_t* message,
        u16_t                    size,     u32_t mutexTmo,
        u32_t                    triggerTmo
        )
{
    gos_result_t uartReceiveResult = GOS_ERROR;
    bool_t       lockTaken         = GOS_FALSE;

    if (gos_mutexLock(&uartRxMutexes[instance], mutexTmo) == GOS_SUCCESS)
    {
        lockTaken = GOS_TRUE;

        drv_uartStopRxBackground(instance);

        /* mark DMA mode and active - callback will signal completion */
        uartRxMode[instance]    = DRV_UART_RX_MODE_DMA;
        uartRxOpActive[instance] = GOS_TRUE;

        if (HAL_UART_Receive_DMA(&huarts[instance], message, size) == HAL_OK)
        {
            DRV_ERROR_CLEAR(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_DMA_HAL);

            if (triggerTmo > 0u)
            {
                if (gos_triggerWait(&uartRxReadyTriggers[instance], 1, triggerTmo) == GOS_SUCCESS &&
                    gos_triggerReset(&uartRxReadyTriggers[instance])               == GOS_SUCCESS &&
                    uartRxOpResult[instance]                                      == GOS_SUCCESS)
                {
                    uartReceiveResult = GOS_SUCCESS;
                    DRV_ERROR_CLEAR(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_DMA_TRIG);
                }
                else
                {
                    (void_t) HAL_UART_Abort_IT(&huarts[instance]);
                    DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_DMA_TRIG);
                }
            }
            else
            {
                uartReceiveResult = GOS_SUCCESS;
            }
        }
        else
        {
            (void_t) HAL_UART_Abort_IT(&huarts[instance]);
            DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_DMA_HAL);
            /* clear DMA mode/active on failure */
            uartRxMode[instance]    = DRV_UART_RX_MODE_NONE;
            uartRxOpActive[instance] = GOS_FALSE;
        }

        /* if DMA was not active (failure), ensure mode cleared */
        if (uartRxOpActive[instance] == GOS_FALSE)
        {
            uartRxMode[instance] = DRV_UART_RX_MODE_NONE;
        }
    }
    else
    {
        DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_DMA_MUTEX);
    }

    if (lockTaken)
    {
        (void_t) gos_mutexUnlock(&uartRxMutexes[instance]);
    }

    return uartReceiveResult;
}

/*
 * Function: drv_uartTransmitIT
 */
GOS_INLINE gos_result_t drv_uartTransmitIT (
        drv_uartPeriphInstance_t instance, u8_t* message,
        u16_t                    size,     u32_t mutexTmo,
        u32_t                    triggerTmo
        )
{
    gos_result_t uartTransmitResult = GOS_ERROR;
    bool_t       lockTaken          = GOS_FALSE;

    if (gos_mutexLock(&uartTxMutexes[instance], mutexTmo) == GOS_SUCCESS)
    {
        lockTaken = GOS_TRUE;

        if (triggerTmo > 0u)
        {
            (void_t) gos_triggerReset(&uartTxReadyTriggers[instance]);
            uartTxOpResult[instance] = GOS_ERROR;
            uartTxOpActive[instance] = GOS_TRUE;
        }

        if (HAL_UART_Transmit_IT(&huarts[instance], message, size) == HAL_OK)
        {
            DRV_ERROR_CLEAR(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_TX_IT_HAL);

            if (triggerTmo > 0u)
            {
                if (gos_triggerWait(&uartTxReadyTriggers[instance], 1, triggerTmo) == GOS_SUCCESS &&
                    gos_triggerReset(&uartTxReadyTriggers[instance])               == GOS_SUCCESS &&
                    uartTxOpResult[instance]                                       == GOS_SUCCESS)
                {
                    uartTransmitResult = GOS_SUCCESS;
                    DRV_ERROR_CLEAR(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_TX_IT_TRIG);
                }
                else
                {
                    (void_t) HAL_UART_Abort_IT(&huarts[instance]);
                    DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_TX_IT_TRIG);
                }
            }
            else
            {
                uartTransmitResult = GOS_SUCCESS;
            }
        }
        else
        {
            (void_t) HAL_UART_Abort_IT(&huarts[instance]);
            DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_TX_IT_HAL);
        }

        uartTxOpActive[instance] = GOS_FALSE;
    }
    else
    {
        DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_TX_IT_MUTEX);
    }

    if (lockTaken)
    {
        (void_t) gos_mutexUnlock(&uartTxMutexes[instance]);
    }

    return uartTransmitResult;
}

/*
 * Function: drv_uartReceiveIT
 */
GOS_INLINE gos_result_t drv_uartReceiveIT (drv_uartPeriphInstance_t instance, u8_t* message, u16_t size, u32_t mutexTmo, u32_t triggerTmo)
{
    gos_result_t uartReceiveResult = GOS_ERROR;
    bool_t   lockTaken         = GOS_FALSE;
    u16_t        bytesNeeded       = 0u;

    if ((message == NULL) || (size == 0u))
    {
        DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_IT_HAL);
        return GOS_ERROR;
    }

    if (gos_mutexLock(&uartRxMutexes[instance], mutexTmo) == GOS_SUCCESS)
    {
        lockTaken = GOS_TRUE;

        if (drv_uartStartRxBackground(instance) == GOS_SUCCESS)
        {
            if (drv_uartRxBufferAvailable(instance) >= size)
            {
                uartReceiveResult = GOS_SUCCESS;
            }
            else if (triggerTmo > 0u)
            {
                bytesNeeded = size - drv_uartRxBufferAvailable(instance);
                (void_t) gos_triggerReset(&uartRxReadyTriggers[instance]);

                if (gos_triggerWait(&uartRxReadyTriggers[instance], bytesNeeded, triggerTmo) == GOS_SUCCESS)
                {
                    uartReceiveResult = GOS_SUCCESS;
                }
                else
                {
                    DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_IT_TRIG);
                }
            }
            else
            {
                uartReceiveResult = GOS_SUCCESS;
            }

            if (uartReceiveResult == GOS_SUCCESS && drv_uartRxBufferAvailable(instance) >= size)
            {
                u16_t idx = 0u;
                for (idx = 0u; idx < size; idx++)
                {
                    message[idx] = drv_uartRxBufferPop(instance);
                }
            }
            else if ((uartReceiveResult == GOS_SUCCESS) && (triggerTmo == 0u))
            {
                // Asynchronous start mode: buffer will fill in the background.
            }
            else if (uartReceiveResult == GOS_SUCCESS)
            {
                uartReceiveResult = GOS_ERROR;
                DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_IT_TRIG);
            }
        }
        else
        {
            DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_IT_HAL);
        }
    }
    else
    {
        DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_IT_MUTEX);
    }

    if (lockTaken)
    {
        (void_t) gos_mutexUnlock(&uartRxMutexes[instance]);
    }

    return uartReceiveResult;
}

/*
 * Function: USART1_IRQHandler
 */
void_t USART1_IRQHandler (void_t)
{
    GOS_ISR_ENTER

    HAL_UART_IRQHandler(&huarts[DRV_UART_INSTANCE_1]);

    GOS_ISR_EXIT
}

/*
 * Function: USART2_IRQHandler
 */
void_t USART2_IRQHandler (void_t)
{
    GOS_ISR_ENTER

    HAL_UART_IRQHandler(&huarts[DRV_UART_INSTANCE_2]);

    GOS_ISR_EXIT
}

/*
 * Function: USART3_IRQHandler
 */
void_t USART3_IRQHandler (void_t)
{
    GOS_ISR_ENTER

    HAL_UART_IRQHandler(&huarts[DRV_UART_INSTANCE_3]);

    GOS_ISR_EXIT
}

/*
 * Function: UART4_IRQHandler
 */
void_t UART4_IRQHandler (void_t)
{
    GOS_ISR_ENTER

    HAL_UART_IRQHandler(&huarts[DRV_UART_INSTANCE_4]);

    GOS_ISR_EXIT
}

/*
 * Function: UART5_IRQHandler
 */
void_t UART5_IRQHandler (void_t)
{
    GOS_ISR_ENTER

    HAL_UART_IRQHandler(&huarts[DRV_UART_INSTANCE_5]);

    GOS_ISR_EXIT
}

/*
 * Function: USART6_IRQHandler
 */
void_t USART6_IRQHandler (void_t)
{
    GOS_ISR_ENTER

    HAL_UART_IRQHandler(&huarts[DRV_UART_INSTANCE_6]);

    GOS_ISR_EXIT
}

/*
 * Function: HAL_UART_TxCpltCallback
 */
void_t HAL_UART_TxCpltCallback (UART_HandleTypeDef *pHuart)
{
    drv_uartPeriphInstance_t instance = DRV_UART_INSTANCE_1;

    for (instance = DRV_UART_INSTANCE_1; instance < DRV_UART_NUM_OF_INSTANCES; instance++)
    {
        if (uartInstanceLut[instance] == pHuart->Instance)
        {
            uartTxOpResult[instance] = GOS_SUCCESS;
            (void_t) gos_triggerIncrement(&uartTxReadyTriggers[instance]);
            break;
        }
        else
        {
            // Continue.
        }
    }
}

/*
 * Function: HAL_UART_RxCpltCallback
 */
void_t HAL_UART_RxCpltCallback (UART_HandleTypeDef *pHuart)
{
    drv_uartPeriphInstance_t instance = DRV_UART_INSTANCE_1;

    for (instance = DRV_UART_INSTANCE_1; instance < DRV_UART_NUM_OF_INSTANCES; instance++)
    {
        if (uartInstanceLut[instance] == pHuart->Instance)
        {
            /* Branch behavior depending on RX mode */
            if (uartRxMode[instance] == DRV_UART_RX_MODE_IT)
            {
                /* IT-based single-byte receive: push into ring buffer and signal waiting users, then re-arm IT */
                uartRxOpResult[instance] = GOS_SUCCESS;
                drv_uartRxBufferPush(instance, uartRxOneByte[instance]);
                (void_t) gos_triggerIncrement(&uartRxReadyTriggers[instance]);

                if (HAL_UART_Receive_IT(&huarts[instance], &uartRxOneByte[instance], 1) == HAL_OK)
                {
                    uartRxOpActive[instance] = GOS_TRUE;
                }
                else
                {
                    uartRxOpActive[instance] = GOS_FALSE;
                    DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_IT_HAL);
                    uartRxMode[instance] = DRV_UART_RX_MODE_NONE;
                }
            }
            else if (uartRxMode[instance] == DRV_UART_RX_MODE_DMA)
            {
                /* DMA-based reception completed: mark success and wake waiters; do not push uartRxOneByte */
                uartRxOpResult[instance] = GOS_SUCCESS;
                (void_t) gos_triggerIncrement(&uartRxReadyTriggers[instance]);
                uartRxOpActive[instance] = GOS_FALSE;
                uartRxMode[instance] = DRV_UART_RX_MODE_NONE;
            }
            else
            {
                /* No background mode set: fallback to treating as IT byte if buffer available */
                uartRxOpResult[instance] = GOS_SUCCESS;
                drv_uartRxBufferPush(instance, uartRxOneByte[instance]);
                (void_t) gos_triggerIncrement(&uartRxReadyTriggers[instance]);

                /* try to re-arm IT to continue receiving */
                if (HAL_UART_Receive_IT(&huarts[instance], &uartRxOneByte[instance], 1) == HAL_OK)
                {
                    uartRxOpActive[instance] = GOS_TRUE;
                    uartRxMode[instance] = DRV_UART_RX_MODE_IT;
                }
                else
                {
                    uartRxOpActive[instance] = GOS_FALSE;
                    DRV_ERROR_SET(uartDiag.instanceErrorFlags[instance], DRV_ERROR_UART_RX_IT_HAL);
                }
            }
            break;
        }
        else
        {
            // Continue.
        }
    }
}

/*
 * Function: HAL_UART_MspInit
 */
void_t HAL_UART_MspInit (UART_HandleTypeDef* pHuart)
{
    /*
     * Local variables.
     */
    drv_uartPeriphInstance_t instance = 0u;
    u8_t                     idx      = 0u;

    /*
     * Function code.
     */
    for (instance = 0u; instance < DRV_UART_NUM_OF_INSTANCES; instance++)
    {
        if (uartInstanceLut[instance] == pHuart->Instance)
        {
            for (idx = 0u; idx < uartConfigSize / sizeof(drv_uartDescriptor_t); idx++)
            {
                if (uartConfig[idx].periphInstance == instance)
                {
                    if (uartConfig[idx].dmaConfigRx != NULL)
                    {
                        __HAL_LINKDMA(pHuart, hdmarx, uartConfig[idx].dmaConfigRx->hdma);
                    }
                    else
                    {
                        // There is no DMA assigned.
                    }

                    if (uartConfig[idx].dmaConfigTx != NULL)
                    {
                        __HAL_LINKDMA(pHuart, hdmatx, uartConfig[idx].dmaConfigTx->hdma);
                    }
                    else
                    {
                        // There is no DMA assigned.
                    }
                }
                else
                {
                    // Continue.
                }
            }
            break;
        }
        else
        {
            // Continue.
        }
    }
}

/*
 * Function: HAL_UART_ErrorCallback
 */
void_t HAL_UART_ErrorCallback (UART_HandleTypeDef* pHuart)
{
    drv_uartPeriphInstance_t instance = 0u;
    u32_t err = pHuart->ErrorCode;

    for (instance = 0u; instance < DRV_UART_NUM_OF_INSTANCES; instance++)
    {
        if (uartInstanceLut[instance] == pHuart->Instance)
        {
            if ((err & HAL_UART_ERROR_PE)  != 0u) { uartDiag.parityErrorCntr[instance]++; }
            if ((err & HAL_UART_ERROR_FE)  != 0u) { uartDiag.framingErrorCntr[instance]++; }
            if ((err & HAL_UART_ERROR_ORE) != 0u) { uartDiag.overrunErrorCntr[instance]++; }
            if ((err & HAL_UART_ERROR_NE)  != 0u) { uartDiag.noiseErrorCntr[instance]++; }

            if (uartRxOpActive[instance] == GOS_TRUE)
            {
                uartRxOpResult[instance] = GOS_ERROR;
                (void_t)gos_triggerIncrement(&uartRxReadyTriggers[instance]);
            }

            if (uartTxOpActive[instance] == GOS_TRUE)
            {
                uartTxOpResult[instance] = GOS_ERROR;
                (void_t)gos_triggerIncrement(&uartTxReadyTriggers[instance]);
            }

            __HAL_UART_CLEAR_PEFLAG(pHuart);
            __HAL_UART_CLEAR_FEFLAG(pHuart);
            __HAL_UART_CLEAR_NEFLAG(pHuart);
            __HAL_UART_CLEAR_OREFLAG(pHuart);
            pHuart->ErrorCode = HAL_UART_ERROR_NONE;

            uartRxOpActive[instance] = GOS_FALSE;
            uartRxMode[instance]     = DRV_UART_RX_MODE_NONE;

            /* Rearm continuous single-byte RX directly. */
            (void_t)drv_uartStartRxBackground(instance);
            break;
        }
    }
}