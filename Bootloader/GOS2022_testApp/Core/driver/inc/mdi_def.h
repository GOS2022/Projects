/*
 * mdi_def.h
 *
 *  Created on: Jan 27, 2025
 *      Author: Ahmed
 */

#ifndef DRIVER_INC_MDI_DEF_H_
#define DRIVER_INC_MDI_DEF_H_

typedef enum
{
	MDI_VDDA,
	MDI_VREF,
	MDI_CPU_TEMP,
	MDI_RTC_TEMP,
	MDI_U_PWR_MEAS,
	MDI_I_PWR_MEAS,
	MDI_NUM_OF_VARIABLES
}mdi_variables_t;

#endif /* DRIVER_INC_MDI_DEF_H_ */
