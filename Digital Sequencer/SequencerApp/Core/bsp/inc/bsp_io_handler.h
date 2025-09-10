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
//! @file       bsp_io_handler.h
//! @author     Ahmed Gazar
//! @date       2025-09-10
//! @version    1.0
//!
//! @brief      Digital Step Sequencer / Board Support Package / IO handler header.
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
#ifndef BSP_IO_HANDLER_H
#define BSP_IO_HANDLER_H
/*
 * Includes
 */
#include "gos_lib.h"

/*
 * Type definitions
 */
/**
 * IO channels.
 */
typedef enum
{
	BSP_IO_STEP_BUTTON_1,     //!< Step ON/OFF button 1
	BSP_IO_STEP_BUTTON_2,     //!< Step ON/OFF button 2
	BSP_IO_STEP_BUTTON_3,     //!< Step ON/OFF button 3
	BSP_IO_STEP_BUTTON_4,     //!< Step ON/OFF button 4
	BSP_IO_STEP_BUTTON_5,     //!< Step ON/OFF button 5
	BSP_IO_STEP_BUTTON_6,     //!< Step ON/OFF button 6
	BSP_IO_STEP_BUTTON_7,     //!< Step ON/OFF button 7
	BSP_IO_STEP_BUTTON_8,     //!< Step ON/OFF button 8
	BSP_IO_START_STOP_BUTTON, //!< Sequencing START/STOP button.
	BSP_IO_STEP_ON_OFF_LED_1, //!< Step ON/OFF indicator LED 1
	BSP_IO_STEP_ON_OFF_LED_2, //!< Step ON/OFF indicator LED 2
	BSP_IO_STEP_ON_OFF_LED_3, //!< Step ON/OFF indicator LED 3
	BSP_IO_STEP_ON_OFF_LED_4, //!< Step ON/OFF indicator LED 4
	BSP_IO_STEP_ON_OFF_LED_5, //!< Step ON/OFF indicator LED 5
	BSP_IO_STEP_ON_OFF_LED_6, //!< Step ON/OFF indicator LED 6
	BSP_IO_STEP_ON_OFF_LED_7, //!< Step ON/OFF indicator LED 7
	BSP_IO_STEP_ON_OFF_LED_8, //!< Step ON/OFF indicator LED 8
	BSP_IO_STEP_CNTR_LED_1,   //!< Step counter LED 1
	BSP_IO_STEP_CNTR_LED_2,   //!< Step counter LED 2
	BSP_IO_STEP_CNTR_LED_3,   //!< Step counter LED 3
	BSP_IO_STEP_CNTR_LED_4,   //!< Step counter LED 4
	BSP_IO_STEP_CNTR_LED_5,   //!< Step counter LED 5
	BSP_IO_STEP_CNTR_LED_6,   //!< Step counter LED 6
	BSP_IO_STEP_CNTR_LED_7,   //!< Step counter LED 7
	BSP_IO_STEP_CNTR_LED_8,   //!< Step counter LED 8
	BSP_IO_TOTAL_NUMBER       //!< Number of IOs.
}bsp_io_handler_def_t;

/**
 * Edge types.
 */
typedef enum
{
	BSP_IO_EDGE_NONE,    //!< No edge detection.
	BSP_IO_EDGE_FALLING, //!< Falling edge (high to low).
	BSP_IO_EDGE_RISING,  //!< Rising edge (low to high).
	BSP_IO_EDGE_BOTH     //!< Both edges.
}bsp_io_handler_edge_t;

/**
 * IO callback type.
 */
typedef void_t (*bsp_io_callback_t)(bsp_io_handler_def_t);

/*
 * Function prototypes
 */
// TODO
gos_result_t bsp_ioHandlerInit (void_t);
gos_result_t bsp_ioHandlerRegisterCallback (bsp_io_handler_def_t pin, bsp_io_callback_t callback);
gos_result_t bsp_ioHandlerGetStepNumber (u8_t* pStepNum);
gos_result_t bsp_ioHandlerReadPin (bsp_io_handler_def_t pin, drv_gpioState_t* pState);
gos_result_t bsp_ioHandlerWritePin (bsp_io_handler_def_t pin, drv_gpioState_t state);
gos_result_t bsp_ioHandlerTogglePin (bsp_io_handler_def_t pin);

#endif /* BSP_INC_BSP_IO_HANDLER_H_ */
