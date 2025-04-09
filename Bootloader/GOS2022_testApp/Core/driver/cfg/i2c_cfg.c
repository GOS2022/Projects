/*
 * i2c_cfg.c
 *
 *  Created on: Mar 15, 2024
 *      Author: Gabor
 */
#include "drv_i2c.h"

/**
 * TODO
 */
GOS_CONST drv_i2cDescriptor_t i2cConfig [] =
{
	{
		.periphInstance  = DRV_I2C_INSTANCE_1,
		.clockSpeed      = 400000,
		.dutyCycle       = I2C_DUTYCYCLE_2,
		.ownAddress1     = 0,
		.addressingMode  = I2C_ADDRESSINGMODE_7BIT,
		.dualAddressMode = I2C_DUALADDRESS_DISABLE,
		.ownAddress2     = 0,
		.generalCallMode = I2C_GENERALCALL_DISABLE,
		.noStretchMode   = I2C_NOSTRETCH_DISABLE
	}
};

/**
 * TODO
 */
u32_t i2cConfigSize = sizeof(i2cConfig);
