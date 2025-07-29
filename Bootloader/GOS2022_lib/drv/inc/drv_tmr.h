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
//! @file       drv_tmr.h
//! @author     Ahmed Gazar
//! @date       2025-07-24
//! @version    1.0
//!
//! @brief      GOS2022 Library / Timer driver header.
//! @details    This component provides access to the timer peripheries.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-07-24    Ahmed Gazar     Initial version created.
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
#ifndef DRV_TMR_H
#define DRV_TMR_H

/*
 * Includes
 */
#include <gos.h>

/*
 * Type definitions
 */
/**
 * TIM periphery instance enum.
 */
typedef enum
{
    DRV_TMR_INSTANCE_1,                                 //!< TIM1
    DRV_TMR_INSTANCE_2,                                 //!< TIM2
    DRV_TMR_INSTANCE_3,                                 //!< TIM3
    DRV_TMR_INSTANCE_4,                                 //!< TIM4
    DRV_TMR_INSTANCE_5,                                 //!< TIM5
    DRV_TMR_INSTANCE_6,                                 //!< TIM6
    DRV_TMR_INSTANCE_7,                                 //!< TIM7
    DRV_TMR_INSTANCE_8,                                 //!< TIM8
    DRV_TMR_INSTANCE_9,                                 //!< TIM9
    DRV_TMR_INSTANCE_10,                                //!< TIM10
    DRV_TMR_INSTANCE_11,                                //!< TIM11
    DRV_TMR_INSTANCE_12,                                //!< TIM12
    DRV_TMR_INSTANCE_13,                                //!< TIM13
    DRV_TMR_INSTANCE_14,                                //!< TIM14
    DRV_TMR_NUM_OF_INSTANCES                            //!< Number of TIM instances.
}drv_tmrPeriphInstance_t;

/**
 * TIM descriptor type.
 */
typedef struct
{
    drv_tmrPeriphInstance_t periphInstance;              //!< Periphery instance.
    u32_t                   prescaler;                   //!< Pre-scaler.
    u32_t                   repetitionCounter;           //!< Repetition counter.
    u32_t                   counterMode;                 //!< Counter mode.
    u32_t                   period;                      //!< Period.
    u32_t                   clockDivision;               //!< Clock division.
    u32_t                   autoReloadPreload;           //!< Auto reload period.
    bool_t                  useClockConfig;              //!< Use clock configuration flag.
    u32_t                   clockSource;                 //!< Clock source.
    bool_t                  useMasterConfig;             //!< Use master configuration flag.
    u32_t                   masterOutputTrigger;         //!< Master output trigger.
    u32_t                   masterSlaveMode;             //!< Master slave mode.
    void_t                  (*periodCallback)(void_t);   //!< Period callback function.
}drv_tmrDescriptor_t;

/**
 * TMR service instance enum.
 */
typedef enum
{
    DRV_TMR_SYSTIMER_INSTANCE = 0,                       //!< System timer instance.
}drv_tmrServiceInstance_t;

/**
 * TMR diagnostic data type.
 */
typedef struct __attribute__((packed))
{
	u32_t  globalErrorFlags;                               //!< Global error flags.
	bool_t instanceInitialized [DRV_TMR_NUM_OF_INSTANCES]; //!< Instance initialized flags.
	u32_t  instanceErrorFlags  [DRV_TMR_NUM_OF_INSTANCES]; //!< Instance error flags.
}drv_tmrDiag_t;

/*
 * Function prototypes
 */
/**
 * @brief     Initializes the registered TMR peripheries.
 * @details   Loops through the TMR configuration array and initializes peripheries
 *            defined by the user (externally).
 *
 * @return    Result of initialization.
 *
 * @retval    #GOS_SUCCESS Initialization successful.
 * @retval    #GOS_ERROR   Configuration array is NULL or some peripheries
 *                         could not be initialized.
 */
gos_result_t drv_tmrInit (
		void_t
		);

/**
 * @brief     Initializes the requested TMR instance.
 * @details   Calls the HAL level initializer, initializes the
 *            mutexes and triggers.
 *
 * @param[in] tmrInstanceIndex Instance index of TMR periphery.
 *
 * @return    Result of initialization.
 *
 * @retval    #GOS_SUCCESS Initialization successful.
 * @retval    #GOS_ERROR   One of the initialization steps failed.
 */
gos_result_t drv_tmrInitInstance (
		u8_t tmrInstanceIndex
		);

/**
 * @brief     De-initializes the requested TMR instance.
 * @details   Calls the HAL level de-initializer.
 *
 * @param[in] tmrInstanceIndex Instance index of TMR periphery.
 *
 * @return    Result of de-initialization.
 *
 * @retval    #GOS_SUCCESS De-initialization successful.
 * @retval    #GOS_ERROR   Wrong instance or HAL error.
 */
gos_result_t drv_tmrDeInitInstance (
		u8_t tmrInstanceIndex
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
gos_result_t drv_tmrGetDiagData (
		drv_tmrDiag_t* pDiag
		);

// TODO
gos_result_t drv_tmrStart (drv_tmrPeriphInstance_t instance, u32_t mutexTmo);

// TODO
gos_result_t drv_tmrStartIT (drv_tmrPeriphInstance_t instance, u32_t mutexTmo);

// TODO
gos_result_t drv_tmrStartDMA (drv_tmrPeriphInstance_t instance, u32_t* pData, u16_t dataLength, u32_t mutexTmo);

// TODO
gos_result_t drv_tmrGetValue (drv_tmrPeriphInstance_t instance, u32_t* pValue);

#endif
