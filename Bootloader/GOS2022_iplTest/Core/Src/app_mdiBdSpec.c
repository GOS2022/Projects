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
#define ADC_SAMPLES          ( 100 )

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
	.taskStackSize 	    = 0x400,
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
	 * Function code.
	 */
	return gos_taskRegister(&mdiTaskDesc, NULL);
}

// TODO
GOS_STATIC void_t app_mdiTask (void_t)
{
	/*
	 * Local variables.
	 */
	u16_t   adcResult [2];
	float_t vdda;
	float_t vref, vref_avg;
	float_t temp, temp_avg;
	u32_t   sum1, sum2;

	/*
	 * Function code.
	 */
	(void_t) gos_taskSleep(1500);

	for (;;)
	{
		sum1 = 0;
		sum2 = 0;

	    for (u16_t i = 0; i < ADC_SAMPLES; ++i)
	    {
	    	(void_t) drv_adcGetValueDMA(DRV_ADC_INSTANCE_1, (u32_t*)adcResult, 2, 1000, 10);
	        sum1 += adcResult[0];
	        sum2 += adcResult[1];
	    }

	    vref_avg = sum2 / ADC_SAMPLES;
	    temp_avg = sum1 / ADC_SAMPLES;

		vdda = (float_t) VREFINT_CAL_VREF * (float_t) *VREFINT_CAL_ADDR / vref_avg / 1000;
		vref = (float_t) vdda / ADC_RESOLUTION * vref_avg;

	    temp = (float_t) ((float_t) ((float_t) (TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP)
	                           / (float_t) (*TEMPSENSOR_CAL2_ADDR - *TEMPSENSOR_CAL1_ADDR))
	                           * (temp_avg - *TEMPSENSOR_CAL1_ADDR) + TEMPSENSOR_CAL1_TEMP);

	    mdiVariables[MDI_VDDA].value.floatValue     = roundf(vdda * 100) / 100;
		mdiVariables[MDI_VREF].value.floatValue     = roundf(vref * 100) / 100;
		mdiVariables[MDI_CPU_TEMP].value.floatValue = roundf(temp * 10) / 10;

		(void_t) gos_taskSleep(250);
	}
}
