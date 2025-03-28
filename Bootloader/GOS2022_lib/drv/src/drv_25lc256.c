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
//! @file       drv_25lc256.c
//! @author     Ahmed Gazar
//! @date       2024-12-19
//! @version    1.0
//!
//! @brief      GOS2022 Library / 25LC256 driver source.
//! @details    For a more detailed description of this driver, please refer to @ref drv_25lc256.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-12-19    Ahmed Gazar     Initial version created.
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
#include <drv_25lc256.h>
#include <drv_error.h>
#include <drv_gpio.h>
#include <math.h>
#include <string.h>

/*
 * Macros
 */
/**
 * Page size in bytes.
 */
#define PAGE_SIZE       ( 64u )

/**
 * Number of pages.
 */
#define W25Q64_PAGE_NUM ( 512u )

/**
 * Total memory size.
 */
#define TOTAL_SIZE      ( PAGE_SIZE * W25Q64_PAGE_NUM )

/*
 * Static variables
 */
/**
 * Empty buffer for write / clear operation (one page).
 */
GOS_STATIC u8_t txBuffer [PAGE_SIZE + 3];

/*
 * Function: drv_25lc256Init
 */
gos_result_t drv_25lc256Init (void_t* pDevice)
{
    /*
     * Local variables.
     */
    gos_result_t initResult = GOS_SUCCESS;
    u32_t        errorFlags = 0u;

    /*
     * Function code.
     */
    DRV_ERROR_CHK_NULL_PTR(pDevice, errorFlags, DRV_ERROR_25LC256_DEVICE_NULL);
    DRV_ERROR_CHK_SET(gos_mutexInit(&((drv_25lc256Descriptor_t*)pDevice)->deviceMutex), ((drv_25lc256Descriptor_t*)pDevice)->errorFlags, DRV_ERROR_25LC256_MUTEX_INIT);
    DRV_ERROR_SET_RESULT(initResult, errorFlags);

    return initResult;
}

/*
 * Function: drv_25lc256GetErrorFlags
 */
gos_result_t drv_25lc256GetErrorFlags (void_t* pDevice, u32_t* pErrorFlags)
{
    /*
     * Local variables.
     */
    gos_result_t _25lc256GetErrorFlagsResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (pDevice != NULL && pErrorFlags != NULL)
    {
        *pErrorFlags = ((drv_25lc256Descriptor_t*)pDevice)->errorFlags;
        _25lc256GetErrorFlagsResult = GOS_SUCCESS;
    }
    else
    {
        // NULL pointer.
    }

    return _25lc256GetErrorFlagsResult;
}

/*
 * Function: drv_25lc256ClearErrorFlags
 */
gos_result_t drv_25lc256ClearErrorFlags (void_t* pDevice, u32_t errorFlags)
{
    /*
     * Local variables.
     */
    gos_result_t _25lc256ClearErrorFlagsResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (pDevice != NULL)
    {
        DRV_ERROR_CLEAR(((drv_25lc256Descriptor_t*)pDevice)->errorFlags, errorFlags);
        _25lc256ClearErrorFlagsResult = GOS_SUCCESS;
    }
    else
    {
        // Device is NULL.
    }

    return _25lc256ClearErrorFlagsResult;
}

/*
 * Function: drv_25lc256Read
 */
GOS_INLINE gos_result_t drv_25lc256Read (void_t* pDevice, u16_t address, u8_t* pTarget, u16_t size)
{
    /*
     * Local variables.
     */
    gos_result_t readResult    = GOS_SUCCESS;
    u16_t        startPage     = 0u;
    u16_t        endPage       = 0u;
    u16_t        numofpages    = 0u;
    u16_t        idx           = 0u;
    u16_t        pos           = 0u;
    u16_t        offset        = 0u;
    u16_t        memAddress    = 0u;
    u16_t        bytesRem      = 0u;
    int32_t      paddrposition = 0;
    u32_t        errorFlags    = 0u;

    /*
     * Function code.
     */
    DRV_ERROR_CHK_NULL_PTR(pDevice, errorFlags, DRV_ERROR_25LC256_DEVICE_NULL);
    DRV_ERROR_CHK_NULL_PTR(pTarget, errorFlags, DRV_ERROR_25LC256_TARGET_NULL);

    if ((errorFlags == 0u) && (address + size) < TOTAL_SIZE)
    {
        DRV_ERROR_CHK_SET(
                gos_mutexLock(&((drv_25lc256Descriptor_t*)pDevice)->deviceMutex, ((drv_25lc256Descriptor_t*)pDevice)->readMutexTmo),
                errorFlags,
                DRV_ERROR_25LC256_MUTEX_LOCK
                );
        DRV_ERROR_SET_RESULT(readResult, errorFlags);

        paddrposition = log(PAGE_SIZE)/log(2);
        startPage     = address / PAGE_SIZE;
        endPage       = ((size+address)/PAGE_SIZE);
        numofpages    = (endPage-startPage) + 1;
        pos           = 0u;
        offset        = address % PAGE_SIZE;

        for (idx = 0u; idx < numofpages && readResult == GOS_SUCCESS; idx++)
        {
            memAddress = startPage << paddrposition | offset;
            bytesRem   = ((size + offset) < PAGE_SIZE) ? size : (PAGE_SIZE - offset);
            txBuffer[0] = 0x03;
            txBuffer[1] = (u8_t)(memAddress >> 8);
            txBuffer[2] = (u8_t)(memAddress & 0xFF);

            (void_t) drv_gpioWritePin(((drv_25lc256Descriptor_t*)pDevice)->csPin, GPIO_STATE_LOW);

            DRV_ERROR_CHK_SET(
                    drv_spiTransmitIT(
                    		((drv_25lc256Descriptor_t*)pDevice)->spiInstance,
        					txBuffer,
        					3,
        					((drv_25lc256Descriptor_t*)pDevice)->readMutexTmo,
        					((drv_25lc256Descriptor_t*)pDevice)->readTriggerTmo
        					),
                    errorFlags,
                    DRV_ERROR_25LC256_READ
                    );
            DRV_ERROR_SET_RESULT(readResult, errorFlags);

            DRV_ERROR_CHK_SET(
                    drv_spiReceiveIT(
                    		((drv_25lc256Descriptor_t*)pDevice)->spiInstance,
        					&pTarget[pos],
        					bytesRem,
        					((drv_25lc256Descriptor_t*)pDevice)->readMutexTmo,
        					((drv_25lc256Descriptor_t*)pDevice)->readTriggerTmo
        					),
                    errorFlags,
                    DRV_ERROR_25LC256_READ
                    );
            DRV_ERROR_SET_RESULT(readResult, errorFlags);

            (void_t) drv_gpioWritePin(((drv_25lc256Descriptor_t*)pDevice)->csPin, GPIO_STATE_HIGH);

            (void_t) gos_taskSleep(10);

            startPage += 1;
            offset    =  0u;
            size      =  size - bytesRem;
            pos       += bytesRem;
        }

        (void_t) gos_mutexUnlock(&((drv_25lc256Descriptor_t*)pDevice)->deviceMutex);
    }
    else
    {
        // Initial check failed.
    }

    DRV_ERROR_SET(((drv_25lc256Descriptor_t*)pDevice)->errorFlags, errorFlags);
    DRV_ERROR_SET_RESULT(readResult, errorFlags);

    return readResult;
}

/*
 * Function: drv_25lc256Write
 */
GOS_INLINE gos_result_t drv_25lc256Write (void_t* pDevice, u16_t address, u8_t* pData, u16_t size)
{
    /*
     * Local variables.
     */
    gos_result_t writeResult     = GOS_SUCCESS;
    u16_t        startPage       = 0u;
    u16_t        endPage         = 0u;
    u16_t        numofpages      = 0u;
    u16_t        idx             = 0u;
    u16_t        pos             = 0u;
    u16_t        offset          = 0u;
    u16_t        memAddress      = 0u;
    u16_t        bytesRem        = 0u;
    int32_t      paddrposition   = 0;
    u32_t        errorFlags      = 0u;
    u8_t         wrenInstruction = 0x06;

    /*
     * Function code.
     */
    DRV_ERROR_CHK_NULL_PTR(pDevice, errorFlags, DRV_ERROR_25LC256_DEVICE_NULL);
    DRV_ERROR_CHK_NULL_PTR(pData, errorFlags, DRV_ERROR_25LC256_DATA_NULL);

    if ((errorFlags == 0u) && ((address + size) < TOTAL_SIZE))
    {
        DRV_ERROR_CHK_SET(
                gos_mutexLock(&((drv_25lc256Descriptor_t*)pDevice)->deviceMutex, ((drv_25lc256Descriptor_t*)pDevice)->writeMutexTmo),
                errorFlags,
                DRV_ERROR_25LC256_MUTEX_LOCK
                );
        DRV_ERROR_SET_RESULT(writeResult, errorFlags);

        paddrposition = log(PAGE_SIZE)/log(2);
        startPage     = address / PAGE_SIZE;
        endPage       = ((size+address)/PAGE_SIZE);
        numofpages    = (endPage-startPage) + 1;
        pos           = 0u;
        offset        = address % PAGE_SIZE;

        for (idx = 0u; idx < numofpages && writeResult == GOS_SUCCESS; idx++)
        {
            memAddress = startPage << paddrposition | offset;
            bytesRem   = ((size + offset) < PAGE_SIZE) ? size : (PAGE_SIZE - offset);

            txBuffer[0] = 0x02;
            txBuffer[1] = (memAddress >> 8);
            txBuffer[2] = memAddress & 0xFF;

            (void_t) memcpy(txBuffer + 3, &pData[pos], bytesRem);
            (void_t) drv_gpioWritePin(((drv_25lc256Descriptor_t*)pDevice)->csPin, GPIO_STATE_LOW);

            DRV_ERROR_CHK_SET(
                    drv_spiTransmitIT(
                    		((drv_25lc256Descriptor_t*)pDevice)->spiInstance,
        					&wrenInstruction,
        					1,
        					((drv_25lc256Descriptor_t*)pDevice)->readMutexTmo,
        					((drv_25lc256Descriptor_t*)pDevice)->readTriggerTmo
        					),
                    errorFlags,
                    DRV_ERROR_25LC256_WRITE
                    );
            DRV_ERROR_SET_RESULT(writeResult, errorFlags);

            (void_t) drv_gpioWritePin(((drv_25lc256Descriptor_t*)pDevice)->csPin, GPIO_STATE_HIGH);
            (void_t) drv_gpioWritePin(((drv_25lc256Descriptor_t*)pDevice)->csPin, GPIO_STATE_LOW);

            DRV_ERROR_CHK_SET(
                    drv_spiTransmitIT(
                    		((drv_25lc256Descriptor_t*)pDevice)->spiInstance,
        					txBuffer,//&pData[pos],
        					bytesRem + 3,
        					((drv_25lc256Descriptor_t*)pDevice)->readMutexTmo,
        					((drv_25lc256Descriptor_t*)pDevice)->readTriggerTmo
        					),
                    errorFlags,
                    DRV_ERROR_25LC256_WRITE
                    );
            DRV_ERROR_SET_RESULT(writeResult, errorFlags);

            (void_t) drv_gpioWritePin(((drv_25lc256Descriptor_t*)pDevice)->csPin, GPIO_STATE_HIGH);

            startPage += 1;
            offset    =  0u;
            size      =  size - bytesRem;
            pos       += bytesRem;

            (void_t) gos_taskSleep(5);
        }

        (void_t) gos_mutexUnlock(&((drv_25lc256Descriptor_t*)pDevice)->deviceMutex);
    }
    else
    {
        // Initial check failed.
    }

    DRV_ERROR_SET(((drv_25lc256Descriptor_t*)pDevice)->errorFlags, errorFlags);
    DRV_ERROR_SET_RESULT(writeResult, errorFlags);

    return writeResult;
}

/*
 * Function: drv_25lc256Erase
 */
gos_result_t drv_25lc256Erase (void_t* pDevice)
{
	/*
	 * Local variables.
	 */
	gos_result_t eraseResult     = GOS_SUCCESS;
	u32_t        errorFlags      = 0u;
	u8_t         wrenInstruction = 0x06;
	u16_t        idx             = 0u;

	/*
	 * Function code.
	 */
	(void_t) memset((void_t*)txBuffer, 0, (PAGE_SIZE + 3));

    DRV_ERROR_CHK_SET(
            gos_mutexLock(&((drv_25lc256Descriptor_t*)pDevice)->deviceMutex, ((drv_25lc256Descriptor_t*)pDevice)->writeMutexTmo),
            errorFlags,
            DRV_ERROR_25LC256_MUTEX_LOCK
            );
    DRV_ERROR_SET_RESULT(eraseResult, errorFlags);

    for (idx = 0u; idx < W25Q64_PAGE_NUM && eraseResult == GOS_SUCCESS; idx++)
    {
    	txBuffer[0] = 0x02;
    	txBuffer[1] = ((idx * PAGE_SIZE) >> 8);
    	txBuffer[2] = (idx * PAGE_SIZE) & 0xFF;

        (void_t) drv_gpioWritePin(((drv_25lc256Descriptor_t*)pDevice)->csPin, GPIO_STATE_LOW);

        DRV_ERROR_CHK_SET(
                drv_spiTransmitIT(
                		((drv_25lc256Descriptor_t*)pDevice)->spiInstance,
    					&wrenInstruction,
    					1,
    					((drv_25lc256Descriptor_t*)pDevice)->readMutexTmo,
    					((drv_25lc256Descriptor_t*)pDevice)->readTriggerTmo
    					),
                errorFlags,
                DRV_ERROR_25LC256_WRITE
                );
        DRV_ERROR_SET_RESULT(eraseResult, errorFlags);

        (void_t) drv_gpioWritePin(((drv_25lc256Descriptor_t*)pDevice)->csPin, GPIO_STATE_HIGH);
        (void_t) drv_gpioWritePin(((drv_25lc256Descriptor_t*)pDevice)->csPin, GPIO_STATE_LOW);

        DRV_ERROR_CHK_SET(
                drv_spiTransmitIT(
                		((drv_25lc256Descriptor_t*)pDevice)->spiInstance,
						txBuffer,
						PAGE_SIZE + 3,
    					((drv_25lc256Descriptor_t*)pDevice)->readMutexTmo,
    					((drv_25lc256Descriptor_t*)pDevice)->readTriggerTmo
    					),
                errorFlags,
                DRV_ERROR_25LC256_WRITE
                );
        DRV_ERROR_SET_RESULT(eraseResult, errorFlags);

        (void_t) drv_gpioWritePin(((drv_25lc256Descriptor_t*)pDevice)->csPin, GPIO_STATE_HIGH);

        (void_t) gos_taskSleep(5);
    }

    DRV_ERROR_SET(((drv_25lc256Descriptor_t*)pDevice)->errorFlags, errorFlags);
    DRV_ERROR_SET_RESULT(eraseResult, errorFlags);

    return eraseResult;
}
