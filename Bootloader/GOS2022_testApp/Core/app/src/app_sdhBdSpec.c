/*
 * app_sdhBdSpec.c
 *
 *  Created on: Dec 24, 2024
 *      Author: Gabor
 */
#include "gos_lib.h"
#include "app.h"
#include "iodef.h"

SVL_DHS_READ_FUNC_DECL(app_sdhBdSpecRead);
SVL_DHS_WRITE_FUNC_DECL(app_sdhBdSpecWrite);
SVL_DHS_READ_FUNC_WRAPPER_DECL(app_sdhReadWrapper);
SVL_DHS_WRITE_FUNC_WRAPPER_DECL(app_sdhWriteWrapper);

GOS_STATIC drv_w25q64Descriptor_t flashDesc =
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

/**
 * Standard device descriptor for SDH FLASH.
 */
GOS_STATIC svl_dhsDevice_t flashDevice =
{
	.name              = "flash",
	.description       = "External W25Q64 over SPI for SW download.",
	.pInitializer      = drv_w25q64Init,
	.pDeviceDescriptor = (void_t*)&flashDesc,
	.readFunctions[0]  = app_sdhReadWrapper,
	.writeFunctions[0] = app_sdhWriteWrapper,
	.enabled           = GOS_TRUE,
	.errorTolerance    = 100
};

gos_result_t app_sdhBdSpecInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t sdhBdSpecInitRes = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(sdhBdSpecInitRes, svl_dhsRegisterDevice(&flashDevice));
	GOS_CONCAT_RESULT(sdhBdSpecInitRes, svl_sdhConfigure(&sdhCfg));

	return sdhBdSpecInitRes;
}

/**
 * TODO
 */
SVL_DHS_READ_FUNC_DEF(app_sdhBdSpecRead, flashDevice, 0, 4);

/**
 * TODO
 */
SVL_DHS_WRITE_FUNC_DEF(app_sdhBdSpecWrite, flashDevice, 0, 4);

/**
 * TODO
 */
SVL_DHS_READ_FUNC_WRAPPER_DECL(app_sdhReadWrapper)
{
	/*
	 * Local variables.
	 */
	void_t* device;
	u32_t address;
	u8_t* pData;
	u16_t size;

	/*
	 * Function code.
	 */
	GOS_UNUSED_PAR(params);
	device = (void_t*)va_arg(args, int);
	address = (u32_t)va_arg(args, int);
	pData = (u8_t*)va_arg(args, int);
	size = (u16_t)va_arg(args, int);

	(void_t) drv_w25q64ReadData(device, address, pData, size);

	if ((((drv_w25q64Descriptor_t*)device)->errorFlags & DRV_ERROR_W25Q64_READ) == 0u)
	{
		return GOS_SUCCESS;
	}
	else
	{
		return GOS_ERROR;
	}
}

/**
 * TODO
 */
SVL_DHS_WRITE_FUNC_WRAPPER_DECL(app_sdhWriteWrapper)
{
	/*
	 * Local variables.
	 */
	void_t* device;
	u32_t address;
	u8_t* pData;
	u16_t size;

	/*
	 * Function code.
	 */
	GOS_UNUSED_PAR(params);
	device = (void_t*)va_arg(args, int);
	address = (u32_t)va_arg(args, int);
	pData = (u8_t*)va_arg(args, int);
	size = (u16_t)va_arg(args, int);

	(void_t) drv_w25q64WriteData(device, address, pData, size);

	if ((((drv_w25q64Descriptor_t*)device)->errorFlags & DRV_ERROR_W25Q64_WRITE) == 0u)
	{
		return GOS_SUCCESS;
	}
	else
	{
		return GOS_ERROR;
	}
}
