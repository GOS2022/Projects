/*
 * app_pdhBdSpec.c
 *
 *  Created on: Dec 17, 2024
 *      Author: Gabor
 */
#include "gos_lib.h"
#include "app.h"
#include "iodef.h"
#include "bld.h"

drv_25lc640Descriptor_t mem01a1_eeprom2 =
{
	.spiInstance     = DRV_SPI_INSTANCE_2,
	.csPin           = IO_SPI2_CS1,
	.readMutexTmo    = 1000u,
	.readTriggerTmo  = 1000u,
	.writeMutexTmo   = 1000u,
	.writeTriggerTmo = 1000u,
};

GOS_STATIC gos_result_t app_pdhBdSpecRead  (u32_t address, u8_t* pData, u16_t size);
GOS_STATIC gos_result_t app_pdhBdSpecWrite (u32_t address, u8_t* pData, u16_t size);

GOS_STATIC svl_pdhCfg_t pdhCfg =
{
	.readFunction  = app_pdhBdSpecRead,
	.writeFunction = app_pdhBdSpecWrite
};

gos_result_t app_pdhBdSpecInit (void_t)
{
	gos_result_t pdhBdSpecInitRes = GOS_SUCCESS;

	pdhBdSpecInitRes &= drv_25lc640Init((void_t*)&mem01a1_eeprom2);
	pdhBdSpecInitRes &= svl_pdhConfigure(&pdhCfg);

	if (pdhBdSpecInitRes != GOS_SUCCESS)
		pdhBdSpecInitRes = GOS_ERROR;

	return pdhBdSpecInitRes;
}

GOS_STATIC gos_result_t app_pdhBdSpecRead  (u32_t address, u8_t* pData, u16_t size)
{
	return drv_25lc640Read((void_t*)&mem01a1_eeprom2, address, pData, size);
}

GOS_STATIC gos_result_t app_pdhBdSpecWrite (u32_t address, u8_t* pData, u16_t size)
{
	return drv_25lc640Write((void_t*)&mem01a1_eeprom2, address, pData, size);
}
