/*
 * rcc_cfg.c
 *
 *  Created on: Sep 10, 2025
 *      Author: Gabor
 */
#include "drv_rcc.h"
#include "stm32f4xx_hal.h"

drv_rccDescriptor_t rccConfig =
{
	.voltageScaling      = PWR_REGULATOR_VOLTAGE_SCALE3,
	.oscillatorType      = RCC_OSCILLATORTYPE_HSI,
	.HSIState            = RCC_HSI_ON,
	.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
	.PLLState            = RCC_PLL_ON,
	.PLLSource           = RCC_PLLSOURCE_HSI,
	.PLLM                = 8,
	.PLLN                = 64,
	.PLLP                = RCC_PLLP_DIV2,
	.PLLQ                = 2,
	.PLLR                = 2,
	.clockType           = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2,
	.SYSCLKSource        = RCC_SYSCLKSOURCE_PLLCLK,
	.AHBCLKDivider       = RCC_SYSCLK_DIV1,
	.APB1CLKDivider      = RCC_HCLK_DIV2,
	.APB2CLKDivider      = RCC_HCLK_DIV1,
	.flashLatency        = FLASH_LATENCY_2,
	.clkEnableFlags      = DRV_RCC_CLK_ENABLE_DMA1  | DRV_RCC_CLK_ENABLE_DMA2  | DRV_RCC_CLK_ENABLE_GPIOA |
						   DRV_RCC_CLK_ENABLE_GPIOB | DRV_RCC_CLK_ENABLE_GPIOC | DRV_RCC_CLK_ENABLE_GPIOD |
						   DRV_RCC_CLK_ENABLE_GPIOH | DRV_RCC_CLK_ENABLE_I2C1  | DRV_RCC_CLK_ENABLE_UART1 |
						   DRV_RCC_CLK_ENABLE_UART5 | DRV_RCC_CLK_ENABLE_TMR10 | DRV_RCC_CLK_ENABLE_TMR11
};
