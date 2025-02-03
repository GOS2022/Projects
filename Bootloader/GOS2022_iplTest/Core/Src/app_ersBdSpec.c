/*
 * app_ersBdSpec.c
 *
 *  Created on: Jan 29, 2025
 *      Author: Ahmed
 */

/*
 * Includes
 */
#include "gos_lib.h"
#include "app.h"
#include "iodef.h"
#include "ers_def.h"
#include "mdi_def.h"

/*
 * Function prototypes
 */
GOS_STATIC gos_result_t app_ersBdSpecRead  (u32_t address, u8_t* pData, u16_t size);
GOS_STATIC gos_result_t app_ersBdSpecWrite (u32_t address, u8_t* pData, u16_t size);
GOS_STATIC void_t       app_ersTask        (void_t);

/*
 * Static variables
 */
/**
 * APP ERS task descriptor.
 */
GOS_STATIC gos_taskDescriptor_t ersTaskDesc =
{
	.taskFunction	    = app_ersTask,
	.taskName 		    = "app_ersTask",
	.taskStackSize 	    = 0x400,
	.taskPriority 	    = 16,
	.taskPrivilegeLevel	= GOS_TASK_PRIVILEGE_USER
};

/**
 * 25LC256 descriptor.
 */
drv_25lc256Descriptor_t mem01a1_eeprom1 =
{
	.spiInstance        = DRV_SPI_INSTANCE_2,
	.csPin              = IO_SPI2_CS2,
	.readMutexTmo       = 1000u,
	.readTriggerTmo     = 1000u,
	.writeMutexTmo      = 1000u,
	.writeTriggerTmo    = 1000u,
};

/**
 * ERS configuration structure.
 */
GOS_STATIC svl_ersCfg_t ersCfg =
{
	.readFunction       = app_ersBdSpecRead,
	.writeFunction      = app_ersBdSpecWrite
};

/*
 * Extern variables
 */
GOS_EXTERN svl_mdiVariable_t mdiVariables [];

/*
 * Function: app_ersBdSpecInit
 */
gos_result_t app_ersBdSpecInit (void_t)
{
	/*
	 * Local variables.
	 */
	gos_result_t ersBdSpecInitRes = GOS_SUCCESS;

	/*
	 * Function code.
	 */
	ersBdSpecInitRes &= drv_25lc256Init((void_t*)&mem01a1_eeprom1);
	ersBdSpecInitRes &= svl_ersConfigure(&ersCfg);
	ersBdSpecInitRes &= gos_taskRegister(&ersTaskDesc, NULL);

	if (ersBdSpecInitRes != GOS_SUCCESS)
	{
		ersBdSpecInitRes = GOS_ERROR;
	}

	return ersBdSpecInitRes;
}

// TODO
GOS_STATIC void_t app_ersTask (void_t)
{
	/*
	 * Local variables.
	 */
	//u32_t numOfEntries = 0u;
	bool_t errorState = GOS_FALSE;
	u8_t  eventData [SVL_ERS_DATA_SIZE];

	/*
	 * Function code.
	 */
	(void_t) gos_taskSleep(1000);
	/*(void_t) svl_ersGetNumOfEntries(&numOfEntries);

	if (numOfEntries < 12u)
	{
		eventData[0] = numOfEntries;
		eventData[1] = 125;
		eventData[2] = 125;
		eventData[3] = 12 - numOfEntries;

		(void_t) svl_ersCreate(numOfEntries % 3, 12 + numOfEntries, eventData);
	}
	else
	{
		// Stop event creation.
	}*/

	// Create startup event.
	(void_t) svl_ersCreate(ERS_STARTUP_EVENT, 0, NULL);

	for (;;)
	{
		if (mdiVariables[MDI_CPU_TEMP].value.floatValue > 35.0 && errorState == GOS_FALSE)
		{
			(void_t) memcpy(eventData, mdiVariables[MDI_CPU_TEMP].value.arrayValue, 4);
			(void_t) svl_ersCreate(ERS_OVERHEAT_EVENT, 0, eventData);
			errorState = GOS_TRUE;
		}

		if (errorState == GOS_TRUE && mdiVariables[MDI_CPU_TEMP].value.floatValue < 30.0)
		{
			errorState = GOS_FALSE;
		}

		(void_t) gos_taskSleep(100);
	}
}

// TODO
GOS_STATIC gos_result_t app_ersBdSpecRead  (u32_t address, u8_t* pData, u16_t size)
{
	/*
	 * Function code.
	 */
	return drv_25lc256Read((void_t*)&mem01a1_eeprom1, address, pData, size);
}

// TODO
GOS_STATIC gos_result_t app_ersBdSpecWrite (u32_t address, u8_t* pData, u16_t size)
{
	/*
	 * Function code.
	 */
	return drv_25lc256Write((void_t*)&mem01a1_eeprom1, address, pData, size);
}
