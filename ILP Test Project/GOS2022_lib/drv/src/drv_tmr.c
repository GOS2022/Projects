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
//! @file       drv_tim.c
//! @author     Ahmed Gazar
//! @date       2024-05-09
//! @version    1.0
//!
//! @brief      GOS2022 Library / Timer driver source.
//! @details    For a more detailed description of this driver, please refer to @ref drv_tim.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-05-09    Ahmed Gazar     Initial version created.
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
#include <drv_tmr.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

/*
 * Static variables
 */
/**
 * TIM instance lookup table.
 */
GOS_STATIC TIM_TypeDef* tmrInstanceLut [] =
{
    [DRV_TMR_INSTANCE_1]  = TIM1,
    [DRV_TMR_INSTANCE_2]  = TIM2,
    [DRV_TMR_INSTANCE_3]  = TIM3,
    [DRV_TMR_INSTANCE_4]  = TIM4,
    [DRV_TMR_INSTANCE_5]  = TIM5,
    [DRV_TMR_INSTANCE_6]  = TIM6,
    [DRV_TMR_INSTANCE_7]  = TIM7,
    [DRV_TMR_INSTANCE_8]  = TIM8,
    [DRV_TMR_INSTANCE_9]  = TIM9,
    [DRV_TMR_INSTANCE_10] = TIM10,
    [DRV_TMR_INSTANCE_11] = TIM11,
    [DRV_TMR_INSTANCE_12] = TIM12,
    [DRV_TMR_INSTANCE_13] = TIM13,
    [DRV_TMR_INSTANCE_14] = TIM14,
};

/**
 * TIM handles.
 */
GOS_STATIC TIM_HandleTypeDef htims              [DRV_TMR_NUM_OF_INSTANCES];

/**
 * TIM mutexes.
 */
GOS_STATIC gos_mutex_t       tmrMutexes         [DRV_TMR_NUM_OF_INSTANCES];

/**
 * TIM ready triggers.
 */
GOS_STATIC gos_trigger_t     tmrReadyTriggers   [DRV_TMR_NUM_OF_INSTANCES];

/*
 * External variables
 */
/**
 * TIM configuration array (shall be defined by user).
 */
GOS_EXTERN GOS_CONST drv_tmrDescriptor_t     tmrConfig [];

/**
 * TIM configuration array size (shall be defined by user).
 */
GOS_EXTERN u32_t                             tmrConfigSize;

/*
 * Function: drv_tmrInit
 */
gos_result_t drv_tmrInit (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t tmrDriverInitResult = GOS_SUCCESS;
    u8_t         tmrIdx              = 0u;

    /*
     * Function code.
     */
    if (tmrConfig != NULL)
    {
        for (tmrIdx = 0u; tmrIdx < tmrConfigSize / sizeof(drv_tmrDescriptor_t); tmrIdx++)
        {
            GOS_CONCAT_RESULT(tmrDriverInitResult, drv_tmrInitInstance(tmrIdx));
        }
    }
    else
    {
        // Configuration array is NULL pointer.
        tmrDriverInitResult = GOS_ERROR;
    }

    return tmrDriverInitResult;
}

/*
 * Function: drv_tmrInitInstance
 */
gos_result_t drv_tmrInitInstance (u8_t tmrInstanceIndex)
{
    /*
     * Local variables.
     */
    gos_result_t            tmrInitResult      = GOS_ERROR;
    drv_tmrPeriphInstance_t instance           = 0u;
    TIM_MasterConfigTypeDef sMasterConfig      = {0};
    TIM_ClockConfigTypeDef  sClockSourceConfig = {0};

    /*
     * Function code.
     */
    if (tmrConfig != NULL && tmrInstanceIndex < (tmrConfigSize / sizeof(drv_tmrDescriptor_t)))
    {
        instance = tmrConfig[tmrInstanceIndex].periphInstance;

        htims[instance].Instance               = tmrInstanceLut[instance];
        htims[instance].Init.AutoReloadPreload = tmrConfig[tmrInstanceIndex].autoReloadPreload;
        htims[instance].Init.ClockDivision     = tmrConfig[tmrInstanceIndex].clockDivision;
        htims[instance].Init.CounterMode       = tmrConfig[tmrInstanceIndex].counterMode;
        htims[instance].Init.Period            = tmrConfig[tmrInstanceIndex].period;
        htims[instance].Init.Prescaler         = tmrConfig[tmrInstanceIndex].prescaler;
        htims[instance].Init.RepetitionCounter = tmrConfig[tmrInstanceIndex].repetitionCounter;
        sMasterConfig.MasterOutputTrigger      = tmrConfig[tmrInstanceIndex].masterOutputTrigger;
        sMasterConfig.MasterSlaveMode          = tmrConfig[tmrInstanceIndex].masterSlaveMode;
        sClockSourceConfig.ClockSource         = tmrConfig[tmrInstanceIndex].clockSource;

        if (HAL_TIM_Base_Init (&htims[instance])            == HAL_OK      &&
            gos_mutexInit     (&tmrMutexes[instance])       == GOS_SUCCESS &&
            gos_triggerInit   (&tmrReadyTriggers[instance]) == GOS_SUCCESS &&
            gos_triggerReset  (&tmrReadyTriggers[instance]) == GOS_SUCCESS
            )
        {
            tmrInitResult = GOS_SUCCESS;

            if (tmrConfig[tmrInstanceIndex].useClockConfig == GOS_TRUE)
            {
                if (HAL_TIM_ConfigClockSource(&htims[instance], &sClockSourceConfig) != HAL_OK)
                {
                    tmrInitResult = GOS_ERROR;
                }
                else
                {
                    // OK.
                }
            }
            else
            {
                // Not used.
            }

            if (tmrConfig[tmrInstanceIndex].useMasterConfig == GOS_TRUE)
            {
                if (HAL_TIMEx_MasterConfigSynchronization(&htims[instance], &sMasterConfig) != HAL_OK)
                {
                    tmrInitResult = GOS_ERROR;
                }
                else
                {
                    // OK.
                }
            }
            else
            {
                // Not used.
            }
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

    return tmrInitResult;
}

/*
 * Function: drv_tmrStart
 */
gos_result_t drv_tmrStart (drv_tmrPeriphInstance_t instance, u32_t mutexTmo)
{
    /*
     * Local variables.
     */
    gos_result_t tmrStartResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (gos_mutexLock(&tmrMutexes[instance], mutexTmo) == GOS_SUCCESS &&
        HAL_TIM_Base_Start(&htims[instance])           == HAL_OK)
    {
        tmrStartResult = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    (void_t) gos_mutexUnlock(&tmrMutexes[instance]);

    return tmrStartResult;
}

/*
 * Function: drv_tmrStartIT
 */
gos_result_t drv_tmrStartIT (drv_tmrPeriphInstance_t instance, u32_t mutexTmo)
{
    /*
     * Local variables.
     */
    gos_result_t tmrStartResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (gos_mutexLock(&tmrMutexes[instance], mutexTmo) == GOS_SUCCESS &&
        HAL_TIM_Base_Start_IT(&htims[instance])        == HAL_OK)
    {
        tmrStartResult = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    (void_t) gos_mutexUnlock(&tmrMutexes[instance]);

    return tmrStartResult;
}

/*
 * Function: drv_tmrStartDMA
 */
gos_result_t drv_tmrStartDMA (drv_tmrPeriphInstance_t instance, u32_t* pData, u16_t dataLength, u32_t mutexTmo)
{
    /*
     * Local variables.
     */
    gos_result_t tmrStartResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (gos_mutexLock(&tmrMutexes[instance], mutexTmo)              == GOS_SUCCESS &&
        HAL_TIM_Base_Start_DMA(&htims[instance], pData, dataLength) == HAL_OK)
    {
        tmrStartResult = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    (void_t) gos_mutexUnlock(&tmrMutexes[instance]);

    return tmrStartResult;
}

/*
 * Function: drv_tmrGetValue
 */
GOS_INLINE gos_result_t drv_tmrGetValue (drv_tmrPeriphInstance_t instance, u32_t* pValue)
{
    /*
     * Local variables.
     */
    gos_result_t tmrGetResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (pValue != NULL)
    {
        *pValue = __HAL_TIM_GET_COUNTER(&htims[instance]);
        tmrGetResult = GOS_SUCCESS;
    }
    else
    {
        // NULL pointer.
    }

    return tmrGetResult;
}

/*
 * Function: HAL_TIM_PeriodElapsedCallback
 */
void_t HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *pHtim)
{
    /*
     * Local variables.
     */
    drv_tmrPeriphInstance_t instance = DRV_TMR_INSTANCE_1;
    u8_t                    idx      = 0u;

    /*
     * Function code.
     */
    if (pHtim != NULL && tmrConfig != NULL)
    {
        for (instance = DRV_TMR_INSTANCE_1; instance < DRV_TMR_NUM_OF_INSTANCES; instance++)
        {
            if (tmrInstanceLut[instance] == pHtim->Instance)
            {
                for (idx = 0u; idx < tmrConfigSize / sizeof(drv_tmrDescriptor_t); idx++)
                {
                    if (tmrConfig[idx].periphInstance == instance)
                    {
                        if (tmrConfig[idx].periodCallback != NULL)
                        {
                            tmrConfig[idx].periodCallback();
                        }
                        else
                        {
                            // No registered callback.
                        }
                        break;
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
    else
    {
        // NULL pointer fail.
    }
}

/*
 * Function: TIM1_IRQHandler
 */
void_t TIM1_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_1]);

    GOS_ISR_EXIT
}

/*
 * Function: TIM2_IRQHandler
 */
void_t TIM2_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_2]);

    GOS_ISR_EXIT
}

/*
 * Function: TIM3_IRQHandler
 */
void_t TIM3_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_3]);

    GOS_ISR_EXIT
}

/*
 * Function: TIM4_IRQHandler
 */
void_t TIM4_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_4]);

    GOS_ISR_EXIT
}

/*
 * Function: TIM5_IRQHandler
 */
void_t TIM5_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_5]);

    GOS_ISR_EXIT
}

/*
 * Function: TIM6_IRQHandler
 */
void_t TIM6_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_6]);

    GOS_ISR_EXIT
}

/*
 * Function: TIM7_IRQHandler
 */
void_t TIM7_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_7]);

    GOS_ISR_EXIT
}

/*
 * Function: TIM8_IRQHandler
 */
void_t TIM8_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_8]);

    GOS_ISR_EXIT
}

/*
 * Function: TIM9_IRQHandler
 */
void_t TIM9_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_9]);

    GOS_ISR_EXIT
}

/*
 * Function: TIM10_IRQHandler
 */
void_t TIM10_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_10]);

    GOS_ISR_EXIT
}

/*
 * Function: TIM11_IRQHandler
 */
void_t TIM11_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_11]);

    GOS_ISR_EXIT
}

/*
 * Function: TIM12_IRQHandler
 */
void_t TIM12_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_12]);

    GOS_ISR_EXIT
}

/*
 * Function: TIM13_IRQHandler
 */
void_t TIM13_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_13]);

    GOS_ISR_EXIT
}

/*
 * Function: TIM14_IRQHandler
 */
void_t TIM14_IRQHandler (void_t)
{
    /*
     * Function code.
     */
    GOS_ISR_ENTER

    HAL_TIM_IRQHandler(&htims[DRV_TMR_INSTANCE_14]);

    GOS_ISR_EXIT
}
