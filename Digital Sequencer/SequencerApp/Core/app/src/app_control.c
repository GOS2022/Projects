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
//! @file       app_control.c
//! @author     Ahmed Gazar
//! @date       2025-09-09
//! @version    1.0
//!
//! @brief      Digital Step Sequencer / Application / Control source.
//! @details    For a more detailed description of this component, please refer to @ref app_control.h
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
#include "app_control.h"
#include "iodef.h" // TODO: remove.
#include "bsp_io_handler.h"
#include "bsp_adc_handler.h"
#include "bsp_tmr_handler.h"

/*
 * Macros
 */
/**
 * Min. BPM value that can be set.
 */
#define APP_CNTRL_BPM_MIN ( 20u )
/**
 * Max. BPM value that can be set.
 */
#define APP_CNTRL_BPM_MAX ( 1000u )

/*
 * Type definitions
 */
/**
 * Control state enum.
 */
typedef enum
{
	APP_CONTROL_STATE_STDBY,        //!< Sequencer in standby mode.
	APP_CONTROL_STATE_SEQ,          //!< Sequencer is running.
	APP_CONTROL_STATE_REMOTE_STDBY, //!< Sequencer is in remote control standby mode.
	APP_CONTROL_STATE_REMOTE_SEQ    //!< Sequencer is running in remote mode.
}app_controlState_t;

/**
 * Control step enum.
 */
typedef enum
{
	APP_CONTROL_STEP_1,             //!< Step 1.
	APP_CONTROL_STEP_2,             //!< Step 2.
	APP_CONTROL_STEP_3,             //!< Step 3.
	APP_CONTROL_STEP_4,             //!< Step 4.
	APP_CONTROL_STEP_5,             //!< Step 5.
	APP_CONTROL_STEP_6,             //!< Step 6.
	APP_CONTROL_STEP_7,             //!< Step 7.
	APP_CONTROL_STEP_8,             //!< Step 8.
	APP_CONTROL_STEP_NUM            //!< Number of steps.
}app_controlStep_t;

/*
 * Static variables
 */
/**
 * State of each step (on/off).
 */
GOS_STATIC bool_t             stepStates [APP_CONTROL_STEP_NUM];

/**
 * State of internal state machine.
 */
GOS_STATIC app_controlState_t state              = APP_CONTROL_STATE_STDBY;

/**
 * Current step counter.
 */
GOS_STATIC u8_t               currentStepCounter = 0u;

/**
 * Control trigger.
 */
GOS_STATIC gos_trigger_t      controlTrigger;

/**
 * Number of steps.
 */
GOS_STATIC u8_t               stepNumber;

/**
 * Currently set BPM value.
 */
GOS_STATIC u8_t               currentBpm;

/**
 * Current period value to wait (ticks).
 */
GOS_STATIC u32_t              currentPeriod;

/**
 * Current period counter (ticks).
 */
GOS_STATIC u32_t              periodCounter;

/*
 * Function prototypes
 */
GOS_STATIC gos_result_t app_controlSetBpm            (u16_t bpm);
GOS_STATIC void_t       app_controlIoPressedCallback (bsp_io_handler_def_t pin);
GOS_STATIC void_t       app_controlTask              (void_t);

/**
 * Control task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t controlHandlerTaskDesc =
{
	.taskFunction	    = app_controlTask,
	.taskName		    = "app_control_task",
	.taskStackSize	    = 0x400,
	.taskPriority	    = 8,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_KERNEL
};

/*
 * Function: app_controlInit
 */
gos_result_t app_controlInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(initResult, bsp_ioHandlerRegisterCallback(BSP_IO_START_STOP_BUTTON, app_controlIoPressedCallback));
	GOS_CONCAT_RESULT(initResult, bsp_ioHandlerRegisterCallback(BSP_IO_STEP_BUTTON_1,     app_controlIoPressedCallback));
	GOS_CONCAT_RESULT(initResult, bsp_ioHandlerRegisterCallback(BSP_IO_STEP_BUTTON_2,     app_controlIoPressedCallback));
	GOS_CONCAT_RESULT(initResult, bsp_ioHandlerRegisterCallback(BSP_IO_STEP_BUTTON_3,     app_controlIoPressedCallback));
	GOS_CONCAT_RESULT(initResult, bsp_ioHandlerRegisterCallback(BSP_IO_STEP_BUTTON_4,     app_controlIoPressedCallback));
	GOS_CONCAT_RESULT(initResult, bsp_ioHandlerRegisterCallback(BSP_IO_STEP_BUTTON_5,     app_controlIoPressedCallback));
	GOS_CONCAT_RESULT(initResult, bsp_ioHandlerRegisterCallback(BSP_IO_STEP_BUTTON_6,     app_controlIoPressedCallback));
	GOS_CONCAT_RESULT(initResult, bsp_ioHandlerRegisterCallback(BSP_IO_STEP_BUTTON_7,     app_controlIoPressedCallback));
	GOS_CONCAT_RESULT(initResult, bsp_ioHandlerRegisterCallback(BSP_IO_STEP_BUTTON_8,     app_controlIoPressedCallback));
	GOS_CONCAT_RESULT(initResult, gos_triggerInit(&controlTrigger));
	GOS_CONCAT_RESULT(initResult, gos_taskRegister(&controlHandlerTaskDesc, NULL));

	return initResult;
}

/*
 * Function: app_controlStepTriggerCallback
 */
void_t app_controlStepTriggerCallback (void_t)
{
	/*
	 * Function code.
	 */
	periodCounter++;
	if (periodCounter >= currentPeriod)
	{
		(void_t) gos_triggerIncrement(&controlTrigger);
		periodCounter = 0u;
	}
	else
	{
		// Keep counting.
	}
}

/**
 * @brief     Sets the requested BPM.
 * @details   Checks the min. and max. values for the requested
 *            BPM value, and limits it in case it is out of
 *            range. Then calculates the frequency value from the
 *            BPM value, and then converts it to period time in [ms].
 *            The calculated value is stored in a shared variable that
 *            is used to update the timing mechanism.
 *
 * @param[in] bpm BPM to be set.
 *
 * @return    Success of BPM setting.
 *
 * @retval    #GOS_SUCCESS BPM set successfully.
 */
GOS_STATIC gos_result_t app_controlSetBpm (u16_t bpm)
{
	/*
	 * Local variables.
	 */
	float_t frequency;

	/*
	 * Function code.
	 */
	if (bpm > APP_CNTRL_BPM_MAX)
	{
		bpm = APP_CNTRL_BPM_MAX;
	}
	else if (bpm < APP_CNTRL_BPM_MIN)
	{
		bpm = APP_CNTRL_BPM_MIN;
	}
	else
	{
		// Requested BPM in range.
	}

	// Calculate frequency.
	frequency = (float_t)bpm / 60.0f;
	// Calculate period.
	currentPeriod = (u32_t) (1000.0f / frequency);

	return GOS_SUCCESS;
}

/**
 * @brief     Determines the required BPM value.
 * @details   Receives the BPM percentage setting from BSP, and
 *            converts it to actual BPM value based on the configured
 *            minimum and maximum values.
 *
 * @param[in] pBpm Pointer to a variable to store the calculated BPM.
 *
 * @return    Result of getting.
 *
 * @retval    #GOS_SUCCESS BPM calculated and stored successfully.
 * @retval    #GOS_ERROR   Input parameter is NULL pointer.
 */
GOS_STATIC gos_result_t app_controlGetCurrentBpm (u16_t *pBpm)
{
	/*
	 * Local variables.
	 */
	gos_result_t getResult = GOS_ERROR;
	u16_t        bpmPercentage;

	/*
	 * Function code.
	 */
	if (*pBpm != NULL)
	{
		// Get BPM setting.
		(void_t) bsp_adcHandlerGetPercentage(BSP_ADC_CHANNEL_BPM, &bpmPercentage);
		// Convert percentage to BPM.
		*pBpm = (u16_t) ((float_t)bpmPercentage / (100.0f * 100.0f) * (APP_CNTRL_BPM_MAX - APP_CNTRL_BPM_MIN) + APP_CNTRL_BPM_MIN);
		getResult = GOS_SUCCESS;
	}
	else
	{
		// NULL pointer error.
	}
	return getResult;
}

/**
 * @brief     IO pressed callback from BSP.
 * @details   Checks the source of the callback, and handles it accordingly:
 *            - Start/stop button will trigger starting or stopping sequencing
 *            - Other buttons set the on/off state for each step
 *
 * @param[in] pin Pin that generated the callback.
 *
 * @return    -
 */
GOS_STATIC void_t app_controlIoPressedCallback (bsp_io_handler_def_t pin)
{
	/*
	 *  Local variables.
	 */
	u16_t bpmToSet;
	u8_t  idx;

	/*
	 * Function code.
	 */
	if ((state == APP_CONTROL_STATE_SEQ) || (state == APP_CONTROL_STATE_STDBY))
	{
		switch (pin)
		{
			case BSP_IO_START_STOP_BUTTON:
			{
				// Check current state.
				if (state == APP_CONTROL_STATE_STDBY)
				{
					// Get number of steps.
					(void_t) bsp_ioHandlerGetStepNumber(&stepNumber);
					// Get BPM setting.
					(void_t) app_controlGetCurrentBpm(&bpmToSet);
					// Set BPM.
					(void_t) app_controlSetBpm(bpmToSet);
					// Clear period counter.
					periodCounter = 0u;
					// Start timer.
					(void_t) bsp_tmrHandlerStartStepTimer();
					// Set running flag.
					state = APP_CONTROL_STATE_SEQ;
				}
				else if (state == APP_CONTROL_STATE_SEQ)
				{
					// Stop timer.
					(void_t) bsp_tmrHandlerStopStepTimer();
					// Reset counter and LEDs.
					currentStepCounter = 0u;
					for (idx = 0u; idx < APP_CONTROL_STEP_NUM; idx++)
					{
						(void_t) bsp_ioHandlerWritePin((bsp_io_handler_def_t)(BSP_IO_STEP_CNTR_LED_1 + idx), GPIO_STATE_LOW);
					}
					// Reset running flag.
					state = APP_CONTROL_STATE_STDBY;
				}
				else
				{
					// Remote control active.
					// Operations are not allowed.
				}
				break;
			}
			case BSP_IO_STEP_BUTTON_1:
			{
				stepStates[APP_CONTROL_STEP_1] = stepStates[APP_CONTROL_STEP_1] == GOS_TRUE ? GOS_FALSE : GOS_TRUE;
				(void_t) bsp_ioHandlerTogglePin(BSP_IO_STEP_ON_OFF_LED_1);
				break;
			}
			case BSP_IO_STEP_BUTTON_2:
			{
				stepStates[APP_CONTROL_STEP_2] = stepStates[APP_CONTROL_STEP_2] == GOS_TRUE ? GOS_FALSE : GOS_TRUE ;
				(void_t) bsp_ioHandlerTogglePin(BSP_IO_STEP_ON_OFF_LED_2);
				break;
			}
			case BSP_IO_STEP_BUTTON_3:
			{
				stepStates[APP_CONTROL_STEP_3] = stepStates[APP_CONTROL_STEP_3] == GOS_TRUE ? GOS_FALSE : GOS_TRUE;
				(void_t) bsp_ioHandlerTogglePin(BSP_IO_STEP_ON_OFF_LED_3);
				break;
			}
			case BSP_IO_STEP_BUTTON_4:
			{
				stepStates[APP_CONTROL_STEP_4] = stepStates[APP_CONTROL_STEP_4] == GOS_TRUE ? GOS_FALSE : GOS_TRUE;
				(void_t) bsp_ioHandlerTogglePin(BSP_IO_STEP_ON_OFF_LED_4);
				break;
			}
			case BSP_IO_STEP_BUTTON_5:
			{
				stepStates[APP_CONTROL_STEP_5] = stepStates[APP_CONTROL_STEP_5] == GOS_TRUE ? GOS_FALSE : GOS_TRUE;
				(void_t) bsp_ioHandlerTogglePin(BSP_IO_STEP_ON_OFF_LED_5);
				break;
			}
			case BSP_IO_STEP_BUTTON_6:
			{
				stepStates[APP_CONTROL_STEP_6] = stepStates[APP_CONTROL_STEP_6] == GOS_TRUE ? GOS_FALSE : GOS_TRUE;
				(void_t) bsp_ioHandlerTogglePin(BSP_IO_STEP_ON_OFF_LED_6);
				break;
			}
			case BSP_IO_STEP_BUTTON_7:
			{
				stepStates[APP_CONTROL_STEP_7] = stepStates[APP_CONTROL_STEP_7] == GOS_TRUE ? GOS_FALSE : GOS_TRUE;
				(void_t) bsp_ioHandlerTogglePin(BSP_IO_STEP_ON_OFF_LED_7);
				break;
			}
			case BSP_IO_STEP_BUTTON_8:
			{
				stepStates[APP_CONTROL_STEP_8] = stepStates[APP_CONTROL_STEP_8] == GOS_TRUE ? GOS_FALSE : GOS_TRUE;
				(void_t) bsp_ioHandlerTogglePin(BSP_IO_STEP_ON_OFF_LED_8);
				break;
			}
			default: break;
		}
	}
	else
	{
		// Remote control is on, operation not allowed.
	}
}

/**
 * @brief   Main control task.
 * @details Updates the required BPM and step number.
 *          When a trigger is received from the HW timer interrupt,
 *          it updates the current step LEDs, and sets the outputs.
 *
 * @return  -
 */
GOS_STATIC void_t app_controlTask (void_t)
{
	/*
	 * Local variables.
	 */
	u16_t currentCV;
	u16_t bpmPercentage;
	u16_t bpmToSet;

	/*
	 * Function code.
	 */
	for(;;)
	{
		// Get current BPM setting.
		(void_t) app_controlGetCurrentBpm(&bpmToSet);
		// Set BPM.
		(void_t) app_controlSetBpm(bpmToSet);
		// Read and adjust step number.
		(void_t) bsp_ioHandlerGetStepNumber(&stepNumber);

		// Check step trigger.
		if (gos_triggerWait(&controlTrigger, 1u, 10u) == GOS_SUCCESS)
		{
			// TODO: remove.
			(void_t) drv_gpioTgglePin(IO_WARN_LED);

			// Turn off current LED.
			(void_t) bsp_ioHandlerWritePin((bsp_io_handler_def_t)(BSP_IO_STEP_CNTR_LED_1 + currentStepCounter), GPIO_STATE_LOW);
			// Increase step counter with overflow handling.
			currentStepCounter++;
			if (currentStepCounter >= stepNumber)
			{
				currentStepCounter = 0u;
			}
			else
			{
				// Within range.
			}
			// Turn on current LED.
			(void_t) bsp_ioHandlerWritePin((bsp_io_handler_def_t)(BSP_IO_STEP_CNTR_LED_1 + currentStepCounter), GPIO_STATE_HIGH);

			// Toggle clock output.

			// Handle trigger and gate output.

			// Update CV output.
			(void_t) bsp_adcHandlerGetPercentage((bsp_adc_channel_def_t)(BSP_ADC_CHANNEL_CV_1 + currentStepCounter), &currentCV);

			// Reset trigger.
			(void_t) gos_triggerReset(&controlTrigger);
		}
		else
		{
			// Trigger not ready.
		}
	}
}
