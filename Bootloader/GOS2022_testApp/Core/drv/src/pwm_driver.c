/*
 * pwm_driver.c
 *
 *  Created on: Jul 3, 2025
 *      Author: Gabor
 */
#include "pwm_driver.h"

#include "stm32f4xx_hal.h"

GOS_STATIC TIM_HandleTypeDef htim2;

gos_result_t pwm_driver_init (void_t)
{
	gos_result_t pwmDriverInitResult = GOS_ERROR;

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 65535;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

	if (HAL_TIM_Base_Init(&htim2) == HAL_OK &&
		HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) == HAL_OK &&
		HAL_TIM_PWM_Init(&htim2) == HAL_OK &&
		HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) == HAL_OK &&
		HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) == HAL_OK &&
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) == HAL_OK
		)
	{
		pwmDriverInitResult = GOS_SUCCESS;
	}

	return pwmDriverInitResult;
}

void_t pwm_driver_set_duty (u8_t duty)
{
	TIM2->CCR1 = 65535 * duty / 100;
}
