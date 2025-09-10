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
//! @file       bsp_adc_handler.c
//! @author     Ahmed Gazar
//! @date       2025-09-10
//! @version    1.0
//!
//! @brief      Digital Step Sequencer / Board Support Package / ADC handler source.
//! @details    For a more detailed description of this component, please refer to @ref bsp_adc_handler.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-09-10    Ahmed Gazar     Initial version created.
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
#include "bsp_adc_handler.h"
#include "iodef.h"

/*
 * Type definitions
 */
/**
 * ADC descriptor.
 */
typedef struct
{
	drv_mcp3008Descriptor_t*  pMcpDevice;        //! Pointer to related MCP device descriptor.
	u8_t                      channel;           //! Channel number.
	u16_t                     maxVoltage;        //! Max. voltage in [mV].
	u16_t                     currentVoltage;    //! Currently stored last voltage [mV].
	u16_t                     currentPercentage; //! Current percentage value.
}bsp_adc_handler_channel_desc_t;

/*
 * Static variables
 */
/**
 * ADC device.
 */
GOS_STATIC drv_mcp3008Descriptor_t mcpDeviceDesc =
{
	.spiInstance     = DRV_SPI_INSTANCE_2,
	.csPin           = IO_SPI2_CS3,
	.readMutexTmo    = 1000u,
	.readTriggerTmo  = 2000u,
	.writeMutexTmo   = 1000u,
	.writeTriggerTmo = 2000u,
};

/**
 * ADC mutex.
 */
GOS_STATIC gos_mutex_t adcMutex;

/**
 * ADC map.
 */
GOS_STATIC bsp_adc_handler_channel_desc_t adcMap [] =
{
	[BSP_ADC_CHANNEL_CV_1] =
	{
		.pMcpDevice = NULL,
		.channel    = 0u,
		.maxVoltage = 2500
	},
	[BSP_ADC_CHANNEL_CV_2] =
	{
		.pMcpDevice = NULL,
		.channel    = 1u,
		.maxVoltage = 2500
	},
	[BSP_ADC_CHANNEL_CV_3] =
	{
		.pMcpDevice = NULL,
		.channel    = 2u,
		.maxVoltage = 2500
	},
	[BSP_ADC_CHANNEL_CV_4] =
	{
		.pMcpDevice = NULL,
		.channel    = 3u,
		.maxVoltage = 2500
	},
	[BSP_ADC_CHANNEL_CV_5] =
	{
		.pMcpDevice = NULL,
		.channel    = 4u,
		.maxVoltage = 2500
	},
	[BSP_ADC_CHANNEL_CV_6] =
	{
		.pMcpDevice = NULL,
		.channel    = 5u,
		.maxVoltage = 2500
	},
	[BSP_ADC_CHANNEL_CV_7] =
	{
		.pMcpDevice = NULL,
		.channel    = 6u,
		.maxVoltage = 2500
	},
	[BSP_ADC_CHANNEL_CV_8] =
	{
		.pMcpDevice = NULL,
		.channel    = 7u,
		.maxVoltage = 2500
	},
	[BSP_ADC_CHANNEL_BPM] =
	{
		.pMcpDevice = NULL,
		.channel    = 0u,
		.maxVoltage = 2500
	}
};

/*
 * Function prototypes
 */
GOS_STATIC void_t bsp_adcHandlerTask (void_t);

/**
 * ADC handler task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t adcHandlerTaskDesc =
{
	.taskFunction	    = bsp_adcHandlerTask,
	.taskName		    = "bsp_adc_handler",
	.taskStackSize	    = 0x400,
	.taskPriority	    = 10,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_KERNEL
};

/*
 * Function: bsp_adcHandlerInit
 */
gos_result_t bsp_adcHandlerInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(initResult, gos_taskRegister(&adcHandlerTaskDesc, NULL));
	GOS_CONCAT_RESULT(initResult, gos_mutexInit(&adcMutex));

	return initResult;
}

/*
 * Function: bsp_adcHandlerGetPercentage
 */
gos_result_t bsp_adcHandlerGetPercentage (bsp_adc_channel_def_t channel, u16_t* pPerc)
{
	/*
	 * Local variables.
	 */
	gos_result_t getResult = GOS_ERROR;

	/*
	 * Function code.
	 */
	if ((pPerc != NULL) && (channel < BSP_ADC_CHANNEL_NUM) && (gos_mutexLock(&adcMutex, 100) == GOS_SUCCESS))
	{
		*pPerc = adcMap[channel].currentPercentage;
		getResult = GOS_SUCCESS;
		(void_t) gos_mutexUnlock(&adcMutex);
	}
	else
	{
		// Wrong request.
	}

	return getResult;
}

/**
 * @brief   ADC handler task.
 * @details Periodically reads the configured ADC channel values.
 *
 * @return  -
 */
GOS_STATIC void_t bsp_adcHandlerTask (void_t)
{
	/*
	 * Local variables.
	 */
	u16_t adcVal;
	// TODO:
	u8_t cntr = 0;
	bool_t up = GOS_TRUE;

	/*
	 * Function code.
	 */
	for (;;)
	{
		if (gos_mutexLock(&adcMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
		{
			for (u8_t idx = 0; idx < BSP_ADC_CHANNEL_NUM; idx++)
			{
				// Read raw ADC value.
				(void_t) drv_mcp3008ReadChannel(adcMap[idx].pMcpDevice, adcMap[idx].channel, &adcVal);
				// Convert to voltage.
				adcMap[idx].currentVoltage = (u16_t)((float_t)adcVal / (float_t)(2 ^ 10)) * ((float_t)adcMap[idx].maxVoltage / 5000.0f);
				// Convert to percentage.
				adcMap[idx].currentPercentage = (u16_t)((float_t)adcVal / (float_t)(2 ^ 10) * 100 * 100);

				// TODO: Faking value.
				adcMap[idx].currentPercentage = idx * 100;
			}
			adcMap[BSP_ADC_CHANNEL_BPM].currentPercentage = cntr * 100;
			(void_t) gos_mutexUnlock(&adcMutex);
		}
		else
		{
			// Unexpected error.
		}

		// TODO: simulation.
		if (up == GOS_TRUE && ++cntr == 90)
		{
			up = GOS_FALSE;
		}
		else if (up == GOS_FALSE && --cntr == 10)
		{
			up = GOS_TRUE;
		}

		(void_t) gos_taskSleep(100);
	}
}
