/*
 * app_pdhBdSpec.c
 *
 *  Created on: Dec 17, 2024
 *      Author: Ahmed Ibrahim Gazar
 */
#include "gos_lib.h"
#include "app.h"
#include "iodef.h"

drv_25lc640Descriptor_t mem01a1_eeprom2 =
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

GOS_STATIC svl_pdhSwVerInfo_t appSwVerInfo =
{
	.author      = "Ahmed Ibrahim Gazar",
	.name        = "GOS_new_test_app",
	.description = "GOS new test application.",
	.major       = 4,
	.minor       = 1,
	.build       = 0,
};

GOS_STATIC svl_pdhSwInfo_t testSwInfo;

GOS_STATIC void_t       app_pdhTestTask    (void_t);
GOS_STATIC void_t       app_pdhBdSpecCheckSoftwareInfo (void_t);
GOS_STATIC gos_result_t app_pdhBdSpecRead  (u32_t address, u8_t* pData, u16_t size);
GOS_STATIC gos_result_t app_pdhBdSpecWrite (u32_t address, u8_t* pData, u16_t size);

GOS_STATIC svl_pdhCfg_t pdhCfg =
{
	.readFunction  = app_pdhBdSpecRead,
	.writeFunction = app_pdhBdSpecWrite
};

GOS_STATIC gos_taskDescriptor_t pdhTestTask =
{
	.taskFunction	= app_pdhTestTask,
	.taskName 		= "app_pdhTestTask",
	.taskStackSize 	= 0x400,
	.taskPriority 	= 10,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGED_USER
};

gos_result_t app_pdhBdSpecInit (void_t)
{
	gos_result_t pdhBdSpecInitRes = GOS_SUCCESS;

	pdhBdSpecInitRes &= drv_25lc640Init((void_t*)&mem01a1_eeprom2);
	pdhBdSpecInitRes &= svl_pdhConfigure(&pdhCfg);
	pdhBdSpecInitRes &= gos_taskRegister(&pdhTestTask, NULL);

	if (pdhBdSpecInitRes != GOS_SUCCESS)
		pdhBdSpecInitRes = GOS_ERROR;

	return pdhBdSpecInitRes;
}

GOS_STATIC void_t app_pdhTestTask (void_t)
{
	u32_t sysTicks = gos_kernelGetSysTicks();

	app_pdhBdSpecCheckSoftwareInfo();

	for (;;)
	{
		(void_t) gos_taskSleep(50);

		if (gos_kernelGetSysTicks() - sysTicks >= 1000)
		{
			(void_t) gos_traceTrace(GOS_FALSE, "\r");
			(void_t) gos_traceTrace(GOS_TRUE, "");
			sysTicks = gos_kernelGetSysTicks();
		}
	}
}

GOS_STATIC void_t app_pdhBdSpecCheckSoftwareInfo (void_t)
{
	u32_t  swInfoCrc     = 0u;
	u32_t  testSwInfoCrc = 0u;
	u32_t  libVerCrc     = 0u;
	u32_t  testLibVerCrc = 0u;
	u32_t  bldCrc        = 0u;
	bool_t forceReset    = GOS_FALSE;
	svl_pdhSwVerInfo_t libVerInfo = {0};

	(void_t) svl_pdhGetLibVersion(&libVerInfo);
	(void_t) svl_pdhGetSwInfo(&testSwInfo);

	(void_t) drv_crcGetCrc32((u8_t*)testSwInfo.bldBinaryInfo.startAddress, testSwInfo.bldBinaryInfo.size, &bldCrc);

	if (bldCrc != testSwInfo.bldBinaryInfo.crc)
	{
		(void_t) memset((void_t*)&testSwInfo.bldBinaryInfo, 0, sizeof(testSwInfo.bldBinaryInfo));
		(void_t) memset((void_t*)&testSwInfo.bldSwVerInfo, 0, sizeof(testSwInfo.bldSwVerInfo));
		(void_t) memset((void_t*)&testSwInfo.bldLibVerInfo, 0, sizeof(testSwInfo.bldLibVerInfo));
		(void_t) memset((void_t*)&testSwInfo.bldOsInfo, 0, sizeof(testSwInfo.bldOsInfo));
		forceReset = GOS_TRUE;
	}
	else
	{
		// Bootloader data OK.
	}

	gos_libGetBuildDate(&appSwVerInfo.date.years, &appSwVerInfo.date.months, &appSwVerInfo.date.days);

	(void_t) drv_crcGetCrc32((u8_t*)&appSwVerInfo, sizeof(appSwVerInfo), &swInfoCrc);
	(void_t) drv_crcGetCrc32((u8_t*)&testSwInfo.appSwVerInfo, sizeof(testSwInfo.appSwVerInfo), &testSwInfoCrc);

	(void_t) drv_crcGetCrc32((u8_t*)&libVerInfo, sizeof(libVerInfo), &libVerCrc);
	(void_t) drv_crcGetCrc32((u8_t*)&testSwInfo.appLibVerInfo, sizeof(testSwInfo.appLibVerInfo), &testLibVerCrc);

	// Check if application information has been modified.
	if (forceReset == GOS_TRUE || (swInfoCrc != testSwInfoCrc) || (libVerCrc != testLibVerCrc) ||
		(testSwInfo.appOsInfo.major != GOS_VERSION_MAJOR) || (testSwInfo.appOsInfo.minor != GOS_VERSION_MINOR))
	{
		(void_t) svl_pdhGetLibVersion(&testSwInfo.appLibVerInfo);
		(void_t) memcpy((void_t*)&testSwInfo.appOsInfo, (void_t*)&appOsInfo, sizeof(svl_pdhOsInfo_t));
		(void_t) memcpy((void_t*)&testSwInfo.appSwVerInfo, (void_t*)&appSwVerInfo, sizeof(svl_pdhSwVerInfo_t));
		(void_t) svl_pdhSetSwInfo(&testSwInfo);

		(void_t) gos_traceTrace(GOS_TRUE, "Software info updated. Restarting device...\r\n");
		(void_t) gos_taskSleep(1000);
		gos_kernelReset();
	}
	else
	{
		// Application information is up to date.
	}
}

GOS_STATIC gos_result_t app_pdhBdSpecRead  (u32_t address, u8_t* pData, u16_t size)
{
	return drv_25lc640Read((void_t*)&mem01a1_eeprom2, address, pData, size);
}

GOS_STATIC gos_result_t app_pdhBdSpecWrite (u32_t address, u8_t* pData, u16_t size)
{
	return drv_25lc640Write((void_t*)&mem01a1_eeprom2, address, pData, size);
}
