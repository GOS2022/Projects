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

GOS_STATIC svl_pdhSwVerInfo_t bldVerInfo =
{
	.author      = "Ahmed Ibrahim Gazar",
	.name        = "GOS2022_bld_stm32f4",
	.description = "GOS2022 bootloader for STM32F4 devices.",
	.major       = 1,
	.minor       = 0,
	.build       = 2,
	.date        =
	{
		.years   = 2024,
		.months  = GOS_TIME_DECEMBER,
		.days    = 26
	}
};

//GOS_STATIC svl_pdhSwInfo_t testSwInfo;

GOS_STATIC void_t       app_pdhTask    (void_t);
GOS_STATIC gos_result_t app_pdhBdSpecRead  (u32_t address, u8_t* pData, u16_t size);
GOS_STATIC gos_result_t app_pdhBdSpecWrite (u32_t address, u8_t* pData, u16_t size);

GOS_STATIC svl_pdhCfg_t pdhCfg =
{
	.readFunction  = app_pdhBdSpecRead,
	.writeFunction = app_pdhBdSpecWrite
};

GOS_STATIC gos_taskDescriptor_t pdhTestTask =
{
	.taskFunction	= app_pdhTask,
	.taskName 		= "app_pdh_task",
	.taskStackSize 	= 0x300,
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

GOS_STATIC void_t app_pdhTask (void_t)
{
	//(void_t) bld_initData(&bldVerInfo);

	for (;;)
	{
		(void_t) gos_taskDelete(pdhTestTask.taskId);
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
