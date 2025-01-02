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
//! @file       bld.c
//! @author     Ahmed Gazar
//! @date       2024-12-26
//! @version    1.0
//!
//! @brief      GOS2022 Library / Bootloader source.
//! @details    For a more detailed description of this driver, please refer to @ref bld.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-12-26    Ahmed Gazar     Initial version created.
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
#include <bld.h>
#include <bld_cfg.h>
#include <string.h>
#include <stdio.h>
#include <drv_crc.h>
#include <drv_flash.h>
#include <svl_pdh.h>
#include <svl_sdh.h>
#include "stm32f4xx_hal.h"

/*
 * Macros
 */
/**
 * Size of chunks to be install at once.
 */
#define BLD_INSTALL_CHUNK_SIZE ( 512 )

/**
 * Progress buffer size.
 */
#define PROGRESS_BUFFER_SIZE   ( 80u )

/**
 * Percentage format buffer size.
 */
#define PERCENTAGE_BUFFER_SIZE ( 16u )

/**
 * Bootloader state control message ID.
 */
#define BLD_STATE_CONT_MSG_ID  ( 0xb4ed )

/*
 * Type definitions
 */
typedef struct
{
    u32_t  initSp;;
    void_t (*resetHandler)(void_t);
}bld_boot_vect_table_t;

/*
 * Static variables
 */
/**
 * Bootloader configuration.
 */
GOS_STATIC svl_pdhBldCfg_t bldConfig     = {0};

/**
 * Current software info.
 */
GOS_STATIC svl_pdhSwInfo_t currentSwInfo = {0};

/**
 * Bootloader state.
 */
GOS_STATIC bld_state_t     bldState;

// TODO
GOS_STATIC u8_t            bldInstallBuffer [BLD_INSTALL_CHUNK_SIZE];

/**
 * Buffer for progress printing.
 */
GOS_STATIC char_t          progressBuffer   [PROGRESS_BUFFER_SIZE];

/**
 * Buffer for progress percentage formatting.
 */
GOS_STATIC char_t          percentageBuffer [PERCENTAGE_BUFFER_SIZE];

/*
 * Function prototypes
 */
GOS_STATIC void_t bld_task (void_t);

GOS_STATIC gos_taskDescriptor_t bldTaskDesc =
{
    .taskFunction       = bld_task,
    .taskPriority       = 0u,
    .taskStackSize      = 0x1600,
    .taskName           = "app_bld_task",
    .taskPrivilegeLevel = GOS_TASK_PRIVILEGE_SUPERVISOR
};

/*
 * Function: bld_init
 */
gos_result_t bld_init (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t initResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    (void_t) svl_pdhGetBldCfg(&bldConfig);

    if (bldConfig.installRequested == GOS_TRUE)
    {
        bldState = BLD_STATE_INSTALL;
    }
    else if (bldConfig.waitForConnectionOnStartup == GOS_TRUE)
    {
        bldState = BLD_STATE_CONNECT_WAIT;
    }
    else if (bldConfig.updateMode == GOS_TRUE)
    {
        bldState = BLD_STATE_WAIT;
    }
    else
    {
    	bldState = BLD_STATE_APP_CHECK;
    }

    initResult = gos_taskRegister(&bldTaskDesc, NULL);

    return initResult;
}

/*
 * Function: bld_stateMachineGetState
 */
gos_result_t bld_stateMachineGetState (bld_state_t* pState)
{
    /*
     * Local variables.
     */
    gos_result_t getStateResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (pState != NULL)
    {
        *pState = bldState;
        getStateResult = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return getStateResult;
}

GOS_STATIC void_t bld_task (void_t)
{
    /*
     * Local variables.
     */
    svl_sdhBinaryDesc_t binaryDescriptor = {0};
    u32_t               installChunkNum  = 0u;
    u32_t               installChunkCntr = 0u;
    u32_t               perc             = 0u;
    u32_t               startTick        = 0u;
    bool_t              connectTmo       = GOS_FALSE;
    gos_message_t       gosMsg           = {0};
    //gos_message_t       gosRespMsg       = {0};
    //svl_sdhControlMsg_t controlMsg = {0}; TODO
    gos_messageId_t     msgIds []        = { BLD_STATE_CONT_MSG_ID, 0 };
    u32_t               byteCounter      = 0u;

    /*
     * Function code.
     */
    // Startup delay (for printing).
    (void_t) gos_taskSleep(500);

    (void_t) svl_pdhGetSwInfo(&currentSwInfo);
    (void_t) memcpy((void_t*)&binaryDescriptor.binaryInfo, (void_t*)&currentSwInfo.appBinaryInfo, sizeof(binaryDescriptor));

    for (;;)
    {
        switch (bldState)
        {
            case BLD_STATE_INSTALL:
            {
                // Install file is marked in configuration.
                // Try to copy it to application FLASH area.
                byteCounter = 0u;

                // Get data from external flash.
                (void_t) svl_sdhGetBinaryData(bldConfig.binaryIndex, &binaryDescriptor);

                // Check start address.
                if (binaryDescriptor.binaryInfo.startAddress < BLD_APP_ROM_START_ADDRESS)
                {
                    bldState = BLD_STATE_WAIT;
                    break;
                }
                else
                {
                    // Start address OK.
                }

                installChunkNum = (binaryDescriptor.binaryInfo.size / BLD_INSTALL_CHUNK_SIZE) +
                                  (binaryDescriptor.binaryInfo.size % BLD_INSTALL_CHUNK_SIZE == 0 ? 0 : 1);

                // Erase application area.
                (void_t) drv_flashErase(binaryDescriptor.binaryInfo.startAddress, binaryDescriptor.binaryInfo.size);

                // Write to internal flash by chunks.
                (void_t) drv_flashUnlock();

                for (installChunkCntr = 0u; installChunkCntr < installChunkNum; installChunkCntr++)
                {
                    // Read chunk from external flash.
                    (void_t) svl_sdhReadBytesFromMemory(
                            binaryDescriptor.binaryLocation + (installChunkCntr * BLD_INSTALL_CHUNK_SIZE),
                            bldInstallBuffer,
                            BLD_INSTALL_CHUNK_SIZE);

                    // Program chunk in internal flash.
                    if (((installChunkCntr + 1) * BLD_INSTALL_CHUNK_SIZE) < binaryDescriptor.binaryInfo.size)
                    {
                        (void_t) drv_flashWriteWithoutLock(
                                 (binaryDescriptor.binaryInfo.startAddress + (installChunkCntr * BLD_INSTALL_CHUNK_SIZE)),
                                 (void_t*)bldInstallBuffer,
                                 BLD_INSTALL_CHUNK_SIZE);

                        byteCounter += BLD_INSTALL_CHUNK_SIZE;
                    }
                    else
                    {
                        (void_t) drv_flashWriteWithoutLock(
                                 (binaryDescriptor.binaryInfo.startAddress + (installChunkCntr * BLD_INSTALL_CHUNK_SIZE)),
                                 (void_t*)bldInstallBuffer,
                                 binaryDescriptor.binaryInfo.size - (installChunkCntr * BLD_INSTALL_CHUNK_SIZE));

                        byteCounter += binaryDescriptor.binaryInfo.size - (installChunkCntr * BLD_INSTALL_CHUNK_SIZE);
                    }

                    // Display progress.
                    perc = 100 * 100 * byteCounter / binaryDescriptor.binaryInfo.size;

                    (void_t) sprintf(progressBuffer, "\rProgress: [ "TRACE_FG_YELLOW_START);

                    for (int i = 1; i <= 25; i++)
                    {
                        if (i * 2 <= (perc / 100))
                        {
                            (void_t) strcat(progressBuffer, "=");
                        }
                        else
                        {
                            (void_t) strcat(progressBuffer, " ");
                        }
                    }

                    (void_t) sprintf(percentageBuffer, "%3u.%02u%% ", (u8_t)(perc / 100), (u8_t)(perc % 100));
                    (void_t) strcat(progressBuffer, percentageBuffer);

                    for (int i = 26; i <= 50; i++)
                    {
                        if (i * 2 <= (perc / 100))
                        {
                            (void_t) strcat(progressBuffer, "=");
                        }
                        else
                        {
                            (void_t) strcat(progressBuffer, " ");
                        }
                    }

                    (void_t) strcat(progressBuffer, TRACE_FORMAT_RESET" ]");
                    (void_t) gos_traceTrace(GOS_FALSE, progressBuffer);
                }

                (void_t) drv_flashLock();

                (void_t) gos_traceTrace(GOS_FALSE, "\r\nInstall finished.\r\n");

                // Check newly installed application integrity.
                bldState = BLD_STATE_APP_CHECK;

                break;
            }
            case BLD_STATE_CONNECT_WAIT:
            {
                // In this state, bootloader is waiting for a connection
                // request.
                (void_t) gos_traceTrace(GOS_TRUE, "Waiting for bootloader connection");

                connectTmo = GOS_TRUE;
                startTick  = gos_kernelGetSysTicks();

                while ((gos_kernelGetSysTicks() - startTick) <= bldConfig.connectionTimeout)
                {
                    (void_t) gos_traceTrace(GOS_FALSE, ".");

                    if (gos_messageRx(msgIds, &gosMsg, 250u) == GOS_SUCCESS)
                    {
                        // TODO: if connected
                        connectTmo = GOS_FALSE;
                        break;
                    }
                    else
                    {
                        // RX timeout.
                    }
                }

                (void_t) gos_traceTrace(GOS_FALSE, "\r\n");

                if (connectTmo == GOS_TRUE)
                {
                    (void_t) gos_traceTrace(GOS_TRUE, "Connection timed out.\r\n");
                    bldState = BLD_STATE_APP_CHECK;
                }
                else
                {
                    (void_t) gos_traceTrace(GOS_TRUE, "Connection successful.\r\n");
                    bldState = BLD_STATE_WAIT;
                }

                break;
            }
            case BLD_STATE_WAIT:
            {
                // In this state, bootloader is waiting for a software install
                // request or other requests.
                (void_t) gos_traceTrace(GOS_TRUE, "Waiting for requests");

                connectTmo = GOS_TRUE;
                startTick = gos_kernelGetSysTicks();

                while ((gos_kernelGetSysTicks() - startTick) <= bldConfig.requestTimeout)
                {
                    (void_t) gos_traceTrace(GOS_FALSE, ".");

                    if (gos_messageRx(msgIds, &gosMsg, 1000u) == GOS_SUCCESS)
                    {
                        // TODO: if connected
                        //connectTmo = GOS_FALSE;

                        // Update start tick to restart timeout.
                        startTick = gos_kernelGetSysTicks();
                        break;
                    }
                    else
                    {
                        // RX timeout.
                        // Check install request.
                        (void_t) svl_pdhGetBldCfg(&bldConfig);

                        if (bldConfig.installRequested == GOS_TRUE)
                        {
                            connectTmo = GOS_FALSE;
                            bldState   = BLD_STATE_INSTALL;
                            break;
                        }
                        else
                        {
                        	// Nothing to do.
                        }
                    }
                }

                (void_t) gos_traceTrace(GOS_FALSE, "\r\n");

                if (connectTmo == GOS_TRUE)
                {
                    (void_t) gos_traceTrace(GOS_TRUE, "Request timed out.\r\n");
                    bldState = BLD_STATE_APP_CHECK;
                }
                else
                {
                    // Request served.
                }

                break;
            }
            case BLD_STATE_APP_CHECK:
            {
                if (bld_checkApplication(&binaryDescriptor.binaryInfo, GOS_TRUE) == GOS_SUCCESS)
                {
                    // Application OK.
                    bldConfig.installRequested = GOS_FALSE;
                    bldConfig.binaryIndex      = 0u;
                    bldConfig.updateMode       = GOS_FALSE;
                    bldConfig.startupCounter   = 1u;

                    (void_t) svl_pdhSetBldCfg(&bldConfig);

                    (void_t) memcpy((void_t*)&currentSwInfo.appBinaryInfo, (void_t*)&binaryDescriptor.binaryInfo, sizeof(currentSwInfo.appBinaryInfo));

                    (void_t) svl_pdhSetSwInfo(&currentSwInfo);

                    (void_t) gos_traceTrace(GOS_TRUE, "Exiting update mode...\r\n");
                    (void_t) gos_taskSleep(300);
                    gos_kernelReset();
                }
                else
                {
                    // Application CRC error. Wait for software install request.
                    bldConfig.installRequested = GOS_FALSE;
                    bldConfig.binaryIndex      = 0u;
                    bldConfig.updateMode       = GOS_TRUE;

                    (void_t) svl_pdhSetBldCfg(&bldConfig);

                    bldState = BLD_STATE_WAIT;

                    (void_t) gos_traceTrace(GOS_TRUE, "Entering update mode...\r\n");
                }
                break;
            }
        }
        (void_t) gos_taskSleep(20);
    }
}

/*
 * Function: bld_checkApplication
 */
gos_result_t bld_checkApplication (svl_pdhBinaryInfo_t* pAppData, bool_t bootMode)
{
    /*
     * Local variables.
     */
    gos_result_t appCheckResult = GOS_ERROR;

    /*
     * Function code.
     */
    // Check application size.
    if (pAppData->size > 0u && pAppData->size <= BLD_APP_ROM_SIZE)
    {
        if (drv_crcCheckCrc32((u8_t*)pAppData->startAddress, pAppData->size, pAppData->crc, NULL) == DRV_CRC_CHECK_OK)
        {
            appCheckResult = GOS_SUCCESS;
        }
        else
        {
            // Application check fail.
            if (bootMode == GOS_TRUE)
            {
                (void_t) gos_traceTrace(GOS_TRUE, "Application CRC error.\r\n");
            }
            else
            {
                (void_t) gos_traceTraceFormattedUnsafe("Application CRC error.\r\n");
            }
        }
    }
    else
    {
        if (bootMode == GOS_TRUE)
        {
            (void_t) gos_traceTrace(GOS_TRUE, "Application size error.\r\n");
        }
        else
        {
            (void_t) gos_traceTraceFormattedUnsafe("Application size error.\r\n");
        }
    }

    return appCheckResult;
}

/*
 * Function: bld_jumpToApplication
 */
gos_result_t bld_jumpToApplication (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t jumpResult = GOS_ERROR;
    u16_t        irqIdx     = 0u;

    /*
     * Function code.
     */
    if (currentSwInfo.appBinaryInfo.startAddress % 0x200 == 0u)
    {
        /* Disable all interrupts */
        __disable_irq();

        /* Disable Systick timer */
        SysTick->CTRL = 0;

        /* Set the clock to the default state */
        HAL_RCC_DeInit();
        HAL_DeInit();

        for (irqIdx = 0; irqIdx < sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]); irqIdx++)
        {
            NVIC->ICER[irqIdx] = 0xFFFFFFFF;
            NVIC->ICPR[irqIdx] = 0xFFFFFFFF;
        }

        SYSCFG->MEMRMP = 0x01;

        SCB->VTOR = currentSwInfo.appBinaryInfo.startAddress;

        // Set the MSP
        __set_MSP(((bld_boot_vect_table_t*)currentSwInfo.appBinaryInfo.startAddress)->initSp);

        // Jump to app firmware
        ((bld_boot_vect_table_t*)currentSwInfo.appBinaryInfo.startAddress)->resetHandler();
    }
    else
    {
        // Address is incorrect.
    }

    return jumpResult;
}

/*
 * Function: bld_getBootloaderSize
 */
gos_result_t bld_getBootloaderSize (u32_t* pSize)
{
    /*
     * Local variables.
     */
    gos_result_t sizeGetResult = GOS_SUCCESS;
    u32_t        address       = BLD_ROM_END_ADDRESS;

    /*
     * Function code.
     */
    if (pSize != NULL)
    {
        *pSize = (BLD_ROM_END_ADDRESS - BLD_ROM_START_ADDRESS);

        for (address = BLD_ROM_END_ADDRESS; address >= BLD_ROM_START_ADDRESS; address--)
        {
            if ((u8_t)(*((u32_t*)address)) != 0xFFu)
            {
                break;
            }
            else
            {
                (*pSize)--;
            }
        }
    }
    else
    {
        // NULL pointer error.
        sizeGetResult = GOS_ERROR;
    }

    return sizeGetResult;
}

/*
 * Function: bld_getBootloaderCrc
 */
gos_result_t bld_getBootloaderCrc (u32_t* pCrc)
{
    /*
     * Local variables.
     */
    gos_result_t crcGetResult = GOS_ERROR;
    u32_t        bldSize      = 0u;

    /*
     * Function code.
     */
    if (pCrc != NULL)
    {
        crcGetResult = bld_getBootloaderSize(&bldSize);
        crcGetResult &= drv_crcGetCrc32((u8_t*)BLD_ROM_START_ADDRESS, bldSize, pCrc);
    }
    else
    {
        // NULL pointer error.
    }

    if (crcGetResult != GOS_SUCCESS)
    {
        crcGetResult = GOS_ERROR;
    }
    else
    {
        // OK.
    }

    return crcGetResult;
}

/*
 * Function: bld_initData
 */
gos_result_t bld_initData (svl_pdhSwVerInfo_t* pBldSwVer)
{
    /*
     * Local variables.
     */
    gos_result_t       initDataResult     = GOS_SUCCESS;
    u32_t              currentBldSwVerCrc = 0u;
    u32_t              desiredBldSwVerCrc = 0u;
	u32_t              libVerCrc          = 0u;
	u32_t              testLibVerCrc      = 0u;
	svl_pdhSwVerInfo_t libVerInfo         = {0};

    /*
     * Function code.
     */
    if (pBldSwVer != NULL)
    {
        // Get current software info.
        (void_t) svl_pdhGetSwInfo(&currentSwInfo);
    	(void_t) svl_pdhGetLibVersion(&libVerInfo);

        // Calculate CRC of current and desired bootloader software info.
        initDataResult &= drv_crcGetCrc32((u8_t*)&currentSwInfo.bldSwVerInfo, sizeof(currentSwInfo.bldSwVerInfo), &currentBldSwVerCrc);
        initDataResult &= drv_crcGetCrc32((u8_t*)pBldSwVer, sizeof(*pBldSwVer), &desiredBldSwVerCrc);

        initDataResult &= drv_crcGetCrc32((u8_t*)&libVerInfo, sizeof(libVerInfo), &libVerCrc);
        initDataResult &= drv_crcGetCrc32((u8_t*)&(currentSwInfo.bldLibVerInfo), sizeof(currentSwInfo.bldLibVerInfo), &testLibVerCrc);

        // Check if reset is needed.
    	if ((currentBldSwVerCrc != desiredBldSwVerCrc) || (libVerCrc != testLibVerCrc) ||
    		(currentSwInfo.bldOsInfo.major != GOS_VERSION_MAJOR) || (currentSwInfo.bldOsInfo.minor != GOS_VERSION_MINOR))
        {
            // Copy desired data.
            (void_t) memcpy((void_t*)&currentSwInfo.bldSwVerInfo, (void_t*)pBldSwVer, sizeof(svl_pdhSwVerInfo_t));

            // Fill out OS info.
            currentSwInfo.bldOsInfo.major = GOS_VERSION_MAJOR;
            currentSwInfo.bldOsInfo.minor = GOS_VERSION_MINOR;

            // Fill out binary data.
            currentSwInfo.bldBinaryInfo.startAddress = BLD_ROM_START_ADDRESS;
            initDataResult &= bld_getBootloaderSize(&currentSwInfo.bldBinaryInfo.size);
            initDataResult &= bld_getBootloaderCrc(&currentSwInfo.bldBinaryInfo.crc);

            // Fill out library info.
            initDataResult &= svl_pdhGetLibVersion(&currentSwInfo.bldLibVerInfo);

            (void_t) svl_pdhSetSwInfo(&currentSwInfo);

            if (initDataResult == GOS_SUCCESS)
            {
                (void_t) gos_traceTraceFormattedUnsafe("Bootloader data updated. Restarting device...\r\n");
                (void_t) gos_taskSleep(1000);
                gos_kernelReset();
            }
            else
            {
                (void_t) gos_traceTraceFormattedUnsafe("Bootloader data initialization failed.\r\n");
                initDataResult = GOS_ERROR;
            }
        }
        else
        {
            // Version info is up-to-date.
        }
    }
    else
    {
        initDataResult = GOS_ERROR;
    }

    return initDataResult;
}

/*
 * Function: bld_initConfig
 */
gos_result_t bld_initConfig (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t initCfgResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    (void_t) svl_pdhGetBldCfg(&bldConfig);

    // If boolean values are incorrect, it indicates an uninitialized
    // bootloader configuration.
    if ((bldConfig.updateMode       != GOS_TRUE && bldConfig.updateMode       != GOS_FALSE &&
        bldConfig.wirelessUpdate   != GOS_TRUE && bldConfig.wirelessUpdate   != GOS_FALSE &&
        bldConfig.installRequested != GOS_TRUE && bldConfig.installRequested != GOS_FALSE) ||
    	bldConfig.connectionTimeout == 0 || bldConfig.requestTimeout == 0 || bldConfig.installTimeout == 0)
    {
        bldConfig.connectionTimeout          = BLD_DEFAULT_CONN_TMO_MS;
        bldConfig.waitForConnectionOnStartup = BLD_DEFAULT_CONN_ON_STARTUP;
        bldConfig.requestTimeout             = BLD_DEFAULT_REQ_TMO_MS;
        bldConfig.installTimeout             = BLD_DEFAULT_INSTALL_TMO_MS;
        bldConfig.startupCounter             = 0u;
        bldConfig.updateMode                 = GOS_FALSE;
        bldConfig.wirelessUpdate             = GOS_FALSE;
        bldConfig.installRequested           = GOS_FALSE;

        (void_t) svl_pdhSetBldCfg(&bldConfig);
    }
    else
    {
        // Configuration is initialized.
    }

    return initCfgResult;
}

/*
 * Function: bld_printConfig
 */
gos_result_t bld_printConfig (void_t)
{
    /*
     * Function code.
     */
    (void_t) svl_pdhGetBldCfg(&bldConfig);

    (void_t) gos_traceTraceFormattedUnsafe(TRACE_BG_BLUE_START "BOOTLOADER CONFIGURATION" TRACE_FORMAT_RESET "\r\n");
    (void_t) gos_traceTraceFormattedUnsafe("Connection on startup:\t%s\r\n", bldConfig.waitForConnectionOnStartup == GOS_TRUE ? "yes" : "no");
    (void_t) gos_traceTraceFormattedUnsafe("Connection timeout:   \t%u ms\r\n", bldConfig.connectionTimeout);
    (void_t) gos_traceTraceFormattedUnsafe("Request timeout:      \t%u ms\r\n", bldConfig.requestTimeout);
    (void_t) gos_traceTraceFormattedUnsafe("Install timeout:      \t%u ms\r\n\r\n", bldConfig.installTimeout);

    return GOS_SUCCESS;
}
