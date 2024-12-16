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
	/*IO_USER_BUTTON,
	IO_BUTTON_1,
	IO_BUTTON_2,
	IO_BUTTON_3,

	IO_DISPL_MOSI,
	IO_DISPL_SCK,
	IO_DISPL_RST,
	IO_DISPL_CS,
	IO_DISPL_DC,

	IO_TOUCH_MISO,
	IO_TOUCH_CS,

	IO_SD_MOSI,
	IO_SD_MISO,
	IO_SD_SCK,
	IO_SD_CS,

	IO_RTC_LCD_SDA,
	IO_RTC_LCD_SCL,

	IO_ANALOG_IN,
	IO_ANALOG_OUT,
	IO_PWM_OUT,*/

	IO_UART_1_RX,
	IO_UART_1_TX,
	//IO_UART_2_RX,
	//IO_UART_2_TX,
	IO_UART_5_RX,
	IO_UART_5_TX,

	IO_UART_4_RX,
	IO_UART_4_TX,

	IO_NUMBER
}io_pin_t;

#endif /* DRIVER_INC_IODEF_H_ */
