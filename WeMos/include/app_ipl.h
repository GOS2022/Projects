#ifndef APP_IPL_H
#define APP_IPL_H

#include "gos.h"

typedef enum
{
    SVL_IPC_MSG_ID_SYS_RES            = 0x100A,
	SVL_IPC_MSG_ID_CPU_LOAD_REQ       = 0x100B,
	SVL_IPC_MSG_ID_CPU_LOAD_RESP      = 0x600B,
	SVL_IPC_MSG_ID_TASK_NUM_REQ       = 0x100C,
	SVL_IPC_MSG_ID_TASK_NUM_RESP      = 0x600C,
	SVL_IPC_MSG_ID_TASK_DATA_REQ      = 0x100D,
	SVL_IPC_MSG_ID_TASK_DATA_RESP     = 0x600D,
	SVL_IPC_MSG_ID_PING_REQ           = 0x100E,
	SVL_IPC_MSG_ID_PING_RESP          = 0x600E,
	SVL_IPC_MSG_ID_SYS_RUNTIME_REQ    = 0x100F,
	SVL_IPC_MSG_ID_SYS_RUNTIME_RESP   = 0x600F,
	SVL_IPC_MSG_ID_SWINFO_REQ         = 0x1010,
	SVL_IPC_MSG_ID_SWINFO_RESP        = 0x6010,
	SVL_IPC_MSG_ID_TASK_VAR_DATA_REQ  = 0x1011,
	SVL_IPC_MSG_ID_TASK_VAR_DATA_RESP = 0x6011,
}svl_ipl_msg_id_t;

/**
 * IPL system time set message.
 */
typedef struct __attribute__((packed))
{
    gos_time_t desiredSystemTime;                        //!< Desired system time.
}svl_iplSystimeSetMessage_t;

#define PDH_STRING_LENGTH       ( 48u )

typedef struct __attribute__((packed))
{
	u16_t 	            major;
	u16_t	            minor;
	u16_t 	            build;
	gos_time_t          date;
	char_t              name        [PDH_STRING_LENGTH];
	char_t              description [PDH_STRING_LENGTH];
	char_t              author      [PDH_STRING_LENGTH];
}svl_pdhSwVerInfo_t;

typedef struct __attribute__((packed))
{
	u16_t 	            major;
	u16_t	            minor;
}svl_pdhOsInfo_t;

typedef struct __attribute__((packed))
{
	u32_t               startAddress;
	u32_t               size;
	u32_t               crc;
}svl_pdhBinaryInfo_t;

typedef struct __attribute__((packed))
{
	svl_pdhSwVerInfo_t  bldLibVerInfo;
	svl_pdhSwVerInfo_t  bldSwVerInfo;
	svl_pdhOsInfo_t     bldOsInfo;
	svl_pdhBinaryInfo_t bldBinaryInfo;
	svl_pdhSwVerInfo_t  appLibVerInfo;
	svl_pdhSwVerInfo_t  appSwVerInfo;
	svl_pdhOsInfo_t     appOsInfo;
	svl_pdhBinaryInfo_t appBinaryInfo;
}svl_ipcSwInfoMsg_t;

/**
 * PDH hardware information structure.
 */
typedef struct __attribute__((packed))
{
	char_t              boardName    [PDH_STRING_LENGTH];
	char_t              revision     [PDH_STRING_LENGTH];
	char_t              author       [PDH_STRING_LENGTH];
	char_t              description  [PDH_STRING_LENGTH];
	gos_time_t          date;
	char_t              serialNumber [PDH_STRING_LENGTH];
}svl_pdhHwInfo_t;

/**
 * SDH binary descriptor type.
 */
typedef struct __attribute__((packed))
{
	char_t              name [32];
	u32_t               binaryLocation;
	svl_pdhBinaryInfo_t binaryInfo;
}svl_sdhBinaryDesc_t;

/**
 * SDH chunk descriptor.
 */
typedef struct __attribute__((packed))
{
	u16_t chunkIdx;
	u8_t  result;
}svl_sdhChunkDesc_t;

gos_result_t app_iplInit (void_t);

gos_result_t iplGetCpuLoad (u16_t* pLoad);

void_t iplSysReset ();

gos_result_t iplPing (u8_t* pResult);

gos_result_t iplGetRunTime (gos_runtime_t* pRunTime);

gos_result_t iplGetTaskNum (u16_t* pTaskNum);

gos_result_t iplGetTaskData (u16_t taskIdx, void_t* pTaskData);

gos_result_t iplGetTaskVarData (u16_t taskIdx, void_t* pTaskVarData);

gos_result_t iplGetHardwareInfo (void_t* pHwInfo);

gos_result_t iplGetSoftwareInfo (void_t* pSwInfo);

gos_result_t iplGetTaskModify (u16_t taskIdx, u8_t modificationType, void_t* pTaskModify);

gos_result_t iplSetSysTime (void_t* pSysTime);

gos_result_t iplGetBinaryNum (u16_t* pBinaryNum);

gos_result_t iplGetBinaryInfo (u16_t index, svl_sdhBinaryDesc_t* pBinaryInfo);

gos_result_t iplSendDownloadRequest (void_t* pBinaryInfo, u8_t* pResult);

gos_result_t iplSendBinaryChunk (void_t* pBinaryChunk, void_t* pResult);

gos_result_t iplSendInstallRequest (u16_t* pIndex);

gos_result_t iplSendEraseRequest (u16_t* pIndex, bool_t defragment);

void_t iplRestart (void_t);

#endif