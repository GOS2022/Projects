/*
 * rcc_driver.c
 *
 *  Created on: Nov 10, 2022
 *      Author: Gabor
 */
#include "rcc_driver.h"
//#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

gos_result_t rcc_driver_init (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_ERROR;
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/*
	 * Function code.
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 4;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) == HAL_OK)
	{
		  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
		                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
		  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
		  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
		  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) == HAL_OK)
		{
			initResult = GOS_SUCCESS;
		}
	}

	// TODO
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();
	__HAL_RCC_DMA2_CLK_ENABLE();
	__HAL_RCC_I2C1_CLK_ENABLE();
	__HAL_RCC_I2C2_CLK_ENABLE();
	__HAL_RCC_SPI2_CLK_ENABLE();
	__HAL_RCC_SPI3_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_TIM11_CLK_ENABLE();
	//__HAL_RCC_DAC_CLK_ENABLE();

	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();
	//__HAL_RCC_USART3_CLK_ENABLE();
	//__HAL_RCC_UART4_CLK_ENABLE();
	//__HAL_RCC_UART5_CLK_ENABLE();
	__HAL_RCC_USART6_CLK_ENABLE();

	return initResult;
}
