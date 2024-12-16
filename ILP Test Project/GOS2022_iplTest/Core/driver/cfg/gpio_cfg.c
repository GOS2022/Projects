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
			.pin = GPIO_PIN_5,
			.mode = GPIO_MODE_OUTPUT_PP,
			.pull = GPIO_NOPULL,
		},
		[IO_UART_1_RX] =
		{
			.port = GPIOA,
			.pin = GPIO_PIN_10,
			.mode = GPIO_MODE_AF_PP,
			.pull = GPIO_NOPULL,
			.speed = GPIO_SPEED_FREQ_VERY_HIGH,
			.alternate = GPIO_AF7_USART1
		},
		[IO_UART_1_TX] =
		{
			.port = GPIOA,
			.pin = GPIO_PIN_9,
			.mode = GPIO_MODE_AF_PP,
			.pull = GPIO_NOPULL,
			.speed = GPIO_SPEED_FREQ_VERY_HIGH,
			.alternate = GPIO_AF7_USART1
		},
		[IO_UART_5_RX] =
		{
			.port = GPIOD,
			.pin = GPIO_PIN_2,
			.mode = GPIO_MODE_AF_PP,
			.pull = GPIO_NOPULL,
			.speed = GPIO_SPEED_FREQ_VERY_HIGH,
			.alternate = GPIO_AF8_UART5
		},
		[IO_UART_5_TX] =
		{
			.port = GPIOC,
			.pin = GPIO_PIN_12,
			.mode = GPIO_MODE_AF_PP,
			.pull = GPIO_NOPULL,
			.speed = GPIO_SPEED_FREQ_VERY_HIGH,
			.alternate = GPIO_AF8_UART5
		},
		[IO_UART_4_RX] =
		{
			.port = GPIOA,
			.pin = GPIO_PIN_1,
			.mode = GPIO_MODE_AF_PP,
			.pull = GPIO_NOPULL,
			.speed = GPIO_SPEED_FREQ_VERY_HIGH,
			.alternate = GPIO_AF8_UART4
		},
		[IO_UART_4_TX] =
		{
			.port = GPIOA,
			.pin = GPIO_PIN_0,
			.mode = GPIO_MODE_AF_PP,
			.pull = GPIO_NOPULL,
			.speed = GPIO_SPEED_FREQ_VERY_HIGH,
			.alternate = GPIO_AF8_UART4
		},
};

/**
 * TODO
 */
u32_t gpioConfigSize = sizeof(gpioConfig);

GOS_CONST drv_gpioItCallbackDescriptor_t gpioItConfig [] = {};

u32_t gpioItConfigSize = sizeof(gpioItConfig);
