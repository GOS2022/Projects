/*
 * bsp_exio_handler.c
 *
 *  Created on: Apr 7, 2025
 *      Author: Ahmed
 */
/*
 * Includes
 */
#include "bsp_exio_handler.h"
#include "iodef.h"

/*
 * Macros
 */
/**
 * Maximum number of Ex-IO interrupts.
 */
#define EX_IO_IT_MAX_NUM         ( 8u )

/**
 * TODO
 */
#define EX_IO_READ_PIN_FUNC_IDX  ( 0u )

/**
 * TODO
 */
#define EX_IO_WRITE_PIN_FUNC_IDX ( 0u )

/**
 * TODO
 */
#define EX_IO_WRITE_PORT_FUNC_IDX ( 1u )

/*
 * Type definitions
 */
/**
 * Ex-IO descriptor.
 */
typedef struct
{
	drv_mcp23017Pin_t pin;
	drv_mcp23017Pin_t masterPin;
	drv_gpioState_t   prevState;
}ex_io_desc_t;

/**
 * Ex-IO IT descriptor.
 */
typedef struct
{
	drv_mcp23017Pin_t pin;
	ex_io_edge_t      edge;
	ex_io_callback_t  callback;
}ex_io_it_desc_t;

/*
 * Static variables
 */
/**
 * Ex-IO device descriptor.
 */
GOS_STATIC drv_mcp23017Descriptor_t mcpDeviceDesc =
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
 * Ex-IO mutex.
 */
GOS_STATIC gos_mutex_t exIoMutex;

/**
 * Array of Ex-IO descriptors.
 */
GOS_STATIC ex_io_desc_t exIoDescriptors [DRV_MCP23017_NUM_OF_PINS] =
{
	{ .masterPin = GPIO_EX_PIN_0, .pin = GPIO_EX_PIN_3 },
	{ .masterPin = GPIO_EX_PIN_1, .pin = GPIO_EX_PIN_2 },
	{ .masterPin = GPIO_EX_PIN_2, .pin = GPIO_EX_PIN_1 },
	{ .masterPin = GPIO_EX_PIN_3, .pin = GPIO_EX_PIN_0 },
	{ .masterPin = GPIO_EX_PIN_4, .pin = GPIO_EX_PIN_7 },
	{ .masterPin = GPIO_EX_PIN_5, .pin = GPIO_EX_PIN_6 },
	{ .masterPin = GPIO_EX_PIN_6, .pin = GPIO_EX_PIN_4 },
	{ .masterPin = GPIO_EX_PIN_7, .pin = GPIO_EX_PIN_5 }
};

/**
 * Array of Ex-IO IT descriptors.
 */
GOS_STATIC ex_io_it_desc_t exIoItDescriptors [EX_IO_IT_MAX_NUM];

/*
 * Function prototypes
 */
GOS_STATIC void_t       bsp_exioHandlerTask      (void_t);
GOS_STATIC gos_result_t bsp_exioReadPinWrapper   (u8_t params, va_list args);
GOS_STATIC gos_result_t bsp_exioWritePinWrapper  (u8_t params, va_list args);
GOS_STATIC gos_result_t bsp_exioWritePortWrapper (u8_t params, va_list args);
GOS_STATIC gos_result_t bsp_exioErrorHandler     (void_t* pDevice);

/**
 * Ex-IO task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t exIoTaskDesc =
{
	.taskFunction	    = bsp_exioHandlerTask,
	.taskName		    = "bsp_ex_io_handler",
	.taskStackSize	    = 0x400,
	.taskPriority	    = 24,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_KERNEL
};

/**
 * Standard device descriptor for ex-io device.
 */
GOS_STATIC svl_dhsDevice_t exIoDevice =
{
	.name                                      = "ex_io_device",
	.description                               = "External MCP23017 over I2C. Address: 0x20.",
	.pInitializer                              = drv_mcp23017Init,
	.pErrorHandler                             = bsp_exioErrorHandler,
	.recoveryType                              = DHS_RECOVERY_ON_LIMIT,
	.pDeviceDescriptor                         = (void_t*)&mcpDeviceDesc,
	.readFunctions[EX_IO_READ_PIN_FUNC_IDX]    = bsp_exioReadPinWrapper,
	.writeFunctions[EX_IO_WRITE_PIN_FUNC_IDX]  = bsp_exioWritePinWrapper,
	.writeFunctions[EX_IO_WRITE_PORT_FUNC_IDX] = bsp_exioWritePortWrapper,
	.enabled                                   = GOS_TRUE,
	.errorTolerance                            = 4
};

/*
 * Function: bsp_exioHandlerInit
 */
gos_result_t bsp_exioHandlerInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(initResult, gos_taskRegister(&exIoTaskDesc, NULL));
	GOS_CONCAT_RESULT(initResult, gos_mutexInit(&exIoMutex));
	GOS_CONCAT_RESULT(initResult, svl_dhsRegisterDevice(&exIoDevice));

	return initResult;
}

/*
 * Function: bsp_exioRegisterCallback
 */
gos_result_t bsp_exioRegisterCallback (drv_mcp23017Pin_t pin, ex_io_edge_t edge, ex_io_callback_t callback)
{
	/*
	 * Local variables.
	 */
	gos_result_t registerResult = GOS_ERROR;
	u8_t         index          = 0u;

	/*
	 * Function code.
	 */
	if ((pin <= GPIO_EX_PIN_7) && (callback != NULL))
	{
		for (index = 0u; index < EX_IO_IT_MAX_NUM; index++)
		{
			if (exIoItDescriptors[index].callback == NULL)
			{
				exIoItDescriptors[index].pin      = pin;
				exIoItDescriptors[index].edge     = edge;
				exIoItDescriptors[index].callback = callback;
				registerResult = GOS_SUCCESS;
				break;
			}
			else
			{
				// Continue.
			}
		}
	}
	else
	{
		// Error.
	}

	return registerResult;
}

/*
 * Function: bsp_exioUnregisterCallback
 */
gos_result_t bsp_exioUnregisterCallback (drv_mcp23017Pin_t pin, ex_io_edge_t edge, ex_io_callback_t callback)
{
	/*
	 * Local variables.
	 */
	gos_result_t registerResult = GOS_ERROR;
	u8_t         index          = 0u;

	/*
	 * Function code.
	 */
	if ((pin <= GPIO_EX_PIN_7) && (callback != NULL))
	{
		for (index = 0u; index < EX_IO_IT_MAX_NUM; index++)
		{
			if (exIoItDescriptors[index].pin      == pin  &&
				exIoItDescriptors[index].edge     == edge &&
				exIoItDescriptors[index].callback == callback)
			{
				exIoItDescriptors[index].pin      = 0;
				exIoItDescriptors[index].edge     = 0;
				exIoItDescriptors[index].callback = NULL;
				registerResult = GOS_SUCCESS;
				break;
			}
			else
			{
				// Continue.
			}
		}
	}
	else
	{
		// Error.
	}

	return registerResult;
}

/*
 * Function: bsp_exioWritePin
 */
gos_result_t bsp_exioWritePin (drv_mcp23017Pin_t pin, drv_gpioState_t state)
{
	/*
	 * Function code.
	 */
	return svl_dhsWriteDevice(exIoDevice.deviceId, EX_IO_WRITE_PIN_FUNC_IDX, 3,
			exIoDevice.pDeviceDescriptor, pin, state);
}

/*
 * Function: bsp_exioReadPin
 */
gos_result_t bsp_exioReadPin (drv_mcp23017Pin_t pin, drv_gpioState_t* pState)
{
	/*
	 * Function code.
	 */
	return svl_dhsReadDevice(exIoDevice.deviceId, EX_IO_READ_PIN_FUNC_IDX, 3,
			exIoDevice.pDeviceDescriptor, pin, pState);
}

/*
 * Function: bsp_exioTogglePin
 */
gos_result_t bsp_exioTogglePin (drv_mcp23017Pin_t pin)
{
	/*
	 * Local variables.
	 */
	gos_result_t    toggleResult = GOS_SUCCESS;
	drv_gpioState_t pinState;

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(toggleResult, bsp_exioReadPin(pin, &pinState));
	GOS_CONCAT_RESULT(toggleResult, bsp_exioWritePin(pin, !pinState));

	return toggleResult;
}

/*
 * Function: bsp_exioWritePort
 */
gos_result_t bsp_exioWritePort (u8_t portState)
{
	/*
	 * Function code.
	 */
	return svl_dhsWriteDevice(exIoDevice.deviceId, EX_IO_WRITE_PORT_FUNC_IDX, 2,
			exIoDevice.pDeviceDescriptor, portState);
}

/**
 * TODO
 * @param
 * @return
 */
GOS_STATIC void_t bsp_exioHandlerTask (void_t)
{
	/*
	 * Local variables.
	 */
	drv_gpioState_t ioState = GPIO_STATE_LOW;

	/*
	 * Function code.
	 */
//	(void_t) svl_dhsRegisterDevice(&exIoDevice);

	for (;;)
	{
		for (int i = 0; i < DRV_MCP23017_NUM_OF_PINS; i++)
		{
			if (gos_mutexLock(&exIoMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
			{
				(void_t) bsp_exioReadPin(exIoDescriptors[i].masterPin, &ioState);
				(void_t) gos_mutexUnlock(&exIoMutex);

				if (ioState != exIoDescriptors[i].prevState)
				{
					for (int j = 0; j < EX_IO_IT_MAX_NUM; j++)
					{
						if (exIoItDescriptors[j].pin    == exIoDescriptors[i].masterPin &&
							((exIoItDescriptors[j].edge == GPIO_EX_EDGE_BOTH) ||
							(ioState == GPIO_STATE_HIGH && exIoItDescriptors[j].edge == GPIO_EX_EDGE_RISING) ||
							(ioState == GPIO_STATE_LOW  && exIoItDescriptors[j].edge == GPIO_EX_EDGE_FALLING)))
						{
							exIoItDescriptors[j].callback();
						}
						else
						{
							// No edge detected.
						}
					}

					exIoDescriptors[i].prevState = ioState;
				}
				else
				{
					// No change in state.
				}
			}
			else
			{
				// Mutex error.
			}
		}
		(void_t) gos_taskSleep(50);
	}
}

GOS_STATIC gos_result_t bsp_exioWritePinWrapper (u8_t params, va_list args)
{
	/*
	 * Local variables.
	 */
	void_t* device;
	drv_mcp23017Pin_t pin;
	drv_gpioState_t state;

	/*
	 * Function code.
	 */
	device = (void_t*)va_arg(args, int);
	pin = (drv_mcp23017Pin_t)va_arg(args, int);
	state = (drv_gpioState_t)va_arg(args, int);

	return drv_mcp23017WritePin(device, MCP23017_PORTB, pin, state);
}

GOS_STATIC gos_result_t bsp_exioReadPinWrapper (u8_t params, va_list args)
{
	/*
	 * Local variables.
	 */
	void_t* device;
	drv_mcp23017Pin_t pin;
	drv_gpioState_t* pState;

	/*
	 * Function code.
	 */
	device = (void_t*)va_arg(args, int);
	pin = (drv_mcp23017Pin_t)va_arg(args, int);
	pState = (drv_gpioState_t*)va_arg(args, int);

	return drv_mcp23017ReadPin(device, MCP23017_PORTA, pin, pState);
}

GOS_STATIC gos_result_t bsp_exioWritePortWrapper (u8_t params, va_list args)
{
	/*
	 * Local variables.
	 */
	void_t* device;
	u8_t portState;

	/*
	 * Function code.
	 */
	device = (void_t*)va_arg(args, int);
	portState = (u8_t)va_arg(args, int);

	return drv_mcp23017WritePort(device, MCP23017_PORTB, portState);
}

GOS_STATIC gos_result_t bsp_exioErrorHandler (void_t* pDevice)
{
	/*
	 * Local variables.
	 */
	u8_t index;
	gos_result_t errorHandlerResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(errorHandlerResult, drv_i2cDeInitInstance(DRV_I2C_INSTANCE_1));
	GOS_CONCAT_RESULT(errorHandlerResult, drv_i2cInitInstance(DRV_I2C_INSTANCE_1));

	GOS_DISABLE_SCHED

	(void_t) drv_gpioWritePin(IO_I2C1_SDA, GPIO_STATE_HIGH);
	(void_t) drv_gpioWritePin(IO_I2C1_SCL, GPIO_STATE_HIGH);
	(void_t) gos_kernelDelayUs(100);
	(void_t) drv_gpioWritePin(IO_I2C1_SDA, GPIO_STATE_LOW);
	(void_t) gos_kernelDelayUs(100);
	(void_t) drv_gpioWritePin(IO_I2C1_SCL, GPIO_STATE_LOW);
	(void_t) gos_kernelDelayUs(100);
	(void_t) drv_gpioWritePin(IO_I2C1_SDA, GPIO_STATE_HIGH);
	(void_t) gos_kernelDelayUs(100);

	for (index = 0u; index < 19u; index++)
	{
		(void_t) drv_gpioTgglePin(IO_I2C1_SCL);
		(void_t) gos_kernelDelayUs(100);
	}

	(void_t) drv_gpioWritePin(IO_I2C1_SCL, GPIO_STATE_LOW);
	(void_t) gos_kernelDelayUs(100);
	(void_t) drv_gpioWritePin(IO_I2C1_SCL, GPIO_STATE_HIGH);
	(void_t) gos_kernelDelayUs(100);
	(void_t) drv_gpioWritePin(IO_I2C1_SDA, GPIO_STATE_LOW);
	(void_t) gos_kernelDelayUs(100);
	(void_t) drv_gpioWritePin(IO_I2C1_SCL, GPIO_STATE_HIGH);
	(void_t) gos_kernelDelayUs(100);
	(void_t) drv_gpioWritePin(IO_I2C1_SDA, GPIO_STATE_HIGH);
	(void_t) gos_kernelDelayUs(100);

	GOS_CONCAT_RESULT(errorHandlerResult, drv_mcp23017Init(pDevice));

	GOS_ENABLE_SCHED

	return errorHandlerResult;
}
