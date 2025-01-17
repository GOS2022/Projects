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
//! @file       drv_dac.c
//! @author     Ahmed Gazar
//! @date       2024-05-08
//! @version    1.0
//!
//! @brief      GOS2022 Library / DAC driver source.
//! @details    For a more detailed description of this driver, please refer to @ref drv_dac.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-05-08    Ahmed Gazar     Initial version created.
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
#include <drv_dac.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_dac.h"

#if defined(STM32F446RE)
/*
 * Static variables
 */
/**
 * DAC instance lookup table.
 */
GOS_STATIC DAC_TypeDef* dacInstanceLut [] =
{
    [DRV_DAC_INSTANCE_1] = DAC1
};

/**
 * DAC handles.
 */
GOS_STATIC DAC_HandleTypeDef hdacs              [DRV_DAC_NUM_OF_INSTANCES];

/**
 * DAC mutexes.
 */
GOS_STATIC gos_mutex_t       dacMutexes         [DRV_DAC_NUM_OF_INSTANCES];

/**
 * DAC ready triggers.
 */
GOS_STATIC gos_trigger_t     dacReadyTriggers   [DRV_DAC_NUM_OF_INSTANCES];

/*
 * External variables
 */
/**
 * DAC configuration array (shall be defined by user).
 */
GOS_EXTERN GOS_CONST drv_dacDescriptor_t     dacConfig [];

/**
 * DAC configuration array size (shall be defined by user).
 */
GOS_EXTERN u32_t                             dacConfigSize;

/*
 * Function: drv_dacInit
 */
gos_result_t drv_dacInit (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t dacDriverInitResult = GOS_SUCCESS;
    u8_t         dacIdx              = 0u;

    /*
     * Function code.
     */
    if (dacConfig != NULL)
    {
        for (dacIdx = 0u; dacIdx < dacConfigSize / sizeof(drv_dacDescriptor_t); dacIdx++)
        {
            GOS_CONCAT_RESULT(dacDriverInitResult, drv_dacInitInstance(dacIdx));
        }
    }
    else
    {
        // Configuration array is NULL pointer.
        dacDriverInitResult = GOS_ERROR;
    }

    return dacDriverInitResult;
}

/*
 * Function: drv_dacInitInstance
 */
gos_result_t drv_dacInitInstance (u8_t dacInstanceIndex)
{
    /*
     * Local variables.
     */
    gos_result_t            dacInitResult = GOS_ERROR;
    drv_dacPeriphInstance_t instance      = 0u;
    DAC_ChannelConfTypeDef  sConfig       = {0};

    /*
     * Function code.
     */
    if (dacConfig != NULL && dacInstanceIndex < (dacConfigSize / sizeof(drv_dacDescriptor_t)))
    {
        instance = dacConfig[dacInstanceIndex].periphInstance;

        hdacs[instance].Instance = dacInstanceLut[instance];

        /*
         *  Configure the selected DAC regular channel.
         */
        sConfig.DAC_Trigger      = dacConfig[dacInstanceIndex].trigger;
        sConfig.DAC_OutputBuffer = dacConfig[dacInstanceIndex].outputBuffer;

        if (HAL_DAC_Init         (&hdacs[instance])                                                == HAL_OK      &&
            HAL_DAC_ConfigChannel(&hdacs[instance], &sConfig, dacConfig[dacInstanceIndex].channel) == HAL_OK      &&
            gos_mutexInit        (&dacMutexes[instance])                                           == GOS_SUCCESS &&
            gos_triggerInit      (&dacReadyTriggers[instance])                                     == GOS_SUCCESS &&
            gos_triggerReset     (&dacReadyTriggers[instance])                                     == GOS_SUCCESS
            )
        {
            dacInitResult = GOS_SUCCESS;
        }
        else
        {
            // Init error.
        }
    }
    else
    {
        // Configuration missing or index is out of array boundary.
    }

    return dacInitResult;
}

/*
 * Function: drv_dacStart
 */
gos_result_t drv_dacStart (
    drv_dacPeriphInstance_t instance,
    u32_t*                  pBuffer,
    u32_t                   buffLen
    )
{
    /*
     * Local variables.
     */
    gos_result_t dacStartResult = GOS_ERROR;
    u8_t         idx            = 0u;

    /*
     * Function code.
     */
    if (pBuffer != NULL && dacConfig != NULL)
    {
        for (idx = 0u; idx < dacConfigSize; idx++)
        {
            if (dacConfig[idx].periphInstance == instance)
            {
                dacStartResult = GOS_SUCCESS;

                if (dacConfig[instance].tmrConfig != NULL)
                {
                    // Start timer.
                    dacStartResult = drv_tmrStart(dacConfig[instance].tmrConfig->periphInstance, GOS_MUTEX_ENDLESS_TMO);
                }
                else
                {
                    // Timer is not configured for this DAC.
                }

                if (dacStartResult == GOS_SUCCESS &&
                    HAL_DAC_Start_DMA(&hdacs[instance], dacConfig[idx].channel, pBuffer, buffLen, dacConfig[idx].dataAlignment) == HAL_OK)
                {
                    dacStartResult = GOS_SUCCESS;
                }
                else
                {
                    // Error.
                }
                break;
            }
            else
            {
                // Continue.
            }
        }
    }
    else
    {
        // NULL pointer.
    }

    return dacStartResult;
}

/*
 * Function: HAL_DAC_MspInit
 */
void_t HAL_DAC_MspInit (DAC_HandleTypeDef* hdac)
{
    /*
     * Local variables.
     */
    drv_dacPeriphInstance_t instance = 0u;
    u8_t                    idx      = 0u;

    /*
     * Function code.
     */
    for (instance = 0u; instance < DRV_DAC_NUM_OF_INSTANCES; instance++)
    {
        if (dacInstanceLut[instance] == hdac->Instance)
        {
            for (idx = 0u; idx < dacConfigSize / sizeof(drv_dacDescriptor_t); idx++)
            {
                if (dacConfig[idx].periphInstance == instance)
                {
                    if (dacConfig[idx].dmaConfig != NULL)
                    {
                        __HAL_LINKDMA(hdac, DMA_Handle1, dacConfig[idx].dmaConfig->hdma);
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
        }
        else
        {
            // Continue.
        }
    }
}
#endif
