/*
 * bsp_exio_handler.h
 *
 *  Created on: Apr 8, 2025
 *      Author: Ahmed
 */

#ifndef INC_BSP_EXIO_HANDLER_H_
#define INC_BSP_EXIO_HANDLER_H_

#include "gos_lib.h"

#define GPIO_EX_PIN_0  ( DRV_MCP23017_PIN_0 )
#define GPIO_EX_PIN_1  ( DRV_MCP23017_PIN_1 )
#define GPIO_EX_PIN_2  ( DRV_MCP23017_PIN_2 )
#define GPIO_EX_PIN_3  ( DRV_MCP23017_PIN_3 )
#define GPIO_EX_PIN_4  ( DRV_MCP23017_PIN_4 )
#define GPIO_EX_PIN_5  ( DRV_MCP23017_PIN_5 )
#define GPIO_EX_PIN_6  ( DRV_MCP23017_PIN_6 )
#define GPIO_EX_PIN_7  ( DRV_MCP23017_PIN_7 )

typedef enum
{
	GPIO_EX_EDGE_FALLING,
	GPIO_EX_EDGE_RISING,
	GPIO_EX_EDGE_BOTH
}ex_io_edge_t;

typedef void_t (*ex_io_callback_t)(void_t);

gos_result_t bsp_exioHandlerInit (void_t);
gos_result_t bsp_exioRegisterCallback (drv_mcp23017Pin_t pin, ex_io_edge_t edge, ex_io_callback_t callback);
gos_result_t bsp_exioUnregisterCallback (drv_mcp23017Pin_t pin, ex_io_edge_t edge, ex_io_callback_t callback);
gos_result_t bsp_exioWritePin (drv_mcp23017Pin_t pin, drv_gpioState_t state);
gos_result_t bsp_exioReadPin (drv_mcp23017Pin_t pin, drv_gpioState_t* pState);
gos_result_t bsp_exioTogglePin (drv_mcp23017Pin_t pin);
gos_result_t bsp_exioWritePort (u8_t portState);

#endif /* INC_BSP_EXIO_HANDLER_H_ */
