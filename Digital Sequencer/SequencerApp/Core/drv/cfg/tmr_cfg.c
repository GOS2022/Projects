/*
 * tmr_cfg.c
 *
 *  Created on: May 9, 2024
 *      Author: Gabor
 */
#include "drv_tmr.h"
#include "stm32f4xx_hal.h"
#include "cfg_def.h"

GOS_EXTERN void_t app_controlStepTriggerCallback (void_t);

/**
 * TODO
 */
drv_tmrDescriptor_t tmrConfig [] =
{
	// TIM10: Step timer.
	[CFG_TMR_STEP] =
	{
		.periphInstance      = DRV_TMR_INSTANCE_10,
		.prescaler           = 64-1,
		.counterMode         = TIM_COUNTERMODE_UP,
		.period              = 1000-1,
		.clockDivision       = TIM_CLOCKDIVISION_DIV1,
		.autoReloadPreload   = TIM_AUTORELOAD_PRELOAD_DISABLE,
		.periodCallback      = app_controlStepTriggerCallback,
		.useMasterConfig     = GOS_FALSE,
		.useClockConfig      = GOS_FALSE
	},
	// TIM11: System timer.
	[CFG_TMR_SYSTIMER] =
	{
		.periphInstance      = DRV_TMR_INSTANCE_11,
		.prescaler           = 64-1,
		.counterMode         = TIM_COUNTERMODE_UP,
		.period              = 1-1,
		.clockDivision       = TIM_CLOCKDIVISION_DIV1,
		.autoReloadPreload   = TIM_AUTORELOAD_PRELOAD_DISABLE,
		.useMasterConfig     = GOS_FALSE,
		.useClockConfig      = GOS_FALSE
	},
};

/**
 * TODO
 */
u32_t tmrConfigSize = sizeof(tmrConfig);

/**
 * TODO
 */
GOS_CONST drv_tmrPeriphInstance_t  tmrServiceConfig [] =
{
	[DRV_TMR_SYSTIMER_INSTANCE] = DRV_TMR_INSTANCE_11
};
