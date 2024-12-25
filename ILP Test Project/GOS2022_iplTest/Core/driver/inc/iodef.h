/*
 * iodef.h
 *
 *  Created on: Jun 28, 2023
 *      Author: Gabor
 */

#ifndef DRIVER_INC_IODEF_H_
#define DRIVER_INC_IODEF_H_

#include "gos.h"

typedef enum
{
	IO_USER_LED     = 0u,

	IO_UART_1_RX,
	IO_UART_1_TX,
	IO_UART_5_RX,
	IO_UART_5_TX,

	IO_UART_4_RX,
	IO_UART_4_TX,
	//IO_I2C1_SCL,
	//IO_I2C1_SDA,
	IO_SPI2_MOSI,
	IO_SPI2_MISO,
	IO_SPI2_SCK,
	IO_SPI2_CS0,
	IO_SPI2_CS1,
	IO_SPI2_CS2,

	IO_NUMBER
}io_pin_t;

#endif /* DRIVER_INC_IODEF_H_ */
