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
#if 0
/*
 * app_pdhBdSpec.c
 *
 *  Created on: Dec 17, 2024
 *      Author: Ahmed Ibrahim Gazar
 */
#include "gos_lib.h"
#include "app.h"
#include "iodef.h"

GOS_STATIC drv_25lc640Descriptor_t eepromDesc =
{
	.spiInstance     = DRV_SPI_INSTANCE_2,
	.csPin           = IO_SPI2_CS1,
	.readMutexTmo    = 1000u,
	.readTriggerTmo  = 1000u,
	.writeMutexTmo   = 1000u,
	.writeTriggerTmo = 1000u,
};

GOS_STATIC svl_pdhOsInfo_t appOsInfo =
{
	.major = GOS_VERSION_MAJOR,
	.minor = GOS_VERSION_MINOR
};

SVL_DHS_READ_FUNC_DECL(app_pdhBdSpecRead);
SVL_DHS_WRITE_FUNC_DECL(app_pdhBdSpecWrite);
SVL_DHS_READ_FUNC_WRAPPER_DECL(app_pdhReadWrapper);
SVL_DHS_WRITE_FUNC_WRAPPER_DECL(app_pdhWriteWrapper);

GOS_STATIC svl_pdhCfg_t pdhCfg =
{
	.readFunction  = app_pdhBdSpecRead,
	.writeFunction = app_pdhBdSpecWrite
};

/**
 * Standard device descriptor for PDH EEPROM.
 */
GOS_STATIC svl_dhsDevice_t eepromDevice =
{
	.name              = "eeprom2",
	.description       = "External 25LC640 over SPI for project data.",
	.pInitializer      = drv_25lc640Init,
	.pDeviceDescriptor = (void_t*)&eepromDesc,
	.readFunctions[0]  = app_pdhReadWrapper,
	.writeFunctions[0] = app_pdhWriteWrapper,
	.enabled           = GOS_TRUE,
	.errorTolerance    = 16
};

/*
 * Function: app_pdhBdSpecInit
 */
gos_result_t app_pdhBdSpecInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t pdhBdSpecInitRes = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	GOS_CONCAT_RESULT(pdhBdSpecInitRes, svl_dhsRegisterDevice(&eepromDevice));
	GOS_CONCAT_RESULT(pdhBdSpecInitRes, svl_dhsForceInitialize(eepromDevice.deviceId));
	GOS_CONCAT_RESULT(pdhBdSpecInitRes, svl_pdhConfigure(&pdhCfg));

	return pdhBdSpecInitRes;
}

/**
 * TODO
 */
SVL_DHS_READ_FUNC_DEF(app_pdhBdSpecRead, eepromDevice, 0, 4);

/**
 * TODO
 */
SVL_DHS_WRITE_FUNC_DEF(app_pdhBdSpecWrite, eepromDevice, 0, 4);

/**
 * TODO
 */
SVL_DHS_READ_FUNC_WRAPPER_DECL(app_pdhReadWrapper)
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

	return drv_25lc640Read(device, address, pData, size);
}

/**
 * TODO
 */
SVL_DHS_WRITE_FUNC_WRAPPER_DECL(app_pdhWriteWrapper)
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

	return drv_25lc640Write(device, address, pData, size);
}

#endif
