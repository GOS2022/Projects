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
//! @file       bsp_adc_handler.h
//! @author     Ahmed Gazar
//! @date       2025-09-10
//! @version    1.0
//!
//! @brief      Digital Step Sequencer / Board Support Package / ADC handler header.
//! @details    This component is the main control logic for DSS.
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
#ifndef BSP_ADC_HANDLER_H
#define BSP_ADC_HANDLER_H
/*
 * Includes
 */
#include "gos_lib.h"

/*
 * Type definitions
 */
/**
 * ADC channels.
 */
typedef enum
{
	BSP_ADC_CHANNEL_CV_1, //!< Control Voltage 1.
	BSP_ADC_CHANNEL_CV_2, //!< Control Voltage 2.
	BSP_ADC_CHANNEL_CV_3, //!< Control Voltage 3.
	BSP_ADC_CHANNEL_CV_4, //!< Control Voltage 4.
	BSP_ADC_CHANNEL_CV_5, //!< Control Voltage 5.
	BSP_ADC_CHANNEL_CV_6, //!< Control Voltage 6.
	BSP_ADC_CHANNEL_CV_7, //!< Control Voltage 7.
	BSP_ADC_CHANNEL_CV_8, //!< Control Voltage 8.
	BSP_ADC_CHANNEL_BPM,  //!< BPM Control.
	BSP_ADC_CHANNEL_NUM   //!< Number of channels.
}bsp_adc_channel_def_t;

/*
 * Function prototypes
 */
/**
 * @brief   Initializes the ADC handler.
 * @details Registers the ADC handler task and initializes the internal mutex.
 *
 * @return  Success of initialization.
 *
 * @retval  #GOS_SUCCESS Initialization successful.
 * @retval  #GOS_ERROR   Task registration or mutex initialization failed.
 */
gos_result_t bsp_adcHandlerInit (void_t);

/**
 * @brief      Returns the percentage value of the given ADC channel.
 * @details    Returns the calculated percentage value from the internal ADC map.
 *
 * @param[in]  channel ADC channel number.
 * @param[out] pPerc   Pointer to a variable to store the percentage [x100].
 *
 * @return     Result of getting.
 *
 * @retval     #GOS_SUCCESS Value returned successfully.
 * @retval     #GOS_ERROR   Invalid channel number, NULL pointer, or mutex lock failure.
 */
gos_result_t bsp_adcHandlerGetPercentage (bsp_adc_channel_def_t channel, u16_t* pPerc);

#endif
