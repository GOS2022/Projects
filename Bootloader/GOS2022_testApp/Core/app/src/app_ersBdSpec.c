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
 * Macros
 */
#define CPU_TEMP_LIMIT_MAX_C  ( 40.0 )
#define CPU_TEMP_LIMIT_MIN_C  ( 36.0 )

/*
 * Function prototypes
 */
GOS_STATIC void_t       app_ersTask         (void_t);
/*GOS_STATIC gos_result_t app_ersBdSpecRead   (u32_t address, u8_t* pData, u16_t size);
GOS_STATIC gos_result_t app_ersBdSpecWrite  (u32_t address, u8_t* pData, u16_t size);
GOS_STATIC gos_result_t app_ersReadWrapper  (u8_t params, va_list args);
GOS_STATIC gos_result_t app_ersWriteWrapper (u8_t params, va_list args);*/

SVL_DHS_READ_FUNC_DECL(app_ersBdSpecRead);
SVL_DHS_WRITE_FUNC_DECL(app_ersBdSpecWrite);
SVL_DHS_READ_FUNC_WRAPPER_DECL(app_ersReadWrapper);
SVL_DHS_WRITE_FUNC_WRAPPER_DECL(app_ersWriteWrapper);


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
drv_25lc256Descriptor_t eepromDesc =
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

/**
 * Standard device descriptor for ERS EEPROM.
 */
GOS_STATIC svl_dhsDevice_t eepromDevice =
{
	.name              = "eeprom1",
	.description       = "External 25LC256 over SPI for events.",
	.pInitializer      = drv_25lc256Init,
	.pDeviceDescriptor = (void_t*)&eepromDesc,
	.readFunctions[0]  = app_ersReadWrapper,
	.writeFunctions[0] = app_ersWriteWrapper,
	.enabled           = GOS_TRUE,
	.errorTolerance    = 16
};

/*
 * Extern variables
 */
GOS_EXTERN svl_mdiVariable_t mdiVariables [];
GOS_EXTERN gos_mutex_t mdiMutex;

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
	GOS_CONCAT_RESULT(ersBdSpecInitRes, svl_dhsRegisterDevice(&eepromDevice));
	GOS_CONCAT_RESULT(ersBdSpecInitRes, svl_ersConfigure(&ersCfg));
	GOS_CONCAT_RESULT(ersBdSpecInitRes, gos_taskRegister(&ersTaskDesc, NULL));

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

	// Create startup event.
	(void_t) svl_ersCreate(ERS_STARTUP_EVENT, 0, NULL);

	for (;;)
	{
		if (gos_mutexLock(&mdiMutex, GOS_MUTEX_ENDLESS_TMO) == GOS_SUCCESS)
		{
			if ((mdiVariables[MDI_CPU_TEMP].value.floatValue > CPU_TEMP_LIMIT_MAX_C) && (errorState == GOS_FALSE))
			{
				(void_t) memcpy(eventData, mdiVariables[MDI_CPU_TEMP].value.arrayValue, 4);
				(void_t) svl_ersCreate(ERS_OVERHEAT_EVENT, 0, eventData);
				errorState = GOS_TRUE;
			}

			if ((errorState == GOS_TRUE) && (mdiVariables[MDI_CPU_TEMP].value.floatValue < CPU_TEMP_LIMIT_MIN_C))
			{
				errorState = GOS_FALSE;
			}

			(void_t) gos_mutexUnlock(&mdiMutex);
		}

		(void_t) gos_taskSleep(100);
	}
}

/**
 * TODO
 */
SVL_DHS_READ_FUNC_DEF(app_ersBdSpecRead, eepromDevice, 0, 4);

/**
 * TODO
 */
SVL_DHS_WRITE_FUNC_DEF(app_ersBdSpecWrite, eepromDevice, 0, 4);


/**
 * TODO
 */
SVL_DHS_READ_FUNC_WRAPPER_DECL(app_ersReadWrapper)
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

	return drv_25lc256Read(device, address, pData, size);
}

/**
 * TODO
 */
SVL_DHS_WRITE_FUNC_WRAPPER_DECL(app_ersWriteWrapper)
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

	return drv_25lc256Write(device, address, pData, size);
}
