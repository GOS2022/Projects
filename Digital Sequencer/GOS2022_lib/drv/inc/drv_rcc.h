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
//! @file       drv_rcc.h
//! @author     Ahmed Gazar
//! @date       2025-09-10
//! @version    1.0
//!
//! @brief      GOS2022 Library / RCC driver header.
//! @details    This component provides access to the RCC peripheries.
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
#ifndef DRV_RCC_H
#define DRV_RCC_H
/*
 * Includes
 */
#include <gos.h>

/*
 * Type definitions
 */
/**
 * RCC descriptor type.
 */
typedef struct
{
	u32_t voltageScaling;      //!< Voltage scaling.
    u32_t oscillatorType;      //!< Oscillator type.
    u32_t HSIState;            //!< HSI state.
    u32_t HSICalibrationValue; //!< HSI calibration value.
    u32_t PLLState;            //!< PLL state.
    u32_t PLLSource;           //!< PLL source.
    u32_t PLLM;                //!< PLLM.
    u32_t PLLN;                //!< PLLN.
    u32_t PLLP;                //!< PLLP.
    u32_t PLLQ;                //!< PLLQ.
    u32_t PLLR;                //!< PLLR.
    u32_t clockType;           //!< Clock type.
    u32_t SYSCLKSource;        //!< System clock source.
    u32_t AHBCLKDivider;       //!< AHB clock divider.
    u32_t APB1CLKDivider;      //!< APB1 clock divider.
    u32_t APB2CLKDivider;      //!< APB2 clock divider.
    u32_t flashLatency;        //!< Flash latency.
    u64_t clkEnableFlags;      //!< Clock enable flags for peripheries.
}drv_rccDescriptor_t;

/**
 * RCC peripheral clock enable options.
 */
typedef enum
{
	DRV_RCC_CLK_ENABLE_UART1  = 0b000000000000000000000000000000000000000000000001,
	DRV_RCC_CLK_ENABLE_UART2  = 0b000000000000000000000000000000000000000000000010,
	DRV_RCC_CLK_ENABLE_UART3  = 0b000000000000000000000000000000000000000000000100,
	DRV_RCC_CLK_ENABLE_UART4  = 0b000000000000000000000000000000000000000000001000,
	DRV_RCC_CLK_ENABLE_UART5  = 0b000000000000000000000000000000000000000000010000,
	DRV_RCC_CLK_ENABLE_UART6  = 0b000000000000000000000000000000000000000000100000,
	DRV_RCC_CLK_ENABLE_I2C1   = 0b000000000000000000000000000000000000000001000000,
	DRV_RCC_CLK_ENABLE_I2C2   = 0b000000000000000000000000000000000000000010000000,
	DRV_RCC_CLK_ENABLE_I2C3   = 0b000000000000000000000000000000000000000100000000,
	DRV_RCC_CLK_ENABLE_SPI1   = 0b000000000000000000000000000000000000001000000000,
	DRV_RCC_CLK_ENABLE_SPI2   = 0b000000000000000000000000000000000000010000000000,
	DRV_RCC_CLK_ENABLE_SPI3   = 0b000000000000000000000000000000000000100000000000,
	DRV_RCC_CLK_ENABLE_SPI4   = 0b000000000000000000000000000000000001000000000000,
	DRV_RCC_CLK_ENABLE_GPIOA  = 0b000000000000000000000000000000000010000000000000,
	DRV_RCC_CLK_ENABLE_GPIOB  = 0b000000000000000000000000000000000100000000000000,
	DRV_RCC_CLK_ENABLE_GPIOC  = 0b000000000000000000000000000000001000000000000000,
	DRV_RCC_CLK_ENABLE_GPIOD  = 0b000000000000000000000000000000010000000000000000,
	DRV_RCC_CLK_ENABLE_GPIOE  = 0b000000000000000000000000000000100000000000000000,
	DRV_RCC_CLK_ENABLE_GPIOF  = 0b000000000000000000000000000001000000000000000000,
	DRV_RCC_CLK_ENABLE_GPIOG  = 0b000000000000000000000000000010000000000000000000,
	DRV_RCC_CLK_ENABLE_GPIOH  = 0b000000000000000000000000000100000000000000000000,
	DRV_RCC_CLK_ENABLE_ADC1   = 0b000000000000000000000000001000000000000000000000,
	DRV_RCC_CLK_ENABLE_ADC2   = 0b000000000000000000000000010000000000000000000000,
	DRV_RCC_CLK_ENABLE_ADC3   = 0b000000000000000000000000100000000000000000000000,
	DRV_RCC_CLK_ENABLE_DAC1   = 0b000000000000000000000001000000000000000000000000,
	DRV_RCC_CLK_ENABLE_DMA1   = 0b000000000000000000000010000000000000000000000000,
	DRV_RCC_CLK_ENABLE_DMA2   = 0b000000000000000000000100000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR1   = 0b000000000000000000001000000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR2   = 0b000000000000000000010000000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR3   = 0b000000000000000000100000000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR4   = 0b000000000000000001000000000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR5   = 0b000000000000000010000000000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR6   = 0b000000000000000100000000000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR7   = 0b000000000000001000000000000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR8   = 0b000000000000010000000000000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR9   = 0b000000000000100000000000000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR10  = 0b000000000001000000000000000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR11  = 0b000000000010000000000000000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR12  = 0b000000000100000000000000000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR13  = 0b000000001000000000000000000000000000000000000000,
	DRV_RCC_CLK_ENABLE_TMR14  = 0b000000010000000000000000000000000000000000000000,

}drv_rccClockEnable_t;

/*
 * Function prototypes
 */
/**
 * @brief     Initializes the clock peripheries.
 * @details   Loops through the RCC configuration array and initializes peripheries
 *            defined by the user (externally).
 *
 * @return    Result of initialization.
 *
 * @retval    #GOS_SUCCESS Initialization successful.
 * @retval    #GOS_ERROR   Configuration array is NULL or some peripheries
 *                         could not be initialized.
 */
gos_result_t drv_rccInit (
        void_t
        );

#endif
