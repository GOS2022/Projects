/*
 * dac_cfg.c
 *
 *  Created on: May 9, 2024
 *      Author: Gabor
 */

#include "drv_dac.h"
#include "drv_dma.h"
#include "drv_tmr.h"
#include "stm32f4xx_hal.h"
#include "cfg_def.h"


/**
 * TODO
 */
GOS_CONST drv_dacDescriptor_t dacConfig [] = {};

/**
 * TODO
 */
u32_t dacConfigSize = sizeof(dacConfig);
