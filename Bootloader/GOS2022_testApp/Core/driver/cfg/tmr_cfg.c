/*
 * tmr_cfg.c
 *
 *  Created on: May 9, 2024
 *      Author: Gabor
 */
#include <cfg_def.h>
#include "drv_tmr.h"
#include "stm32f4xx_hal.h"

/**
 * TODO
 */
/*GOS_CONST*/ drv_tmrDescriptor_t tmrConfig [] =
{
	// TIM2
	/*{
		.periphInstance      = DRV_TMR_INSTANCE_2,
		.prescaler           = 0,
		.repetitionCounter   = 0,
		.counterMode         = TIM_COUNTERMODE_UP,
	    .period              = 65536-1,
	    .clockDivision       = TIM_CLOCKDIVISION_DIV1,
	    .autoReloadPreload   = TIM_AUTORELOAD_PRELOAD_DISABLE,
		.useClockConfig      = GOS_TRUE,
	    .clockSource         = TIM_CLOCKSOURCE_INTERNAL,
		.useMasterConfig     = GOS_TRUE,
	    .masterOutputTrigger = TIM_TRGO_RESET,
	    .masterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE
	},*/
	// TIM3
	{
	    .periphInstance      = DRV_TMR_INSTANCE_3,
	    .prescaler           = 50-1,
	    .repetitionCounter   = 0,
	    .counterMode         = TIM_COUNTERMODE_UP,
	    .period              = 1000-1,
	    .clockDivision       = TIM_CLOCKDIVISION_DIV1,
	    .autoReloadPreload   = TIM_AUTORELOAD_PRELOAD_DISABLE,
		.useClockConfig      = GOS_TRUE,
	    .clockSource         = TIM_CLOCKSOURCE_INTERNAL,
		.useMasterConfig     = GOS_TRUE,
	    .masterOutputTrigger = TIM_TRGO_RESET,
	    .masterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE
	},
	// TIM12 -> system timer
	{
		.periphInstance      = DRV_TMR_INSTANCE_12,
		.prescaler           = 84-1,
		.counterMode         = TIM_COUNTERMODE_UP,
		.period              = 0xffffffff-1,
		.clockDivision       = TIM_CLOCKDIVISION_DIV1,
		.autoReloadPreload   = TIM_AUTORELOAD_PRELOAD_DISABLE,
		.useMasterConfig     = GOS_FALSE,
		.useClockConfig      = GOS_FALSE
	}
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
	[DRV_TMR_SYSTIMER_INSTANCE] = DRV_TMR_INSTANCE_12
};
