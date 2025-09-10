//*************************************************************************************************
//
//                            #####             #####             #####
//                          #########         #########         #########
//                         ##                ##       ##       ##
//                        ##                ##         ##        #####
//                        ##     #####      ##         ##           #####
//                         ##       ##       ##       ##                ##
//                          #########         #########         #########
//                            #####             #####             #####
//
//                                      (c) Ahmed Gazar, 2025
//
//*************************************************************************************************
//! @file       bsp_io_handler.c
//! @author     Ahmed Gazar
//! @date       2025-09-09
//! @version    1.0
//!
//! @brief      Digital Step Sequencer / Board Support Package / IO handler source.
//! @details    For a more detailed description of this component, please refer to @ref bsp_io_handler.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-09-09    Ahmed Gazar     Initial version created.
//*************************************************************************************************
//
// Copyright (c) 2025 Ahmed Gazar
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//*************************************************************************************************
/*
 * Includes
 */
#include "bsp_io_handler.h"
#include "iodef.h"

/*
 * Type definitions
 */
/**
 * IO descriptor.
 */
typedef struct
{
	drv_mcp23017Descriptor_t* pMcpDevice;   //! Pointer to related MCP device descriptor.
	u8_t                      mcpPort;      //! MCP device port.
	drv_mcp23017Pin_t         mcpPin;       //! MCP device pin.
	drv_gpioState_t           prevState;    //! Previous state.
	drv_gpioState_t           currentState; //! Current state.
	bsp_io_handler_edge_t     edge;         //! Edge configuration.
	bsp_io_callback_t         callback;     //! Callback for edge detection.
}bsp_io_handler_io_desc_t;

/*
 * Static variables
 */
/**
 * IO device 0 descriptor (LED drivers).
 */
/*GOS_STATIC drv_mcp23017Descriptor_t mcpDeviceDesc0 =
{
	.deviceAddress   = 0x25,
	.i2cInstance     = DRV_I2C_INSTANCE_1,
	.portADir        = MCP23017_IODIR_ALL_OUTPUT,
	.portBDir        = MCP23017_IODIR_ALL_OUTPUT,
	.portAPol        = MCP23017_IPOL_ALL_NORMAL,
	.portBPol        = MCP23017_IPOL_ALL_NORMAL,
	.readMutexTmo    = GOS_MUTEX_ENDLESS_TMO,
	.readTriggerTmo  = 100u,
	.writeMutexTmo   = GOS_MUTEX_ENDLESS_TMO,
	.writeTriggerTmo = 100u
};*/

/**
 * IO device 1 descriptor (buttons).
 */
/*GOS_STATIC drv_mcp23017Descriptor_t mcpDeviceDesc1 =
{
	.deviceAddress   = 0x26,
	.i2cInstance     = DRV_I2C_INSTANCE_1,
	.portADir        = MCP23017_IODIR_ALL_INPUT,
	.portBDir        = MCP23017_IODIR_ALL_INPUT,
	.portAPol        = MCP23017_IPOL_ALL_NORMAL,
	.portBPol        = MCP23017_IPOL_ALL_NORMAL,
	.readMutexTmo    = GOS_MUTEX_ENDLESS_TMO,
	.readTriggerTmo  = 100u,
	.writeMutexTmo   = GOS_MUTEX_ENDLESS_TMO,
	.writeTriggerTmo = 100u
};*/

// TODO: test IO device.
GOS_STATIC drv_mcp23017Descriptor_t mcpDeviceDesc1 =
{
	.deviceAddress   = 0x20,
	.i2cInstance     = DRV_I2C_INSTANCE_1,
	.portADir        = MCP23017_IODIR_ALL_INPUT,
	.portBDir        = MCP23017_IODIR_ALL_OUTPUT,
	.portAPol        = MCP23017_IPOL_ALL_INVERTED,
	.portBPol        = MCP23017_IPOL_ALL_INVERTED,
	.readMutexTmo    = GOS_MUTEX_ENDLESS_TMO,
	.readTriggerTmo  = 100u,
	.writeMutexTmo   = GOS_MUTEX_ENDLESS_TMO,
	.writeTriggerTmo = 100u
};

/**
 * IO mutex.
 */
GOS_STATIC gos_mutex_t ioMutex;

/**
 * IO map.
 */
GOS_STATIC bsp_io_handler_io_desc_t ioMap [] =
{
	[ BSP_IO_STEP_BUTTON_1 ]     =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_0,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_RISING
	},
	[ BSP_IO_STEP_BUTTON_2 ]     =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_1,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_RISING
	},
	[ BSP_IO_STEP_BUTTON_3 ]     =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_2,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_RISING
	},
	[ BSP_IO_STEP_BUTTON_4 ]     =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_3,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_RISING
	},
	[ BSP_IO_STEP_BUTTON_5 ]     =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_4,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_RISING
	},
	[ BSP_IO_STEP_BUTTON_6 ]     =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_5,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_RISING
	},
	[ BSP_IO_STEP_BUTTON_7 ]     =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_6,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_RISING
	},
	[ BSP_IO_STEP_BUTTON_8 ]     =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_7,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_RISING
	},
	[ BSP_IO_START_STOP_BUTTON ] =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTA,//MCP23017_PORTB,
		.mcpPin       = DRV_MCP23017_PIN_0,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_RISING
	},
	[ BSP_IO_STEP_ON_OFF_LED_1 ] =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB, //MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_0,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_ON_OFF_LED_2 ] =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB, //MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_1,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_ON_OFF_LED_3 ] =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB, //MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_2,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_ON_OFF_LED_4 ] =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB, //MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_3,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_ON_OFF_LED_5 ] =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB, //MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_4,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_ON_OFF_LED_6 ] =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB, //MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_5,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_ON_OFF_LED_7 ] =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB, //MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_6,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_ON_OFF_LED_8 ] =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB, //MCP23017_PORTA,
		.mcpPin       = DRV_MCP23017_PIN_7,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_CNTR_LED_1 ]   =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB,
		.mcpPin       = DRV_MCP23017_PIN_0,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_CNTR_LED_2 ]   =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB,
		.mcpPin       = DRV_MCP23017_PIN_1,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_CNTR_LED_3 ]   =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB,
		.mcpPin       = DRV_MCP23017_PIN_2,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_CNTR_LED_4 ]   =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB,
		.mcpPin       = DRV_MCP23017_PIN_3,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_CNTR_LED_5 ]   =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB,
		.mcpPin       = DRV_MCP23017_PIN_4,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_CNTR_LED_6 ]   =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB,
		.mcpPin       = DRV_MCP23017_PIN_5,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_CNTR_LED_7 ]   =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB,
		.mcpPin       = DRV_MCP23017_PIN_6,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	},
	[ BSP_IO_STEP_CNTR_LED_8 ]   =
	{
		.pMcpDevice   = &mcpDeviceDesc1,
		.mcpPort      = MCP23017_PORTB,
		.mcpPin       = DRV_MCP23017_PIN_7,
		.currentState = GPIO_STATE_LOW,
		.prevState    = GPIO_STATE_LOW,
		.callback     = NULL,
		.edge         = BSP_IO_EDGE_NONE
	}
};

/*
 * Function prototypes
 */
GOS_STATIC void_t bsp_ioHandlerTask (void_t);

/**
 * IO handler task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t ioHandlerTaskDesc =
{
	.taskFunction	    = bsp_ioHandlerTask,
	.taskName		    = "bsp_io_handler",
	.taskStackSize	    = 0x400,
	.taskPriority	    = 24,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_KERNEL
};

/*
 * Function: bsp_ioHandlerInit
 */
gos_result_t bsp_ioHandlerInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(initResult, gos_taskRegister(&ioHandlerTaskDesc, NULL));
	GOS_CONCAT_RESULT(initResult, gos_mutexInit(&ioMutex));

	return initResult;
}

/*
 * Function: bsp_ioHandlerRegisterCallback
 */
gos_result_t bsp_ioHandlerRegisterCallback (bsp_io_handler_def_t pin, bsp_io_callback_t callback)
{
	/*
	 * Local variables.
	 */
	gos_result_t registerResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pin < BSP_IO_TOTAL_NUMBER) && (callback != NULL))
	{
		ioMap[pin].callback = callback;
		registerResult = GOS_SUCCESS;
	}
	else
	{
		// Wrong request.
	}

	return registerResult;
}

/*
 * Function: bsp_ioHandlerGetStepNumber
 */
gos_result_t bsp_ioHandlerGetStepNumber (u8_t* pStepNum)
{
	/*
	 * Local variables.
	 */
	gos_result_t    getResult = GOS_ERROR;
	drv_gpioState_t checkState [8u];
	u8_t            cntr;

	/*
	 * Function code.
	 */
	if (pStepNum != NULL)
	{
		// Default return value.
		*pStepNum = 8u;

		(void_t) drv_gpioReadPin(IO_STEP_CNT_1, &checkState[0]);
		(void_t) drv_gpioReadPin(IO_STEP_CNT_2, &checkState[1]);
		(void_t) drv_gpioReadPin(IO_STEP_CNT_3, &checkState[2]);
		(void_t) drv_gpioReadPin(IO_STEP_CNT_4, &checkState[3]);
		(void_t) drv_gpioReadPin(IO_STEP_CNT_5, &checkState[4]);
		(void_t) drv_gpioReadPin(IO_STEP_CNT_6, &checkState[5]);
		(void_t) drv_gpioReadPin(IO_STEP_CNT_7, &checkState[6]);
		(void_t) drv_gpioReadPin(IO_STEP_CNT_8, &checkState[7]);

		for (cntr = 8u; cntr > 0u; cntr--)
		{
			if (checkState[cntr - 1] == GPIO_STATE_LOW)
			{
				*pStepNum = cntr;
				break;
			}
			else
			{
				// Continue checking which pin is active.
			}
		}

		getResult = GOS_SUCCESS;
	}
	else
	{
		// NULL pointer error.
	}

	return getResult;
}

/*
 * Function: bsp_ioHandlerReadPin
 */
gos_result_t bsp_ioHandlerReadPin (bsp_io_handler_def_t pin, drv_gpioState_t* pState)
{
	/*
	 * Local variables.
	 */
	gos_result_t readResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pState != NULL) && (pin < BSP_IO_TOTAL_NUMBER))
	{
		*pState = ioMap[pin].currentState;
		readResult = GOS_SUCCESS;
	}
	else
	{
		// Wrong request.
	}

	return readResult;
}

/*
 * Function: bsp_ioHandlerWritePin
 */
gos_result_t bsp_ioHandlerWritePin (bsp_io_handler_def_t pin, drv_gpioState_t state)
{
	/*
	 * Function code.
	 */
	return drv_mcp23017WritePin(ioMap[pin].pMcpDevice, ioMap[pin].mcpPort, ioMap[pin].mcpPin, state);
}

/*
 * Function: bsp_ioHandlerTogglePin
 */
gos_result_t bsp_ioHandlerTogglePin (bsp_io_handler_def_t pin)
{
	/*
	 * Local variables.
	 */
	gos_result_t    toggleResult = GOS_SUCCESS;
	drv_gpioState_t state;

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(toggleResult, bsp_ioHandlerReadPin (pin, &state));
	GOS_CONCAT_RESULT(toggleResult, bsp_ioHandlerWritePin (pin, !state));

	return toggleResult;
}

/**
 * @brief   IO handler task.
 * @details Periodically checks the configured IO states and detects state changes.
 *          If a callback is configured for a specific type of state change, it calls
 *          the related callback function.
 *
 * @return  -
 */
GOS_STATIC void_t bsp_ioHandlerTask (void_t)
{
	/*
	 * Local variables.
	 */
	u8_t ioIdx;
	// (void_t) drv_mcp23017Init(&mcpDeviceDesc0);
	(void_t) drv_mcp23017Init(&mcpDeviceDesc1);

	/*
	 * Function code.
	 */
	for (;;)
	{
		// Iterate over IO map and perform the following activities:
		// - Update IO state
		// - Check if state change occurred
		// - Call callback if registered
		for (ioIdx = 0u; ioIdx < BSP_IO_TOTAL_NUMBER; ioIdx++)
		{
			// 1. Update IO state.
			ioMap[ioIdx].prevState = ioMap[ioIdx].currentState;
			(void_t) drv_mcp23017ReadPin(ioMap[ioIdx].pMcpDevice, ioMap[ioIdx].mcpPort, ioMap[ioIdx].mcpPin, &ioMap[ioIdx].currentState);

			// 2. Check state change.
			if (ioMap[ioIdx].prevState != ioMap[ioIdx].currentState)
			{
				// 3. Check if callback exists.
				if (ioMap[ioIdx].callback != NULL)
				{
					switch (ioMap[ioIdx].edge)
					{
						case BSP_IO_EDGE_NONE:
						{
							// No edge configured, nothing to do.
							break;
						}
						case BSP_IO_EDGE_BOTH:
						{
							// Both edges needed, call callback.
							ioMap[ioIdx].callback(ioIdx);
							break;
						}
						case BSP_IO_EDGE_RISING:
						{
							// Rising edge needed, check if it is rising.
							if ((ioMap[ioIdx].prevState == GPIO_STATE_LOW) && (ioMap[ioIdx].currentState == GPIO_STATE_HIGH))
							{
								// Call callback.
								ioMap[ioIdx].callback(ioIdx);
							}
							else
							{
								// Wrong edge.
							}
							break;
						}
						case BSP_IO_EDGE_FALLING:
						{
							// Falling edge needed, check if it is falling.
							if ((ioMap[ioIdx].prevState == GPIO_STATE_HIGH) && (ioMap[ioIdx].currentState == GPIO_STATE_LOW))
							{
								// Call callback.
								ioMap[ioIdx].callback(ioIdx);
							}
							else
							{
								// Wrong edge.
							}
							break;
						}
						default: break;
					}
				}
				else
				{
					// Missing callback.
				}
			}
			else
			{
				// State unchanged.
			}
		}
		(void_t) gos_taskSleep(10);
	}
}
