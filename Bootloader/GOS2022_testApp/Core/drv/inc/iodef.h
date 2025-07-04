/*
 * iodef.h
 *
 *  Created on: Jun 28, 2023
 *      Author: Gabor
 */

#ifndef DRV_INC_IODEF_H_
#define DRV_INC_IODEF_H_

#include "gos.h"

typedef enum
{
	IO_USER_LED     = 0u,
	IO_USER_BUTTON,

	IO_UART_1_RX,
	IO_UART_1_TX,
	IO_UART_5_RX,
	IO_UART_5_TX,

	IO_UART_4_RX,
	IO_UART_4_TX,

	IO_SPI2_MOSI,
	IO_SPI2_MISO,
	IO_SPI2_SCK,
	IO_SPI2_CS0,
	IO_SPI2_CS1,
	IO_SPI2_CS2,

	IO_I2C1_SCL,
	IO_I2C1_SDA,

	IO_WEMOS_RST,

	IO_DISPL_MOSI,
	IO_DISPL_SCK,
	IO_DISPL_RST,
	IO_DISPL_CS,
	IO_DISPL_DC,
	IO_DISPL_BNESS,

	IO_TOUCH_MISO,
	IO_TOUCH_CS,

	IO_U_MEAS_IN,
	IO_I_MEAS_IN,

	IO_NUMBER
}io_pin_t;

#endif /* DRV_INC_IODEF_H_ */
