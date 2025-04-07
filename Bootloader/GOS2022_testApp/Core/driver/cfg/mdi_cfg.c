/*
 * mdi_cfg.c
 *
 *  Created on: Jan 27, 2025
 *      Author: Ahmed
 */
#include <gos_lib.h>
#include <mdi_def.h>

/**
 * TODO
 */
svl_mdiVariable_t mdiVariables [] =
{
	[MDI_VDDA] =
	{
		.name = "V_dda [V]",
		.type = MDI_VAL_TYPE_FLOAT,
	},
	[MDI_VREF] =
	{
		.name = "V_ref [V]",
		.type = MDI_VAL_TYPE_FLOAT,
	},
	[MDI_CPU_TEMP] =
	{
		.name = "T_cpu [C]",
		.type = MDI_VAL_TYPE_FLOAT,
	},
	[MDI_RTC_TEMP] =
	{
		.name = "T_rtc [C]",
		.type = MDI_VAL_TYPE_FLOAT,
	}
};

/**
 * TODO
 */
u32_t mdiVariablesSize = sizeof(mdiVariables);
