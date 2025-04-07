/*
 * driver.c
 *
 *  Created on: Jun 28, 2023
 *      Author: Gabor
 */

#include "driver.h"
#include "cfg_def.h"
#include "rcc_driver.h"
#include "gos_lib.h"

GOS_STATIC void_t sysTickHook (void_t);

gos_result_t driver_init (void_t)
{
	gos_result_t driverInitResult = GOS_SUCCESS;

	driverInitResult &= rcc_driver_init();
	driverInitResult &= drv_tmrInit();
	driverInitResult &= drv_systimerStart();
	driverInitResult &= drv_gpioInit();
	driverInitResult &= drv_i2cInit();
	driverInitResult &= drv_spiInit();
	driverInitResult &= drv_dmaInit();
	driverInitResult &= drv_uartInit();
	driverInitResult &= drv_itInit();
	driverInitResult &= drv_adcInit();

	gos_kernelRegisterSysTickHook(sysTickHook);

	return driverInitResult;
}

GOS_STATIC void_t sysTickHook (void_t)
{
	HAL_IncTick();
}
