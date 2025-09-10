/*
 * app_sdhBdSpec.c
 *
 *  Created on: Dec 24, 2024
 *      Author: Gabor
 */
#include "gos_lib.h"
#include "app.h"
#include "iodef.h"

GOS_STATIC gos_result_t app_sdhBdSpecRead (u32_t address, u8_t* pData, u32_t size);
GOS_STATIC gos_result_t app_sdhBdSpecWrite (u32_t address, u8_t* pData, u32_t size);

GOS_STATIC drv_w25q64Descriptor_t mem01a1flash =
{
	.spiInstance     = DRV_SPI_INSTANCE_2,
	.csPin           = IO_SPI2_CS0,
	.readMutexTmo    = 1000u,
	.readTriggerTmo  = 2000u,
	.writeMutexTmo   = 1000u,
	.writeTriggerTmo = 2000u,
};

GOS_STATIC svl_sdhCfg_t sdhCfg =
{
	.readFunction  = app_sdhBdSpecRead,
	.writeFunction = app_sdhBdSpecWrite
};

gos_result_t app_sdhBdSpecInit (void_t)
{
	gos_result_t sdhBdSpecInitRes = GOS_SUCCESS;

	sdhBdSpecInitRes &= drv_w25q64Init((void_t*)&mem01a1flash);
	sdhBdSpecInitRes &= svl_sdhConfigure(&sdhCfg);

	if (sdhBdSpecInitRes != GOS_SUCCESS)
		sdhBdSpecInitRes = GOS_ERROR;

	return sdhBdSpecInitRes;
}

GOS_STATIC gos_result_t app_sdhBdSpecRead (u32_t address, u8_t* pData, u32_t size)
{
	drv_w25q64ReadData((void_t*)&mem01a1flash, address, pData, size);

	if ((mem01a1flash.errorFlags & DRV_ERROR_W25Q64_READ) == 0u)
	{
		return GOS_SUCCESS;
	}
	else
	{
		return GOS_ERROR;
	}
}

GOS_STATIC gos_result_t app_sdhBdSpecWrite (u32_t address, u8_t* pData, u32_t size)
{
	drv_w25q64WriteData((void_t*)&mem01a1flash, address, pData, size);

	if ((mem01a1flash.errorFlags & DRV_ERROR_W25Q64_WRITE) == 0u)
	{
		return GOS_SUCCESS;
	}
	else
	{
		return GOS_ERROR;
	}
}
