/*
 * gpio_cfg.c
 *
 *  Created on: Feb 27, 2024
 *      Author: Gabor
 */

#include <drv_gpio.h>
#include "iodef.h"

/**
 * TODO
 */
GOS_CONST drv_gpioDescriptor_t gpioConfig [] =
{
		[IO_USER_LED] =
		{
			.port = GPIOA,
			.pin  = GPIO_PIN_5,
			.mode = GPIO_MODE_OUTPUT_PP,
			.pull = GPIO_NOPULL,
		},
		[IO_UART_2_RX] =
		{
			.port = GPIOA,
			.pin = GPIO_PIN_2,
			.mode = GPIO_MODE_AF_PP,
			.pull = GPIO_NOPULL,
			.speed = GPIO_SPEED_FREQ_VERY_HIGH,
			.alternate = GPIO_AF7_USART2
		},
		[IO_UART_2_TX] =
		{
			.port = GPIOA,
			.pin = GPIO_PIN_3,
			.mode = GPIO_MODE_AF_PP,
			.pull = GPIO_NOPULL,
			.speed = GPIO_SPEED_FREQ_VERY_HIGH,
			.alternate = GPIO_AF7_USART2
		},
};

/**
 * TODO
 */
u32_t gpioConfigSize = sizeof(gpioConfig);

GOS_CONST drv_gpioItCallbackDescriptor_t gpioItConfig [] = {};

u32_t gpioItConfigSize = sizeof(gpioItConfig);
