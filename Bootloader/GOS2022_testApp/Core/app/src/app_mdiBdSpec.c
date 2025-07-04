/*
 * app_mdiBdSpec.c
 *
 *  Created on: Jan 27, 2025
 *      Author: Ahmed
 */

/*
 * Includes
 */
#include "app.h"
#include <gos_lib.h>
#include "mdi_def.h"
#include "math.h"

/*
 * Macros
 */
#define VREFINT_CAL_ADDR     ( (u16_t*) (0x1FFF7A2AU) )
#define VREFINT_CAL_VREF     ( 3300UL )
#define ADC_RESOLUTION       ( 4095 )
#define TEMPSENSOR_CAL2_TEMP ( (s32_t)  110 )
#define TEMPSENSOR_CAL1_TEMP ( (s32_t)  30  )
#define TEMPSENSOR_CAL2_ADDR ( (u16_t*) (0x1FFF7A2EU) )
#define TEMPSENSOR_CAL1_ADDR ( (u16_t*) (0x1FFF7A2CU) )
#define ADC_SAMPLES          ( 8 )

/*
 * Global variables
 */
gos_mutex_t mdiMutex;

/*
 * Function prototypes
 */
GOS_STATIC void_t app_mdiTask (void_t);

/**
 * APP MDI task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t mdiTaskDesc =
{
	.taskFunction	    = app_mdiTask,
	.taskName 		    = "app_mdiTask",
	.taskStackSize 	    = 0x600,
	.taskPriority 	    = 15,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_USER
};

GOS_EXTERN svl_mdiVariable_t mdiVariables [];

/*
 * Function: app_mdiBdSpecInit
 */
gos_result_t app_mdiBdSpecInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t initResult = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	initResult &= gos_mutexInit(&mdiMutex);
	initResult &= gos_taskRegister(&mdiTaskDesc, NULL);

	if (initResult != GOS_SUCCESS)
	{
		initResult = GOS_ERROR;
	}

	return initResult;
}

// TODO
GOS_STATIC void_t app_mdiTask (void_t)
{
	/*
	 * Local variables.
	 */
	u16_t   adcResult [4];
	float_t vdda;
	float_t vref, u_pwr, i_pwr;
	float_t temp;
	u16_t   rtcTemp;
	float_t adcRes1_avg;
	float_t adcRes2_avg;
	float_t adcRes3_avg;
	float_t adcRes4_avg;

	/*
	 * Function code.
	 */
	(void_t) gos_taskSleep(1500);

	for (;;)
	{
		adcRes1_avg = 0;
		adcRes2_avg = 0;
		adcRes3_avg = 0;
		adcRes4_avg = 0;

		for (u16_t i = 0; i < ADC_SAMPLES; ++i)
		{
			drv_adcGetValueIT(0, &adcResult[0], 1000, 1000);
			drv_adcGetValueIT(1, &adcResult[1], 1000, 1000);
			drv_adcGetValueIT(2, &adcResult[2], 1000, 1000);
			drv_adcGetValueIT(3, &adcResult[3], 1000, 1000);

			adcRes1_avg += adcResult[0];
			adcRes2_avg += adcResult[1];
			adcRes3_avg += adcResult[2];
			adcRes4_avg += adcResult[3];

			gos_taskYield();
		}

		adcRes1_avg /= ADC_SAMPLES;
		adcRes2_avg /= ADC_SAMPLES;
		adcRes3_avg /= ADC_SAMPLES;
		adcRes4_avg /= ADC_SAMPLES;

		adcResult[0] = (u16_t)adcRes1_avg;
		adcResult[1] = (u16_t)adcRes2_avg;
		adcResult[2] = (u16_t)adcRes3_avg;
		adcResult[3] = (u16_t)adcRes4_avg;


		vdda = (float_t) VREFINT_CAL_VREF * (float_t) *VREFINT_CAL_ADDR / adcResult[1] / 1000;
		vref = (float_t) vdda / ADC_RESOLUTION * adcResult[1];

	    temp = (float_t) ((float_t) ((float_t) (TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP)
	                           / (float_t) (*TEMPSENSOR_CAL2_ADDR - *TEMPSENSOR_CAL1_ADDR))
	                           * (adcResult[0] - *TEMPSENSOR_CAL1_ADDR) + TEMPSENSOR_CAL1_TEMP);
	    (void_t) bsp_rtcHandlerGetTemperature(&rtcTemp);
	    u_pwr = 2 * vdda * adcResult[2] / ADC_RESOLUTION;
	    i_pwr = (-1)*(1000 * ((vdda * adcResult[3] / ADC_RESOLUTION) - 2.5) / 0.185);

	    if (gos_mutexLock(&mdiMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
	    {
		    mdiVariables[MDI_VDDA].value.floatValue     = roundf(vdda * 100) / 100;
			mdiVariables[MDI_VREF].value.floatValue     = roundf(vref * 100) / 100;
			mdiVariables[MDI_CPU_TEMP].value.floatValue = roundf(temp * 10) / 10;
			mdiVariables[MDI_RTC_TEMP].value.floatValue = (float_t)rtcTemp / 100;
			mdiVariables[MDI_U_PWR_MEAS].value.floatValue = roundf(u_pwr * 100) / 100;
			mdiVariables[MDI_I_PWR_MEAS].value.floatValue = roundf(i_pwr * 100) / 100;

			(void_t) gos_mutexUnlock(&mdiMutex);
	    }

		(void_t) gos_taskSleep(250);
	}
}
