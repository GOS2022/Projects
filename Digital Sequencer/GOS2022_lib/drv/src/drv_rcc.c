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
//! @file       drv_rcc.c
//! @author     Ahmed Gazar
//! @date       2025-09-10
//! @version    1.0
//!
//! @brief      GOS2022 Library / RCC driver source.
//! @details    For a more detailed description of this driver, please refer to @ref drv_rcc.h
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
/*
 * Includes
 */
#include <drv_rcc.h>
#include "stm32f4xx_hal.h"

/*
 * External variables
 */
/**
 * RCC configuration (shall be defined by user).
 */
GOS_EXTERN GOS_CONST drv_rccDescriptor_t rccConfig;

/*
 * Function: drv_rccInit
 */
gos_result_t drv_rccInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t       initResult       = GOS_ERROR;
	RCC_OscInitTypeDef RCCOscInitStruct = {0};
	RCC_ClkInitTypeDef RCCClkInitStruct = {0};

	/*
	 * Function code.
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(rccConfig.voltageScaling);

	RCCOscInitStruct.OscillatorType      = rccConfig.oscillatorType;
	RCCOscInitStruct.HSIState            = rccConfig.HSIState;
	RCCOscInitStruct.HSICalibrationValue = rccConfig.HSICalibrationValue;
	RCCOscInitStruct.PLL.PLLState        = rccConfig.PLLState;
	RCCOscInitStruct.PLL.PLLSource       = rccConfig.PLLSource;
	RCCOscInitStruct.PLL.PLLM            = rccConfig.PLLM;
	RCCOscInitStruct.PLL.PLLN            = rccConfig.PLLN;
	RCCOscInitStruct.PLL.PLLP            = rccConfig.PLLP;
	RCCOscInitStruct.PLL.PLLQ            = rccConfig.PLLQ;
	RCCOscInitStruct.PLL.PLLR            = rccConfig.PLLR;

	if (HAL_RCC_OscConfig(&RCCOscInitStruct) == HAL_OK)
	{
		RCCClkInitStruct.ClockType      = rccConfig.clockType;
		RCCClkInitStruct.SYSCLKSource   = rccConfig.SYSCLKSource;
		RCCClkInitStruct.AHBCLKDivider  = rccConfig.AHBCLKDivider;
		RCCClkInitStruct.APB1CLKDivider = rccConfig.APB1CLKDivider;
		RCCClkInitStruct.APB2CLKDivider = rccConfig.APB2CLKDivider;

		if (HAL_RCC_ClockConfig(&RCCClkInitStruct, rccConfig.flashLatency) == HAL_OK)
		{
			initResult = GOS_SUCCESS;

			// Check CLK enable flags.
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_GPIOA)
			{
				__HAL_RCC_GPIOA_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_GPIOB)
			{
				__HAL_RCC_GPIOB_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_GPIOC)
			{
				__HAL_RCC_GPIOC_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_GPIOD)
			{
				__HAL_RCC_GPIOD_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_GPIOE)
			{
				__HAL_RCC_GPIOE_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_GPIOF)
			{
				__HAL_RCC_GPIOF_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_GPIOG)
			{
				__HAL_RCC_GPIOG_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_GPIOH)
			{
				__HAL_RCC_GPIOH_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_UART1)
			{
				__HAL_RCC_USART1_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_UART2)
			{
				__HAL_RCC_USART2_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_UART3)
			{
				__HAL_RCC_USART3_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_UART4)
			{
				__HAL_RCC_UART4_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_UART5)
			{
				__HAL_RCC_UART5_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_UART6)
			{
				__HAL_RCC_USART6_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_I2C1)
			{
				__HAL_RCC_I2C1_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_I2C2)
			{
				__HAL_RCC_I2C2_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_I2C3)
			{
				__HAL_RCC_I2C3_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_ADC1)
			{
				__HAL_RCC_ADC1_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_ADC2)
			{
				__HAL_RCC_ADC2_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_ADC3)
			{
				__HAL_RCC_ADC3_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_DAC1)
			{
				__HAL_RCC_DAC_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_DMA1)
			{
				__HAL_RCC_DMA1_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_DMA2)
			{
				__HAL_RCC_DMA2_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR1)
			{
				__HAL_RCC_TIM1_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR2)
			{
				__HAL_RCC_TIM2_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR3)
			{
				__HAL_RCC_TIM3_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR4)
			{
				__HAL_RCC_TIM4_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR5)
			{
				__HAL_RCC_TIM5_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR6)
			{
				__HAL_RCC_TIM6_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR7)
			{
				__HAL_RCC_TIM7_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR8)
			{
				__HAL_RCC_TIM8_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR9)
			{
				__HAL_RCC_TIM9_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR10)
			{
				__HAL_RCC_TIM10_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR11)
			{
				__HAL_RCC_TIM11_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR12)
			{
				__HAL_RCC_TIM12_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR13)
			{
				__HAL_RCC_TIM13_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_TMR14)
			{
				__HAL_RCC_TIM14_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_SPI1)
			{
				__HAL_RCC_SPI1_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_SPI2)
			{
				__HAL_RCC_SPI2_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_SPI3)
			{
				__HAL_RCC_SPI3_CLK_ENABLE();
			}
			if (rccConfig.clkEnableFlags & DRV_RCC_CLK_ENABLE_SPI4)
			{
				__HAL_RCC_SPI4_CLK_ENABLE();
			}

			__HAL_RCC_SYSCFG_CLK_ENABLE();
			__HAL_RCC_PWR_CLK_ENABLE();
		}
		else
		{
			// Init error.
		}
	}
	else
	{
		// Init error.
	}

	return initResult;
}
