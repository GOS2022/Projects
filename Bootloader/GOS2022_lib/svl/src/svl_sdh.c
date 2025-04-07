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
//! @file       svl_sdh.c
//! @author     Ahmed Gazar
//! @date       2025-03-28
//! @version    1.1
//!
//! @brief      GOS2022 Library / Software Download Handler
//! @details    For a more detailed description of this service, please refer to @ref svl_sdh.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-12-24    Ahmed Gazar     Initial version created.
// 1.1        2025-03-28    Ahmed Gazar     *    Rework of communication between callbacks and
//                                               SDH daemon
//                                          +    Missing documentation added
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
#include <svl_cfg.h>
#include <svl_ipl.h>
#include <svl_sdh.h>
#include <drv_crc.h>
#include <string.h>

/*
 * Macros
 */
/**
 * SDH storage descriptor area start address.
 */
#define SVL_SDH_STORAGE_DESC_AREA_START     ( 0u    )

/**
 * SDH storage binary descriptor start address.
 */
#define SVL_SDH_STORAGE_DESC_BIN_DESC_START ( SVL_SDH_STORAGE_DESC_AREA_START + sizeof(u16_t) )

/**
 * SDH storage descriptor area size.
 */
#define SVL_SDH_STORAGE_DESC_AREA_SIZE      ( 4096u )

/**
 * SDH storage descriptor are end.
 */
#define SVL_SDH_STORAGE_DESC_AREA_END       ( SVL_SDH_STORAGE_DESC_AREA_START + SVL_SDH_STORAGE_DESC_AREA_SIZE - 1 )

/**
 * SDH binary area start address.
 */
#define SVL_SDH_BINARY_AREA_START           ( SVL_SDH_STORAGE_DESC_AREA_END + 1 )

/**
 * SDH binary area size.
 */
#define SVL_SDH_BINARY_AREA_SIZE            ( 8388608 - SVL_SDH_BINARY_AREA_START )

/**
 * SDH binary area end.
 */
#define SVL_SDH_BINARY_AREA_END             ( SVL_SDH_BINARY_AREA_START + SVL_SDH_BINARY_AREA_SIZE - 1 )

/**
 * SDH binary chunk size.
 */
#define SVL_SDH_CHUNK_SIZE                  ( 1024u )

/**
 * SDH binary buffer size.
 */
#define SVL_SDH_BUFFER_SIZE                 ( SVL_SDH_CHUNK_SIZE + 64u )

/**
 * SDH daemon expected trigger value.
 */
#define SVL_SDH_DAEMON_TRIGGER_VALUE        ( 1u )

/**
 * SDH feedback expected trigger value.
 */
#define SVL_SDH_FEEDBACK_TRIGGER_VALUE      ( 1u )

/*
 * Type definitions
 */
/**
 * SDH states.
 */
typedef enum
{
    SDH_STATE_IDLE,               //!< Idle state.
    SDH_STATE_DOWNLOADING_BINARY, //!< Downloading binary.
    SDH_STATE_BINARY_NUM_REQ,     //!< Number of binaries request.
    SDH_STATE_BINARY_INFO_REQ,    //!< Binary info request.
    SDH_STATE_BINARY_DOWNLOAD_REQ,//!< Binary download request.
    SDH_STATE_BINARY_INSTALL_REQ, //!< Binary install request.
    SDH_STATE_BINARY_ERASE_REQ    //!< Binary erase request.
}svl_sdhState_t;

/**
 * SDH download request results.
 */
typedef enum
{
    SDH_DOWNLOAD_REQ_OK = 1,            //!< Request OK.
    SDH_DOWNLOAD_REQ_DESC_SIZE_ERR = 2, //!< Descriptor does not fit.
    SDH_DOWNLOAD_REQ_FILE_SIZE_ERR = 4  //!< File does not fit.
}svl_sdhDownloadReqRes_t;

/**
 * SDH chunk descriptor.
 */
typedef struct __attribute__((packed))
{
    u16_t chunkIdx; //!< Index of binary chunk.
    u8_t  result;   //!< Result of processing.
}svl_sdhChunkDesc_t;

/**
 * SDH sysmon message IDs.
 */
typedef enum
{
    SVL_SDH_SYSMON_MSG_BINARY_NUM_REQ        = 0x1001, //!< SDH sysmon message ID for binary number request.
    SVL_SDH_SYSMON_MSG_BINARY_NUM_RESP       = 0x1A01, //!< SDH sysmon message ID for binary number response.
    SVL_SDH_SYSMON_MSG_BINARY_INFO_REQ       = 0x1002, //!< SDH sysmon message ID for binary info request.
    SVL_SDH_SYSMON_MSG_BINARY_INFO_RESP      = 0x1A02, //!< SDH sysmon message ID for binary info response.
    SVL_SDH_SYSMON_MSG_DOWNLOAD_REQ          = 0x1003, //!< SDH sysmon message ID for download request.
    SVL_SDH_SYSMON_MSG_DOWNLOAD_RESP         = 0x1A03, //!< SDH sysmon message ID for download response.
    SVL_SDH_SYSMON_MSG_BINARY_CHUNK_REQ      = 0x1004, //!< SDH sysmon message ID for binary chunk request.
    SVL_SDH_SYSMON_MSG_BINARY_CHUNK_RESP     = 0x1A04, //!< SDH sysmon message ID for bianry chunk response.
    SVL_SDH_SYSMON_MSG_SOFTWARE_INSTALL_REQ  = 0x1005, //!< SDH sysmon message ID for install request.
    SVL_SDH_SYSMON_MSG_SOFTWARE_INSTALL_RESP = 0x1A05, //!< SDH sysmon message ID for install response.
    SVL_SDH_SYSMON_MSG_BINARY_ERASE_REQ      = 0x1006, //!< SDH sysmon message ID for erase request.
    SVL_SDH_SYSMON_MSG_BINARY_ERASE_RESP     = 0x1A06  //!< SDH sysmon message ID for erase response.
}svl_sdhSysmonMsgId_t;

/*
 * Static variables
 */
/**
 * Read function pointer.
 */
GOS_STATIC svl_sdhReadWriteFunc_t sdhReadFunction = NULL;

/**
 * Write function pointer.
 */
GOS_STATIC svl_sdhReadWriteFunc_t sdhWriteFunction = NULL;

/**
 * SDH buffer for sysmon messages.
 */
GOS_STATIC u8_t  sdhBuffer [SVL_SDH_BUFFER_SIZE];

/**
 * Control trigger from IPL or sysmon to SDH task.
 */
GOS_STATIC gos_trigger_t sdhControlTrigger;

/**
 * Feedback trigger from SDH task to IPL or sysmon.
 */
GOS_STATIC gos_trigger_t sdhControlFeedbackTrigger;

/**
 * SDH state-machine variable.
 */
GOS_STATIC svl_sdhState_t sdhState = SDH_STATE_IDLE;

/**
 * SDH requested state.
 */
GOS_STATIC svl_sdhState_t sdhRequestedState = SDH_STATE_IDLE;

/*
 * Function prototypes
 */
GOS_STATIC void_t svl_sdhDaemon                           (void_t);
GOS_STATIC void_t svl_sdhSysmonBinaryNumReqCallback       (void_t);
GOS_STATIC void_t svl_sdhSysmonBinaryInfoReqCallback      (void_t);
GOS_STATIC void_t svl_sdhSysmonDownloadReqCallback        (void_t);
GOS_STATIC void_t svl_sdhSysmonBinaryChunkReqCallback     (void_t);
GOS_STATIC void_t svl_sdhSysmonSoftwareInstallReqCallback (void_t);
GOS_STATIC void_t svl_sdhSysmonBinaryEraseReqCallback     (void_t);
GOS_STATIC void_t svl_sdhIplBinaryNumReqCallback          (u8_t* pData, u32_t size, u32_t crc);
GOS_STATIC void_t svl_sdhIplBinaryInfoReqCallback         (u8_t* pData, u32_t size, u32_t crc);
GOS_STATIC void_t svl_sdhIplDownloadReqCallback           (u8_t* pData, u32_t size, u32_t crc);
GOS_STATIC void_t svl_sdhIplBinaryChunkReqCallback        (u8_t* pData, u32_t size, u32_t crc);
GOS_STATIC void_t svl_sdhIplSoftwareInstallReqCallback    (u8_t* pData, u32_t size, u32_t crc);
GOS_STATIC void_t svl_sdhIplBinaryEraseReqCallback        (u8_t* pData, u32_t size, u32_t crc);

/**
 * Sysmon binary number request.
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t sysmonBinaryNumReqMsg =
{
    .callback        = svl_sdhSysmonBinaryNumReqCallback,
    .messageId       = SVL_SDH_SYSMON_MSG_BINARY_NUM_REQ,
    .payload         = NULL,
    .payloadSize     = 0u,
    .protocolVersion = 1u
};

/**
 * Sysmon binary info request.
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t sysmonBinaryInfoReqMsg =
{
    .callback        = svl_sdhSysmonBinaryInfoReqCallback,
    .messageId       = SVL_SDH_SYSMON_MSG_BINARY_INFO_REQ,
    .payload         = (void_t*)sdhBuffer,
    .payloadSize     = sizeof(u16_t),
    .protocolVersion = 1u
};

/**
 * Sysmon download request.
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t sysmonDownloadReqMsg =
{
    .callback        = svl_sdhSysmonDownloadReqCallback,
    .messageId       = SVL_SDH_SYSMON_MSG_DOWNLOAD_REQ,
    .payload         = (void_t*)sdhBuffer,
    .payloadSize     = sizeof(svl_sdhBinaryDesc_t),
    .protocolVersion = 1u
};

/**
 * Sysmon binary chunk request.
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t sysmonBinaryChunkReqMsg =
{
    .callback        = svl_sdhSysmonBinaryChunkReqCallback,
    .messageId       = SVL_SDH_SYSMON_MSG_BINARY_CHUNK_REQ,
    .payload         = (void_t*)sdhBuffer,
    .payloadSize     = sizeof(svl_sdhChunkDesc_t) + SVL_SDH_CHUNK_SIZE,
    .protocolVersion = 1u
};

/**
 * Sysmon software install request.
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t sysmonSoftwareInstallReqMsg =
{
    .callback        = svl_sdhSysmonSoftwareInstallReqCallback,
    .messageId       = SVL_SDH_SYSMON_MSG_SOFTWARE_INSTALL_REQ,
    .payload         = (void_t*)sdhBuffer,
    .payloadSize     = sizeof(u16_t),
    .protocolVersion = 1u
};

/**
 * Sysmon binary erase request.
 */
GOS_STATIC gos_sysmonUserMessageDescriptor_t sysmonBinaryEraseReqMsg =
{
    .callback        = svl_sdhSysmonBinaryEraseReqCallback,
    .messageId       = SVL_SDH_SYSMON_MSG_BINARY_ERASE_REQ,
    .payload         = (void_t*)sdhBuffer,
    .payloadSize     = sizeof(u16_t) + sizeof(bool_t),
    .protocolVersion = 1u
};

/**
 * IPL binary number request.
 */
GOS_STATIC svl_iplUserMsgDesc_t iplBinaryNumReqMsg =
{
    .callback        = svl_sdhIplBinaryNumReqCallback,
    .msgId           = 0x02
};

/**
 * IPL binary info request.
 */
GOS_STATIC svl_iplUserMsgDesc_t iplBinaryInfoReqMsg =
{
    .callback        = svl_sdhIplBinaryInfoReqCallback,
    .msgId           = 0x12
};

/**
 * IPL download request.
 */
GOS_STATIC svl_iplUserMsgDesc_t iplDownloadReqMsg =
{
    .callback        = svl_sdhIplDownloadReqCallback,
    .msgId           = 0x22
};

/**
 * IPL binary chunk request.
 */
GOS_STATIC svl_iplUserMsgDesc_t iplBinaryChunkReqMsg =
{
    .callback        = svl_sdhIplBinaryChunkReqCallback,
    .msgId           = 0x32
};

/**
 * IPL software install request.
 */
GOS_STATIC svl_iplUserMsgDesc_t iplSoftwareInstallReqMsg =
{
    .callback        = svl_sdhIplSoftwareInstallReqCallback,
    .msgId           = 0x42
};

/**
 * IPL binary erase request.
 */
GOS_STATIC svl_iplUserMsgDesc_t iplEraseReqMsg =
{
    .callback        = svl_sdhIplBinaryEraseReqCallback,
    .msgId           = 0x52
};

/**
 * SDH daemon descriptor.
 */
GOS_STATIC gos_taskDescriptor_t svlSdhTaskDesc =
{
    .taskFunction       = svl_sdhDaemon,
    .taskName           = "svl_sdh_daemon",
    .taskStackSize      = SVL_SDH_DAEMON_STACK_SIZE,
    .taskPriority       = SVL_SDH_DAEMON_PRIORITY,
    .taskPrivilegeLevel = GOS_TASK_PRIVILEGE_KERNEL
};

/*
 * Function: svl_sdhInit
 */
gos_result_t svl_sdhInit (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t initResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    // Register sysmon callbacks.
    GOS_CONCAT_RESULT(initResult, gos_sysmonRegisterUserMessage(&sysmonBinaryNumReqMsg));
    GOS_CONCAT_RESULT(initResult, gos_sysmonRegisterUserMessage(&sysmonBinaryInfoReqMsg));
    GOS_CONCAT_RESULT(initResult, gos_sysmonRegisterUserMessage(&sysmonDownloadReqMsg));
    GOS_CONCAT_RESULT(initResult, gos_sysmonRegisterUserMessage(&sysmonBinaryChunkReqMsg));
    GOS_CONCAT_RESULT(initResult, gos_sysmonRegisterUserMessage(&sysmonSoftwareInstallReqMsg));
    GOS_CONCAT_RESULT(initResult, gos_sysmonRegisterUserMessage(&sysmonBinaryEraseReqMsg));

    // Register IPL callbacks.
    GOS_CONCAT_RESULT(initResult, svl_iplRegisterUserMsg(&iplBinaryNumReqMsg));
    GOS_CONCAT_RESULT(initResult, svl_iplRegisterUserMsg(&iplBinaryInfoReqMsg));
    GOS_CONCAT_RESULT(initResult, svl_iplRegisterUserMsg(&iplDownloadReqMsg));
    GOS_CONCAT_RESULT(initResult, svl_iplRegisterUserMsg(&iplBinaryChunkReqMsg));
    GOS_CONCAT_RESULT(initResult, svl_iplRegisterUserMsg(&iplSoftwareInstallReqMsg));
    GOS_CONCAT_RESULT(initResult, svl_iplRegisterUserMsg(&iplEraseReqMsg));

    GOS_CONCAT_RESULT(initResult, gos_taskRegister(&svlSdhTaskDesc, NULL));
    GOS_CONCAT_RESULT(initResult, gos_triggerInit(&sdhControlTrigger));
    GOS_CONCAT_RESULT(initResult, gos_triggerInit(&sdhControlFeedbackTrigger));

    return initResult;
}

/*
 * Function: svl_sdhConfigure
 */
gos_result_t svl_sdhConfigure (svl_sdhCfg_t* pCfg)
{
    /*
     * Local variables.
     */
    gos_result_t cfgResult = GOS_ERROR;

    /*
     * Function code.
     */
    if (pCfg != NULL && pCfg->readFunction != NULL && pCfg->writeFunction != NULL)
    {
        sdhReadFunction  = pCfg->readFunction;
        sdhWriteFunction = pCfg->writeFunction;
        cfgResult        = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return cfgResult;
}

/*
 * Function: svl_sdhGetBinaryData
 */
gos_result_t svl_sdhGetBinaryData (u16_t index, svl_sdhBinaryDesc_t* pDesc)
{
    /*
     * Local variables.
     */
    gos_result_t dataGetResult = GOS_SUCCESS;
    u16_t        numOfBinaries = 0u;

    /*
     * Function code.
     */
    if (pDesc != NULL && sdhReadFunction != NULL)
    {
        (void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

        if (index < numOfBinaries)
        {
            (void_t) sdhReadFunction(
                    SVL_SDH_STORAGE_DESC_BIN_DESC_START + index * sizeof(svl_sdhBinaryDesc_t),
                    (u8_t*)pDesc,
                    sizeof(*pDesc)
                    );
        }
        else
        {
            // Invalid request.
            dataGetResult = GOS_ERROR;
        }
    }
    else
    {
        // NULL pointer error.
        dataGetResult = GOS_ERROR;
    }

    return dataGetResult;
}

/*
 * Function: svl_sdhSetBinaryData
 */
gos_result_t svl_sdhSetBinaryData (u16_t index, svl_sdhBinaryDesc_t* pDesc)
{
    /*
     * Local variables.
     */
    gos_result_t dataSetResult = GOS_SUCCESS;
    u16_t        numOfBinaries = 0u;

    /*
     * Function code.
     */
    if (pDesc != NULL && sdhReadFunction != NULL && sdhWriteFunction != NULL)
    {
        (void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

        if (index < numOfBinaries)
        {
            (void_t) sdhWriteFunction(
                    SVL_SDH_STORAGE_DESC_BIN_DESC_START + index * sizeof(svl_sdhBinaryDesc_t),
                    (u8_t*)pDesc,
                    sizeof(*pDesc)
                    );
        }
        else
        {
            // Invalid request.
            dataSetResult = GOS_ERROR;
        }
    }
    else
    {
        // NULL pointer error.
        dataSetResult = GOS_ERROR;
    }

    return dataSetResult;
}

/*
 * Function: svl_sdhReadBytesFromMemory
 */
gos_result_t svl_sdhReadBytesFromMemory (u32_t address, u8_t* pBuffer, u32_t size)
{
    /*
     * Local variables.
     */
    gos_result_t readResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    if (pBuffer != NULL && sdhReadFunction != NULL)
    {
        (void_t) sdhReadFunction(address, pBuffer, size);
    }
    else
    {
        // NULL pointer error.
        readResult = GOS_ERROR;
    }

    return readResult;
}

/**
 * @brief   SDH daemon task.
 * @details Handles the incoming requests via sysmon or IPL.
 *
 * @return -
 */
GOS_STATIC void_t svl_sdhDaemon (void_t)
{
    /*
     * Local variables.
     */
    svl_sdhBinaryDesc_t binaryDescriptor    = {0};
    svl_sdhBinaryDesc_t newBinaryDescriptor = {0};
    svl_sdhChunkDesc_t  chunkDesc           = {0};
    svl_pdhBldCfg_t     bldCfg              = {0};
    u16_t               numOfChunks         = 0u;
    u16_t               index               = 0u;
    u16_t               numOfBinaries       = 0u;
    u8_t                result              = 0u;
    u32_t               startAddress        = 0u;
    u32_t               fromAddress         = 0u;
    u32_t               totalCopySize       = 0u;
    bool_t              defragment          = GOS_FALSE;

    /*
     * Function code.
     */
    for (;;)
    {
        if (gos_triggerWait(&sdhControlTrigger, SVL_SDH_DAEMON_TRIGGER_VALUE, GOS_TRIGGER_ENDLESS_TMO) == GOS_SUCCESS)
        {
            switch (sdhRequestedState)
            {
                case SDH_STATE_BINARY_NUM_REQ:
                {
                    if (sdhReadFunction != NULL && sdhWriteFunction != NULL)
                    {
#if SVL_SDH_TRACE_LEVEL > 0
                        (void_t) gos_traceTrace(GOS_TRUE, "SDH binary number request received.\r\n");
#endif
                        u16_t numOfBinaries = 0u;

                        (void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

                        if (numOfBinaries == 0xFFFF)
                        {
                            numOfBinaries = 0u;
                            (void_t) sdhWriteFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));
                        }
                        else
                        {
                            // Number OK.
                        }

                        (void_t) memcpy((void_t*)sdhBuffer, &numOfBinaries, sizeof(numOfBinaries));

                        (void_t) gos_triggerIncrement(&sdhControlFeedbackTrigger);
                    }
                    else
                    {
                        // Request cannot be served.
                    }
                    break;
                }
                case SDH_STATE_BINARY_INFO_REQ:
                {
                    if (sdhReadFunction != NULL)
                    {
                        (void_t) memcpy((void_t*)&index, sdhBuffer, sizeof(index));

#if SVL_SDH_TRACE_LEVEL > 0
                        (void_t) gos_traceTraceFormatted(GOS_TRUE, "SDH binary info request received. Index: %u\r\n", index);
#endif
                        if (svl_sdhGetBinaryData(index, &binaryDescriptor) == GOS_SUCCESS)
                        {
                            (void_t) memcpy(sdhBuffer, (void_t*)&binaryDescriptor, sizeof(binaryDescriptor));
                            gos_triggerIncrement(&sdhControlFeedbackTrigger);
                        }
                        else
                        {
                            // Wrong request.
                        }
                    }
                    else
                    {
                        // Request cannot be served.
                    }
                    break;
                }
                case SDH_STATE_BINARY_INSTALL_REQ:
                {
                    if (sdhReadFunction != NULL)
                    {
                        (void_t) memcpy((void_t*)&index, (void_t*)sdhBuffer, sizeof(index));

#if SVL_SDH_TRACE_LEVEL > 0
                        (void_t) gos_traceTraceFormatted(GOS_TRUE, "SDH binary install request received. Index: %u\r\n", index);
#endif

                        (void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

                        if (index < numOfBinaries)
                        {
                            (void_t) svl_pdhGetBldCfg(&bldCfg);

                            bldCfg.installRequested = GOS_TRUE;
                            bldCfg.binaryIndex      = index;

                            (void_t) svl_pdhSetBldCfg(&bldCfg);

                            (void_t) memcpy((void_t*)sdhBuffer, (void_t*)&index, sizeof(index));

                            (void_t) gos_triggerIncrement(&sdhControlFeedbackTrigger);
                        }
                    }
                    else
                    {
                        // Request cannot be served.
                    }
                    break;
                }
                case SDH_STATE_BINARY_DOWNLOAD_REQ:
                {
                    if (sdhReadFunction != NULL)
                    {
#if SVL_SDH_TRACE_LEVEL > 0
                        (void_t) gos_traceTrace(GOS_TRUE, "SDH binary download request received.\r\n");
#endif
                        // Check if descriptor fits.
                        (void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

                        if ((SVL_SDH_STORAGE_DESC_BIN_DESC_START + ((numOfBinaries + 1) * sizeof(svl_sdhBinaryDesc_t))) < SVL_SDH_STORAGE_DESC_AREA_SIZE)
                        {
                            (void_t) memcpy((void_t*)&newBinaryDescriptor, (void_t*)sdhBuffer, sizeof(newBinaryDescriptor));

                            if (numOfBinaries > 0)
                            {
                                (void_t) sdhReadFunction(
                                        SVL_SDH_STORAGE_DESC_BIN_DESC_START + (numOfBinaries - 1) * sizeof(svl_sdhBinaryDesc_t),
                                        (u8_t*)&binaryDescriptor,
                                        sizeof(binaryDescriptor)
                                        );

                                newBinaryDescriptor.binaryLocation = binaryDescriptor.binaryLocation + binaryDescriptor.binaryInfo.size;
                            }
                            else
                            {
                                newBinaryDescriptor.binaryLocation = SVL_SDH_BINARY_AREA_START;
                            }

#if SVL_SDH_TRACE_LEVEL == 2
                            (void_t) gos_traceTraceFormatted(
                                    GOS_TRUE,
                                    "SDH new binary info:\r\n"
                                    "Name: %s\r\n"
                                    "Location: %u\r\n"
                                    "Size: %u\r\n"
                                    "Address: %u\r\n"
                                    "CRC: %u\r\n",
                                    newBinaryDescriptor.name,
                                    newBinaryDescriptor.binaryLocation,
                                    newBinaryDescriptor.binaryInfo.size,
                                    newBinaryDescriptor.binaryInfo.startAddress,
                                    newBinaryDescriptor.binaryInfo.crc);
#endif

                            if ((newBinaryDescriptor.binaryLocation + newBinaryDescriptor.binaryInfo.size) < SVL_SDH_BINARY_AREA_END)
                            {
                                (void_t) sdhWriteFunction(
                                        SVL_SDH_STORAGE_DESC_BIN_DESC_START + numOfBinaries * sizeof(svl_sdhBinaryDesc_t),
                                        (u8_t*)&newBinaryDescriptor,
                                        sizeof(newBinaryDescriptor)
                                );
                                result = SDH_DOWNLOAD_REQ_OK;

                                numOfChunks = newBinaryDescriptor.binaryInfo.size / SVL_SDH_CHUNK_SIZE + (newBinaryDescriptor.binaryInfo.size % SVL_SDH_CHUNK_SIZE == 0 ? 0 : 1);

                                sdhState = SDH_STATE_DOWNLOADING_BINARY;
                            }
                            else
                            {
                                // New binary does not fit.
                                result = SDH_DOWNLOAD_REQ_FILE_SIZE_ERR;
                            }
                        }
                        else
                        {
                            // There is not enough size for descriptor.
                            result = SDH_DOWNLOAD_REQ_DESC_SIZE_ERR;
                        }

                        (void_t) memcpy((void_t*)sdhBuffer, (void_t*)&result, sizeof(result));
                        (void_t) gos_triggerIncrement(&sdhControlFeedbackTrigger);
                    }
                    else
                    {
                        // Request cannot be served.
                    }
                    break;
                }
                case SDH_STATE_DOWNLOADING_BINARY:
                {
                    if (sdhReadFunction != NULL && sdhWriteFunction != NULL)
                    {
                        // Get chunk descriptor.
                        (void_t) memcpy((void_t*)&chunkDesc, (void_t*)sdhBuffer, sizeof(chunkDesc));

#if SVL_SDH_TRACE_LEVEL > 0
                        (void_t) gos_traceTrace(GOS_TRUE, "SDH binary chunk request received.\r\n");
#endif

#if SVL_SDH_TRACE_LEVEL == 2
                        u32_t percentage = 100 * 100 * (chunkDesc.chunkIdx + 1) / numOfChunks;
                        (void_t) gos_traceTraceFormatted(
                                GOS_TRUE,
                                "SDH chunk counter [%u/%u] ... %3u.%02u%%\r\n",
                                chunkDesc.chunkIdx + 1,
                                numOfChunks,
                                percentage / 100,
                                percentage % 100
                        );
#endif

                        // Save chunk.
                        (void_t) sdhWriteFunction(
                                newBinaryDescriptor.binaryLocation + chunkDesc.chunkIdx * SVL_SDH_CHUNK_SIZE,
                                (u8_t*)(/*controlMsg.pData*/sdhBuffer + sizeof(chunkDesc)),
                                SVL_SDH_CHUNK_SIZE
                        );

                        // Send response.
                        chunkDesc.result = 1;

                        (void_t) memcpy((void_t*)sdhBuffer, (void_t*)&chunkDesc, sizeof(chunkDesc));
                        (void_t) gos_triggerIncrement(&sdhControlFeedbackTrigger);

                        // Check if all chunks have been received.
                        if (chunkDesc.chunkIdx == (numOfChunks - 1))
                        {
#if SVL_SDH_TRACE_LEVEL > 0
                            (void_t) gos_traceTrace(GOS_TRUE, "SDH binary download finished.\r\n");
#endif
                            (void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));
                            numOfBinaries++;
                            (void_t) sdhWriteFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

                            sdhState = SDH_STATE_IDLE;
                        }
                        else
                        {
                            // Continue reception.
                        }
                    }
                    break;
                }
                case SDH_STATE_BINARY_ERASE_REQ:
                {
                    if (sdhReadFunction != NULL && sdhWriteFunction != NULL)
                    {
                        (void_t) memcpy((void_t*)&index, (void_t*)sdhBuffer, sizeof(index));
                        (void_t) memcpy((void_t*)&defragment, (void_t*)sdhBuffer + sizeof(index), sizeof(defragment));
#if SVL_SDH_TRACE_LEVEL > 0
                        (void_t) gos_traceTraceFormatted(GOS_TRUE, "SDH binary erase request received. Index: %u\r\n", index);
#endif

                        (void_t) sdhReadFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

                        if (index < numOfBinaries)
                        {
                            svl_sdhGetBinaryData(index, &binaryDescriptor);

                            // Start address to copy is the binary location of
                            // the binary that is being deleted.
                            startAddress = binaryDescriptor.binaryLocation;

                            // Reorganize descriptors and binary.
                            if (index != (numOfBinaries - 1))
                            {
                                // Move descriptors.
                                for (u16_t idx = 0u; idx < (numOfBinaries - (index + 1)); idx++)
                                {
                                    (void_t) svl_sdhGetBinaryData((index + 1 + idx), &binaryDescriptor);
                                    (void_t) svl_sdhSetBinaryData((index + idx), &binaryDescriptor);
                                }

                                // Move binaries and update descriptor data.
                                // Prepare variables.
                                if (defragment == GOS_TRUE)
                                {
                                    (void_t) svl_sdhGetBinaryData(index, &binaryDescriptor);
                                    fromAddress = binaryDescriptor.binaryLocation;

                                    (void_t) svl_sdhGetBinaryData((numOfBinaries - 2), &binaryDescriptor);
                                    totalCopySize = binaryDescriptor.binaryLocation + binaryDescriptor.binaryInfo.size - fromAddress;

                                    // Move in chunks using SDH buffer.
                                    for (u32_t cntr = 0u; (cntr * SVL_SDH_CHUNK_SIZE) < totalCopySize; cntr++)
                                    {
                                        if ((cntr + 1) * SVL_SDH_CHUNK_SIZE < totalCopySize)
                                        {
                                            (void_t) sdhReadFunction(fromAddress + (cntr * SVL_SDH_CHUNK_SIZE), sdhBuffer, SVL_SDH_CHUNK_SIZE);
                                            (void_t) sdhWriteFunction(startAddress + (cntr * SVL_SDH_CHUNK_SIZE), sdhBuffer, SVL_SDH_CHUNK_SIZE);
                                        }
                                        else
                                        {
                                            (void_t) sdhReadFunction(fromAddress + (cntr * SVL_SDH_CHUNK_SIZE), sdhBuffer, totalCopySize - (cntr * SVL_SDH_CHUNK_SIZE));
                                            (void_t) sdhWriteFunction(startAddress + (cntr * SVL_SDH_CHUNK_SIZE), sdhBuffer, totalCopySize - (cntr * SVL_SDH_CHUNK_SIZE));
                                        }
                                    }
                                }
                                else
                                {
                                    // No defragmentation needed.
                                }
                            }
                            else
                            {
                                // No moving required.
                            }

                            // Decrease number of binaries.
                            numOfBinaries--;
                            (void_t) sdhWriteFunction(SVL_SDH_STORAGE_DESC_AREA_START, (u8_t*)&numOfBinaries, sizeof(numOfBinaries));

                            (void_t) memcpy((void_t*)sdhBuffer, (void_t*)&index, sizeof(index));
                            (void_t) gos_triggerIncrement(&sdhControlFeedbackTrigger);
                        }
                        else
                        {
                            // Wrong request.
                        }
                    }
                    else
                    {
                        // Request cannot be served.
                    }
                    break;
                }
                default: break;
            }
            (void_t) gos_triggerReset(&sdhControlTrigger);
        }
        else
        {
            // Timeout.
#if SVL_SDH_TRACE_LEVEL > 0
            if (sdhState != SDH_STATE_IDLE)
            {
                (void_t) gos_traceTrace(GOS_TRUE, "SDH timeout.\r\n");
            }
            else
            {
                // Just message RX timeout in idle state.
            }
#endif
            sdhState = SDH_STATE_IDLE;
            (void_t) gos_taskSleep(100);
        }
    }
}

/**
 * @brief   Callback function for binary number request.
 * @details Handles the binary number request via sysmon.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sdhSysmonBinaryNumReqCallback (void_t)
{
    /*
     * Function code.
     */
    sdhRequestedState = SDH_STATE_BINARY_NUM_REQ;

    // Increment trigger to signal for task.
    (void_t) gos_triggerIncrement(&sdhControlTrigger);

    if (gos_triggerWait(&sdhControlFeedbackTrigger, SVL_SDH_FEEDBACK_TRIGGER_VALUE, 3000) == GOS_SUCCESS)
    {
        (void_t) gos_gcpTransmitMessage(
                CFG_SYSMON_GCP_CHANNEL_NUM,
                SVL_SDH_SYSMON_MSG_BINARY_NUM_RESP,
                (void_t*)sdhBuffer,
                sizeof(u16_t),
                0xFFFF);
    }
    else
    {
        // Nothing to do.
    }

    // Reset trigger to 0.
    (void_t) gos_triggerReset(&sdhControlFeedbackTrigger);
}

/**
 * @brief   Callback function for binary info request.
 * @details Handles the binary info request via sysmon.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sdhSysmonBinaryInfoReqCallback (void_t)
{
    /*
     * Function code.
     */
    sdhRequestedState = SDH_STATE_BINARY_INFO_REQ;

    // Increment trigger to signal for task.
    (void_t) gos_triggerIncrement(&sdhControlTrigger);

    if (gos_triggerWait(&sdhControlFeedbackTrigger, SVL_SDH_FEEDBACK_TRIGGER_VALUE, 3000) == GOS_SUCCESS)
    {
        (void_t) gos_gcpTransmitMessage(
                CFG_SYSMON_GCP_CHANNEL_NUM,
                SVL_SDH_SYSMON_MSG_BINARY_INFO_RESP,
                (void_t*)sdhBuffer,
                sizeof(svl_sdhBinaryDesc_t),
                0xFFFF);
    }
    else
    {
        // Nothing to do.
    }

    // Reset trigger to 0.
    (void_t) gos_triggerReset(&sdhControlFeedbackTrigger);
}

/**
 * @brief   Callback function for download request.
 * @details Handles the download request via sysmon.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sdhSysmonDownloadReqCallback (void_t)
{
    /*
     * Function code.
     */
    sdhRequestedState = SDH_STATE_BINARY_DOWNLOAD_REQ;

    // Increment trigger to signal for task.
    (void_t) gos_triggerIncrement(&sdhControlTrigger);

    if (gos_triggerWait(&sdhControlFeedbackTrigger, SVL_SDH_FEEDBACK_TRIGGER_VALUE, 3000) == GOS_SUCCESS)
    {
        (void_t) gos_gcpTransmitMessage(
                CFG_SYSMON_GCP_CHANNEL_NUM,
                SVL_SDH_SYSMON_MSG_DOWNLOAD_RESP,
                (void_t*)sdhBuffer,
                sizeof(u8_t),
                0xFFFF);
    }
    else
    {
        // Nothing to do.
    }

    // Reset trigger to 0.
    (void_t) gos_triggerReset(&sdhControlFeedbackTrigger);
}

/**
 * @brief   Callback function for binary chunk request.
 * @details Handles the binary chunk request via sysmon.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sdhSysmonBinaryChunkReqCallback (void_t)
{
    /*
     * Function code.
     */
    sdhRequestedState = SDH_STATE_DOWNLOADING_BINARY;

    // Increment trigger to signal for task.
    (void_t) gos_triggerIncrement(&sdhControlTrigger);

    if (gos_triggerWait(&sdhControlFeedbackTrigger, SVL_SDH_FEEDBACK_TRIGGER_VALUE, 10000) == GOS_SUCCESS)
    {
        (void_t) gos_gcpTransmitMessage(
                CFG_SYSMON_GCP_CHANNEL_NUM,
                SVL_SDH_SYSMON_MSG_BINARY_CHUNK_RESP,
                (void_t*)sdhBuffer,
                sizeof(svl_sdhChunkDesc_t),
                0xFFFF);
    }
    else
    {
        // Nothing to do.
    }

    // Reset trigger to 0.
    (void_t) gos_triggerReset(&sdhControlFeedbackTrigger);
}

/**
 * @brief   Callback function for install request.
 * @details Handles the install request via sysmon.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sdhSysmonSoftwareInstallReqCallback (void_t)
{
    /*
     * Function code.
     */
    sdhRequestedState = SDH_STATE_BINARY_INSTALL_REQ;

    // Increment trigger to signal for task.
    (void_t) gos_triggerIncrement(&sdhControlTrigger);

    if (gos_triggerWait(&sdhControlFeedbackTrigger, SVL_SDH_FEEDBACK_TRIGGER_VALUE, 3000) == GOS_SUCCESS)
    {
        (void_t) gos_gcpTransmitMessage(
                CFG_SYSMON_GCP_CHANNEL_NUM,
                SVL_SDH_SYSMON_MSG_SOFTWARE_INSTALL_RESP,
                (void_t*)sdhBuffer,
                sizeof(u16_t),
                0xFFFF);
    }
    else
    {
        // Nothing to do.
    }

    // Reset trigger to 0.
    (void_t) gos_triggerReset(&sdhControlFeedbackTrigger);
}

/**
 * @brief   Callback function for erase request.
 * @details Handles the erase request via sysmon.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sdhSysmonBinaryEraseReqCallback (void_t)
{
    /*
     * Function code.
     */
    sdhRequestedState = SDH_STATE_BINARY_ERASE_REQ;

    // Increment trigger to signal for task.
    (void_t) gos_triggerIncrement(&sdhControlTrigger);

    if (gos_triggerWait(&sdhControlFeedbackTrigger, SVL_SDH_FEEDBACK_TRIGGER_VALUE, 5000) == GOS_SUCCESS)
    {
        (void_t) gos_gcpTransmitMessage(
                CFG_SYSMON_GCP_CHANNEL_NUM,
                SVL_SDH_SYSMON_MSG_BINARY_ERASE_RESP,
                (void_t*)sdhBuffer,
                sizeof(u16_t),
                0xFFFF);
    }
    else
    {
        // Nothing to do.
    }

    // Reset trigger to 0.
    (void_t) gos_triggerReset(&sdhControlFeedbackTrigger);
}

/**
 * @brief     Callback function for binary number request.
 * @details   Handles the binary number request via IPL.
 *
 * @param[in] pData Pointer to the array containing the received data.
 * @param     size  Size of the received data.
 * @param     crc   CRC of the received data.
 *
 * @return  -
 */
GOS_STATIC void_t svl_sdhIplBinaryNumReqCallback (u8_t* pData, u32_t size, u32_t crc)
{
    /*
     * Function code.
     */
    sdhRequestedState = SDH_STATE_BINARY_NUM_REQ;

    // Increment trigger to signal for task.
    (void_t) gos_triggerIncrement(&sdhControlTrigger);

    if (gos_triggerWait(&sdhControlFeedbackTrigger, SVL_SDH_FEEDBACK_TRIGGER_VALUE, 3000) == GOS_SUCCESS)
    {
        (void_t) svl_iplSendMessage(0xA02, sdhBuffer, sizeof(u16_t));
    }
    else
    {
        // Nothing to do.
    }

    // Reset trigger to 0.
    (void_t) gos_triggerReset(&sdhControlFeedbackTrigger);
}

/**
 * @brief     Callback function for binary info request.
 * @details   Handles the binary info request via IPL.
 *
 * @param[in] pData Pointer to the array containing the received data.
 * @param     size  Size of the received data.
 * @param     crc   CRC of the received data.
 *
 * @return    -
 */
GOS_STATIC void_t svl_sdhIplBinaryInfoReqCallback (u8_t* pData, u32_t size, u32_t crc)
{
    /*
     * Function code.
     */
    sdhRequestedState = SDH_STATE_BINARY_INFO_REQ;

    if (size > SVL_SDH_BUFFER_SIZE)
    {
        size = SVL_SDH_BUFFER_SIZE;
    }
    else
    {
        // Size check OK.
    }

    (void_t) memcpy((void_t*)sdhBuffer, (void_t*)pData, size);

    // Increment trigger to signal for task.
    (void_t) gos_triggerIncrement(&sdhControlTrigger);

    if (gos_triggerWait(&sdhControlFeedbackTrigger, SVL_SDH_FEEDBACK_TRIGGER_VALUE, 3000) == GOS_SUCCESS)
    {
        (void_t) svl_iplSendMessage(0xA12, (u8_t*)sdhBuffer, sizeof(svl_sdhBinaryDesc_t));
    }
    else
    {
        // Nothing to do.
    }

    // Reset trigger to 0.
    (void_t) gos_triggerReset(&sdhControlFeedbackTrigger);
}

/**
 * @brief     Callback function for download request.
 * @details   Handles the download request via IPL.
 *
 * @param[in] pData Pointer to the array containing the received data.
 * @param     size  Size of the received data.
 * @param     crc   CRC of the received data.
 *
 * @return    -
 */
GOS_STATIC void_t svl_sdhIplDownloadReqCallback (u8_t* pData, u32_t size, u32_t crc)
{
    /*
     * Function code.
     */
    sdhRequestedState = SDH_STATE_BINARY_DOWNLOAD_REQ;

    if (size > SVL_SDH_BUFFER_SIZE)
    {
        size = SVL_SDH_BUFFER_SIZE;
    }
    else
    {
        // Size check OK.
    }

    (void_t) memcpy((void_t*)sdhBuffer, (void_t*)pData, size);

    // Increment trigger to signal for task.
    (void_t) gos_triggerIncrement(&sdhControlTrigger);

    if (gos_triggerWait(&sdhControlFeedbackTrigger, SVL_SDH_FEEDBACK_TRIGGER_VALUE, 3000) == GOS_SUCCESS)
    {
        (void_t) svl_iplSendMessage(0xA22, sdhBuffer, sizeof(u8_t));
    }
    else
    {
        // Nothing to do.
    }

    // Reset trigger to 0.
    (void_t) gos_triggerReset(&sdhControlFeedbackTrigger);
}

/**
 * @brief     Callback function for binary chunk request.
 * @details   Handles the binary chunk request via IPL.
 *
 * @param[in] pData Pointer to the array containing the received data.
 * @param     size  Size of the received data.
 * @param     crc   CRC of the received data.
 *
 * @return    -
 */
GOS_STATIC void_t svl_sdhIplBinaryChunkReqCallback (u8_t* pData, u32_t size, u32_t crc)
{
    /*
     * Function code.
     */
    sdhRequestedState = SDH_STATE_DOWNLOADING_BINARY;

    if (size > SVL_SDH_BUFFER_SIZE)
    {
        size = SVL_SDH_BUFFER_SIZE;
    }
    else
    {
        // Size check OK.
    }

    (void_t) memcpy((void_t*)sdhBuffer, (void_t*)pData, size);

    // Increment trigger to signal for task.
    (void_t) gos_triggerIncrement(&sdhControlTrigger);

    if (gos_triggerWait(&sdhControlFeedbackTrigger, SVL_SDH_FEEDBACK_TRIGGER_VALUE, 10000) == GOS_SUCCESS)
    {
        (void_t) svl_iplSendMessage(0xA32, sdhBuffer, sizeof(svl_sdhChunkDesc_t));
    }
    else
    {
        // Nothing to do.
    }

    // Reset trigger to 0.
    (void_t) gos_triggerReset(&sdhControlFeedbackTrigger);
}

/**
 * @brief     Callback function for install request.
 * @details   Handles the install request via IPL.
 *
 * @param[in] pData Pointer to the array containing the received data.
 * @param     size  Size of the received data.
 * @param     crc   CRC of the received data.
 *
 * @return    -
 */
GOS_STATIC void_t svl_sdhIplSoftwareInstallReqCallback (u8_t* pData, u32_t size, u32_t crc)
{
    /*
     * Function code.
     */
    sdhRequestedState = SDH_STATE_BINARY_INSTALL_REQ;

    if (size > SVL_SDH_BUFFER_SIZE)
    {
        size = SVL_SDH_BUFFER_SIZE;
    }
    else
    {
        // Size check OK.
    }

    (void_t) memcpy((void_t*)sdhBuffer, (void_t*)pData, size);

    // Increment trigger to signal for task.
    (void_t) gos_triggerIncrement(&sdhControlTrigger);

    if (gos_triggerWait(&sdhControlFeedbackTrigger, SVL_SDH_FEEDBACK_TRIGGER_VALUE, 3000) == GOS_SUCCESS)
    {
        (void_t) svl_iplSendMessage(0xA42, sdhBuffer, sizeof(u16_t));
    }
    else
    {
        // Nothing to do.
    }

    // Reset trigger to 0.
    (void_t) gos_triggerReset(&sdhControlFeedbackTrigger);
}

/**
 * @brief     Callback function for erase request.
 * @details   Handles the erase request via IPL.
 *
 * @param[in] pData Pointer to the array containing the received data.
 * @param     size  Size of the received data.
 * @param     crc   CRC of the received data.
 *
 * @return    -
 */
GOS_STATIC void_t svl_sdhIplBinaryEraseReqCallback (u8_t* pData, u32_t size, u32_t crc)
{
    /*
     * Function code.
     */
    sdhRequestedState = SDH_STATE_BINARY_ERASE_REQ;

    if (size > SVL_SDH_BUFFER_SIZE)
    {
        size = SVL_SDH_BUFFER_SIZE;
    }
    else
    {
        // Size check OK.
    }

    (void_t) memcpy((void_t*)sdhBuffer, (void_t*)pData, size);

    // Increment trigger to signal for task.
    (void_t) gos_triggerIncrement(&sdhControlTrigger);

    if (gos_triggerWait(&sdhControlFeedbackTrigger, SVL_SDH_FEEDBACK_TRIGGER_VALUE, 5000) == GOS_SUCCESS)
    {
        (void_t) svl_iplSendMessage(0xA52, sdhBuffer, sizeof(u16_t));
    }
    else
    {
        // Nothing to do.
    }

    // Reset trigger to 0.
    (void_t) gos_triggerReset(&sdhControlFeedbackTrigger);
}
