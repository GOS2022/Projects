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

	IO_UART_2_RX,
	IO_UART_2_TX,

	IO_NUMBER
}io_pin_t;

#endif /* DRIVER_INC_IODEF_H_ */
