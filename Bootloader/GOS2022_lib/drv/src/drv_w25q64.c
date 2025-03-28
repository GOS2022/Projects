//*************************************************************************************************
//
//                            #####             #####             #####
//                          #########         #########         #########
//                         ##                ##       ##       ##
//                        ##                ##         ##        #####
//                        ##     #####      ##         ##           #####
//                         ##       ##       ##       ##                ##
//                          #########         #########         #########
//                            #####             #####             #####
//
//                                      (c) Ahmed Gazar, 2024
//
//*************************************************************************************************
//! @file       drv_w25q64.c
//! @author     Ahmed Gazar
//! @date       2024-12-24
//! @version    1.0
//!
//! @brief      GOS2022 Library / W25Q64 driver source.
//! @details    For a more detailed description of this driver, please refer to @ref drv_w25q64.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-12-24    Ahmed Gazar     Initial version created.
//*************************************************************************************************
//
// Copyright (c) 2024 Ahmed Gazar
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//*************************************************************************************************
/*
 * Includes
 */
#include <drv_w25q64.h>
#include <drv_error.h>
#include <drv_gpio.h>
#include <math.h>
#include <string.h>

/*
 * Macros
 */

/**
 * Size of a sector.
 */
#define SECTOR_SIZE ( 4096u )

/*
 * Static variables
 */
/**
 * Sector buffer for data update.
 */
GOS_STATIC u8_t sectorBuffer [SECTOR_SIZE];

/**
 * Page write buffer.
 */
GOS_STATIC u8_t pageWriteBuffer [266];

/*
 * Function prototypes
 */
GOS_STATIC void_t drv_w25q64ReadDataFast   (void_t* pDevice, u32_t startPage, u8_t offset, u8_t* pData, u32_t size);
GOS_STATIC void_t drv_w25q64WriteDataClean (void_t* pDevice, u32_t startPage, u8_t offset, u8_t* pData, u32_t size);
GOS_STATIC u32_t  drv_w25q64BytesToWrite   (u32_t size, u16_t offset);
GOS_STATIC u32_t  drv_w25q64BytesToModify  (u32_t size, u16_t offset);
GOS_STATIC void_t drv_w25q64ChipSelectLow  (void_t* pDevice);
GOS_STATIC void_t drv_w25q64ChipSelectHigh (void_t* pDevice);
GOS_STATIC void_t drv_w25q64WriteEnable    (void_t* pDevice);
GOS_STATIC void_t drv_w25q64WriteDisable   (void_t* pDevice);
GOS_STATIC void_t drv_w25q64Write          (void_t* pDevice, u8_t* pData, u16_t size);
GOS_STATIC void_t drv_w25q64Read           (void_t* pDevice, u8_t* pData, u16_t size);
GOS_STATIC void_t drv_w25q64EraseSector    (void_t* pDevice, u16_t sector);

/*
 * Function: drv_w25q64Init
 */
gos_result_t drv_w25q64Init (void_t* pDevice)
{
    /*
     * Local variables.
     */
    gos_result_t initResult = GOS_SUCCESS;
    u32_t        errorFlags = 0u;

    /*
     * Function code.
     */
    DRV_ERROR_CHK_NULL_PTR(pDevice, errorFlags, DRV_ERROR_W25Q64_DEVICE_NULL);
    DRV_ERROR_CHK_SET(gos_mutexInit(&((drv_w25q64Descriptor_t*)pDevice)->deviceMutex), ((drv_w25q64Descriptor_t*)pDevice)->errorFlags, DRV_ERROR_W25Q64_MUTEX_INIT);
    DRV_ERROR_SET_RESULT(initResult, errorFlags);

    return initResult;
}

/*
 * Function: drv_w25q64GetErrorFlags
 */
gos_result_t drv_w25q64GetErrorFlags (void_t* pDevice, u32_t* pErrorFlags)
{
    /*
     * Local variables.
     */
    gos_result_t w25q64GetErrorFlagsResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (pDevice != NULL && pErrorFlags != NULL)
    {
        *pErrorFlags = ((drv_w25q64Descriptor_t*)pDevice)->errorFlags;
        w25q64GetErrorFlagsResult = GOS_SUCCESS;
    }
    else
    {
        // NULL pointer.
    }

    return w25q64GetErrorFlagsResult;
}

/*
 * Function: drv_w25q64ClearErrorFlags
 */
gos_result_t drv_w25q64ClearErrorFlags (void_t* pDevice, u32_t errorFlags)
{
    /*
     * Local variables.
     */
    gos_result_t w25q64ClearErrorFlagsResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (pDevice != NULL)
    {
        DRV_ERROR_CLEAR(((drv_w25q64Descriptor_t*)pDevice)->errorFlags, errorFlags);
        w25q64ClearErrorFlagsResult = GOS_SUCCESS;
    }
    else
    {
        // Device is NULL.
    }

    return w25q64ClearErrorFlagsResult;
}

/*
 * Function: drv_w25q64Reset
 */
GOS_INLINE void_t drv_w25q64Reset (void_t* pDevice)
{
	/*
	 * Local variables.
	 */
	u8_t command [] = { 0x66, 0x99 };

    /*
     * Function code.
     */
	if (pDevice != NULL)
	{
        DRV_ERROR_CHK_SET(
                gos_mutexLock(&((drv_w25q64Descriptor_t*)pDevice)->deviceMutex, ((drv_w25q64Descriptor_t*)pDevice)->readMutexTmo),
				((drv_w25q64Descriptor_t*)pDevice)->errorFlags,
                DRV_ERROR_W25Q64_MUTEX_LOCK
                );

		drv_w25q64ChipSelectLow(pDevice);
		drv_w25q64Write(pDevice, command, 2);
		drv_w25q64ChipSelectHigh(pDevice);

		(void_t) gos_taskSleep(100);

		(void_t) gos_mutexUnlock(&((drv_w25q64Descriptor_t*)pDevice)->deviceMutex);
	}
	else
	{
		// Device is NULL pointer.
	}
}

/*
 * Function: drv_w25q64ReadId
 */
GOS_INLINE void_t drv_w25q64ReadId (void_t* pDevice, u32_t* pId)
{
	/*
	 * Local variables.
	 */
	u8_t command = 0x9f;
	u8_t id [3];

    /*
     * Function code.
     */
	if ((pDevice != NULL) && (pId != NULL))
	{
        DRV_ERROR_CHK_SET(
                gos_mutexLock(&((drv_w25q64Descriptor_t*)pDevice)->deviceMutex, ((drv_w25q64Descriptor_t*)pDevice)->readMutexTmo),
				((drv_w25q64Descriptor_t*)pDevice)->errorFlags,
                DRV_ERROR_W25Q64_MUTEX_LOCK
                );

		drv_w25q64ChipSelectLow(pDevice);
		drv_w25q64Write(pDevice, &command, 1);
		drv_w25q64Read(pDevice, id, 3);
		drv_w25q64ChipSelectHigh(pDevice);

		*pId = ((id[0] << 16) | (id[1] << 8) | id[2]);

		(void_t) gos_mutexUnlock(&((drv_w25q64Descriptor_t*)pDevice)->deviceMutex);
	}
	else
	{
		// Device or target variable is NULL.
	}
}

/*
 * Function: drv_w25q64ReadData
 */
GOS_INLINE void_t drv_w25q64ReadData (void_t* pDevice, u32_t address, u8_t* pData, u32_t size)
{
	/*
	 * Local variables.
	 */
    u32_t startPage  = address / W25Q64_PAGE_SIZE;
    u8_t  offset     = address % W25Q64_PAGE_SIZE;
	u32_t memAddress = startPage * W25Q64_PAGE_SIZE + offset;
	u8_t  command [] = { 0x03, (u8_t)((memAddress >> 16) & 0xFF),
						(u8_t)((memAddress >> 8) & 0xFF), (u8_t)(memAddress & 0xFF)};

    /*
     * Function code.
     */
	if ((pDevice != NULL) && (pData != NULL))
	{
        DRV_ERROR_CHK_SET(
                gos_mutexLock(&((drv_w25q64Descriptor_t*)pDevice)->deviceMutex, ((drv_w25q64Descriptor_t*)pDevice)->readMutexTmo),
				((drv_w25q64Descriptor_t*)pDevice)->errorFlags,
                DRV_ERROR_W25Q64_MUTEX_LOCK
                );

		drv_w25q64ChipSelectLow(pDevice);
		drv_w25q64Write(pDevice, command, 4);
		drv_w25q64Read(pDevice, pData, size);
		drv_w25q64ChipSelectHigh(pDevice);

		(void_t) gos_mutexUnlock(&((drv_w25q64Descriptor_t*)pDevice)->deviceMutex);
	}
	else
	{
		// Device or data is NULL pointer.
	}
}

/*
 * Function: drv_w25q64WriteData
 */
GOS_INLINE void_t drv_w25q64WriteData (void_t* pDevice, u32_t address, u8_t* pData, u32_t size)
{
	/*
	 * Local variables.
	 */
    u32_t startPage    = address / W25Q64_PAGE_SIZE;
    u8_t  offset       = address % W25Q64_PAGE_SIZE;
	u16_t startSector  = startPage / 16;
	u16_t endSector    = (startPage + ((size + offset - 1) / W25Q64_PAGE_SIZE)) / 16;
	u16_t numOfSectors = endSector - startSector + 1;
	u32_t sectorOffset = ((startPage % 16) * W25Q64_PAGE_SIZE) + offset;
	u32_t dataIndx     = 0u;

    /*
     * Function code.
     */
	if ((pDevice != NULL) && (pData != NULL))
	{
        DRV_ERROR_CHK_SET(
                gos_mutexLock(&((drv_w25q64Descriptor_t*)pDevice)->deviceMutex, ((drv_w25q64Descriptor_t*)pDevice)->readMutexTmo),
				((drv_w25q64Descriptor_t*)pDevice)->errorFlags,
                DRV_ERROR_W25Q64_MUTEX_LOCK
                );

		for (u16_t i = 0u; i < numOfSectors; i++)
		{
			u32_t _startPage = startSector * 16;

			//(void_t) memset(sectorBuffer, 0, SECTOR_SIZE);

			drv_w25q64ReadDataFast(pDevice, _startPage, 0, sectorBuffer, SECTOR_SIZE);

			u16_t bytesRem = drv_w25q64BytesToModify(size, sectorOffset);

			for (u16_t j = 0u; j < bytesRem; j++)
			{
				sectorBuffer[j + sectorOffset] = pData[j + dataIndx];
			}

			drv_w25q64WriteDataClean(pDevice, _startPage, 0, sectorBuffer, SECTOR_SIZE);

			startSector++;
			sectorOffset = 0;
			dataIndx = dataIndx + bytesRem;
			size = size - bytesRem;
		}

		(void_t) gos_mutexUnlock(&((drv_w25q64Descriptor_t*)pDevice)->deviceMutex);
	}
	else
	{
		// Device or data is NULL pointer.
	}
}

/**
 * @brief   Reads data in fast mode.
 * @details Sends a fast read command to the chip.
 *
 * @param   pDevice
 * @param   startPage : Page to start reading from.
 * @param   offset    : Byte offset within the start page.
 * @param   pData     : Target buffer.
 * @param   size      : Size of data to be read.
 *
 * @return  -
 */
GOS_STATIC void_t drv_w25q64ReadDataFast (void_t* pDevice, u32_t startPage, u8_t offset, u8_t* pData, u32_t size)
{
	/*
	 * Local variables.
	 */
	u32_t memAddress = startPage * W25Q64_PAGE_SIZE + offset;
	u8_t  command [] = { 0x0b, (u8_t)((memAddress >> 16) & 0xFF),
						(u8_t)((memAddress >> 8) & 0xFF), (u8_t)(memAddress & 0xFF), 0x00 };

    /*
     * Function code.
     */
	drv_w25q64ChipSelectLow(pDevice);
	drv_w25q64Write(pDevice, command, 5);
	drv_w25q64Read(pDevice, pData, size);
	drv_w25q64ChipSelectHigh(pDevice);
}

/**
 * TODO
 * @param pDevice
 * @param startPage
 * @param offset
 * @param pData
 * @param size
 * @return
 */
GOS_STATIC void_t drv_w25q64WriteDataClean (void_t* pDevice, u32_t startPage, u8_t offset, u8_t* pData, u32_t size)
{
	/*
	 * Local variables.
	 */
	u32_t endPage      = startPage + ((size + offset - 1) / W25Q64_PAGE_SIZE);
	u32_t numOfPages   = endPage - startPage + 1;
	u16_t startSector  = startPage / 16;
	u16_t endSector    = endPage / 16;
	u16_t numOfSectors = endSector - startSector + 1;
	u32_t dataPosition = 0u;
	u32_t idx;

    /*
     * Function code.
     */
	for (idx = 0u; idx < numOfSectors; idx++)
	{
		drv_w25q64EraseSector(pDevice, (startSector + idx));
	}

	(void_t) memset(pageWriteBuffer, 0, 266);

	for (idx = 0; idx < numOfPages; idx++)
	{
		u32_t memAddress = (startPage * W25Q64_PAGE_SIZE) + offset;
		u32_t bytesRem = drv_w25q64BytesToWrite(size, offset);
		u32_t indx = 4;

		drv_w25q64WriteEnable(pDevice);

		pageWriteBuffer[0] = 0x02;
		pageWriteBuffer[1] = (u8_t)((memAddress >> 16) & 0xFF);
		pageWriteBuffer[2] = (u8_t)((memAddress >> 8) & 0xFF);
		pageWriteBuffer[3] = (u8_t)(memAddress & 0xFF);

		u16_t bytesToSend = bytesRem + indx;

		for (u16_t i = 0; i < bytesRem; i++)
		{
			pageWriteBuffer[indx++] = pData[ i + dataPosition];
		}

		drv_w25q64ChipSelectLow(pDevice);

		drv_w25q64Write(pDevice, pageWriteBuffer, bytesToSend);

		drv_w25q64ChipSelectHigh(pDevice);

		startPage++;
		offset = 0;
		size = size - bytesRem;
		dataPosition = dataPosition + bytesRem;

		(void_t) gos_taskSleep(5);

		drv_w25q64WriteDisable(pDevice);
	}
}

/**
 * @brief   Calculates the number of bytes to write.
 * @details Calculates the number of bytes to write.
 *
 * @param   size   : Data size.
 * @param   offset : Offset.
 *
 * @return  Number of bytes to write.
 */
GOS_STATIC u32_t drv_w25q64BytesToWrite (u32_t size, u16_t offset)
{
	/*
	 * Local variables.
	 */
	u32_t bytesToWrite = 0u;

    /*
     * Function code.
     */
	if ((size + offset) < W25Q64_PAGE_SIZE)
	{
		bytesToWrite = size;
	}
	else
	{
		bytesToWrite = W25Q64_PAGE_SIZE - offset;
	}

	return bytesToWrite;
}

/**
 * @brief   Calculates the number of bytes to modify.
 * @details Calculates the number of bytes to modify.
 *
 * @param   size   : Data size.
 * @param   offset : Offset.
 *
 * @return  Number of bytes to modify.
 */
GOS_STATIC u32_t drv_w25q64BytesToModify (u32_t size, u16_t offset)
{
	/*
	 * Local variables.
	 */
	u32_t bytesToWrite = 0u;

    /*
     * Function code.
     */
	if ((size + offset) < SECTOR_SIZE)
	{
		bytesToWrite = size;
	}
	else
	{
		bytesToWrite = SECTOR_SIZE - offset;
	}

	return bytesToWrite;
}

/**
 * @brief   Sets the chip select pin to low.
 * @details Sets the chip select pin to low.
 *
 * @param   pDevice : Pointer to the device descriptor.
 *
 * @return  -
 */
GOS_STATIC void_t drv_w25q64ChipSelectLow (void_t* pDevice)
{
    /*
     * Function code.
     */
	(void_t) drv_gpioWritePin(((drv_w25q64Descriptor_t*)pDevice)->csPin, GPIO_STATE_LOW);
}

/**
 * @brief   Sets the chip select pin to high.
 * @details Sets the chip select pin to high.
 *
 * @param   pDevice : Pointer to the device descriptor.
 *
 * @return  -
 */
GOS_STATIC void_t drv_w25q64ChipSelectHigh (void_t* pDevice)
{
    /*
     * Function code.
     */
	(void_t) drv_gpioWritePin(((drv_w25q64Descriptor_t*)pDevice)->csPin, GPIO_STATE_HIGH);
}

/**
 * @brief   Sends a write enable command to the chip.
 * @details Sends a write enable command to the chip.
 *
 * @param   pDevice : Pointer to the device descriptor.
 *
 * @return  -
 */
GOS_STATIC void_t drv_w25q64WriteEnable (void_t* pDevice)
{
	/*
	 * Local variables.
	 */
	u8_t command = 0x06;

    /*
     * Function code.
     */
	drv_w25q64ChipSelectLow(pDevice);
	drv_w25q64Write(pDevice, &command, 1);
	drv_w25q64ChipSelectHigh(pDevice);
	(void_t) gos_taskSleep(5);
}

/**
 * @brief   Sends a write disable command to the chip.
 * @details Sends a write disable command to the chip.
 *
 * @param   pDevice : Pointer to the device descriptor.
 *
 * @return  -
 */
GOS_STATIC void_t drv_w25q64WriteDisable (void_t* pDevice)
{
	/*
	 * Local variables.
	 */
	u8_t command = 0x04;

    /*
     * Function code.
     */
	drv_w25q64ChipSelectLow(pDevice);
	drv_w25q64Write(pDevice, &command, 1);
	drv_w25q64ChipSelectHigh(pDevice);
	(void_t) gos_taskSleep(5);
}

/**
 * @brief
 * @details TODO
 *
 * @param pDevice
 * @param pData
 * @param size
 * @return
 */
GOS_STATIC void_t drv_w25q64Write (void_t* pDevice, u8_t* pData, u16_t size)
{
    /*
     * Function code.
     */
    DRV_ERROR_CHK_SET(
            drv_spiTransmitIT(
            		((drv_w25q64Descriptor_t*)pDevice)->spiInstance,
					pData,
					size,
					((drv_w25q64Descriptor_t*)pDevice)->writeMutexTmo,
					((drv_w25q64Descriptor_t*)pDevice)->writeTriggerTmo
					),
			((drv_w25q64Descriptor_t*)pDevice)->errorFlags,
			DRV_ERROR_W25Q64_WRITE
            );
}

/**
 * @brief
 * @details TODO
 *
 * @param pDevice
 * @param pData
 * @param size
 * @return
 */
GOS_STATIC void_t drv_w25q64Read (void_t* pDevice, u8_t* pData, u16_t size)
{
    /*
     * Function code.
     */
    DRV_ERROR_CHK_SET(
            drv_spiReceiveIT(
            		((drv_w25q64Descriptor_t*)pDevice)->spiInstance,
					pData,
					size,
					((drv_w25q64Descriptor_t*)pDevice)->readMutexTmo,
					((drv_w25q64Descriptor_t*)pDevice)->readTriggerTmo
					),
			((drv_w25q64Descriptor_t*)pDevice)->errorFlags,
			DRV_ERROR_W25Q64_READ
            );
}

/**
 * TODO
 * @param pDevice
 * @param sector
 * @return
 */
GOS_STATIC void_t drv_w25q64EraseSector (void_t* pDevice, u16_t sector)
{
	/*
	 * Local variables.
	 */
	u32_t memAddress = sector * 16 * W25Q64_PAGE_SIZE;
	u8_t  command [] = { 0x20, (u8_t)((memAddress >> 16) & 0xFF),
		(u8_t)((memAddress >> 8) & 0xFF), (u8_t)(memAddress & 0xFF) };

    /*
     * Function code.
     */
	drv_w25q64WriteEnable(pDevice);
	drv_w25q64ChipSelectLow(pDevice);
	drv_w25q64Write(pDevice, command, 4);
	drv_w25q64ChipSelectHigh(pDevice);

	(void_t) gos_taskSleep(450);

	drv_w25q64WriteDisable(pDevice);
}
