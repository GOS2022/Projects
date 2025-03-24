#ifndef WIFISERVER_H
#define WIFISERVER_H

extern "C" {
    #include "gos.h"
}

typedef enum
{
    MSG_LED_TEST_ID           = ( 0x1001 ),
    MSG_SYSTEM_RESET_ID       = ( 0x100A ),
    MSG_CPU_LOAD_REQ_ID       = ( 0x100B ),
    MSG_CPU_LOAD_RESP_ID      = ( 0x600B ),
    MSG_TASK_NUM_REQ_ID       = ( 0x100C ),
    MSG_TASK_NUM_RESP_ID      = ( 0x600C ),
    MSG_TASK_DATA_REQ_ID      = ( 0x100D ),
    MSG_TASK_DATA_RESP_ID     = ( 0x600D ),
    MSG_PING_REQ_ID           = ( 0x100E ),
    MSG_PING_RESP_ID          = ( 0x600E ),
    MSG_RUNTIME_REQ_ID        = ( 0x100F ),
    MSG_RUNTIME_RESP_ID       = ( 0x600F ),
    MSG_SWINFO_REQ_ID         = ( 0x1010 ),
    MSG_SWINFO_RESP_ID        = ( 0x6010 ),
    MSG_TASK_VAR_DATA_REQ_ID  = ( 0x1011 ),
    MSG_TASK_VAR_DATA_RESP_ID = ( 0x6011 ),
    MSG_TASK_MODIFY_REQ_ID    = ( 0x1012 ),
    MSG_TASK_MODIFY_RESP_ID   = ( 0x6012 ),
    MSG_HWINFO_REQ_ID         = ( 0x1013 ),
    MSG_HWINFO_RESP_ID        = ( 0x6013 ),
    MSG_SYNC_TIME_REQ_ID      = ( 0x1014 ),
    MSG_SYNC_TIME_RESP_ID     = ( 0x6014 ),

    MSG_BINARY_NUM_REQ_ID     = ( 0x1015 ),
    MSG_BINARY_NUM_RESP_ID    = ( 0x6015 ),
    MSG_BINARY_INFO_REQ_ID    = ( 0x1016 ),
    MSG_BINARY_INFO_RESP_ID   = ( 0x6016 ),
    MSG_DOWNLOAD_REQ_ID       = ( 0x1017 ),
    MSG_DOWNLOAD_RESP_ID      = ( 0x6017 ),
    MSG_BINARY_CHUNK_REQ_ID   = ( 0x1018 ),
    MSG_BIANRY_CHUNK_RESP_ID  = ( 0x6018 ),
    MSG_INSTALL_REQ_ID        = ( 0x1019 ),
    MSG_INSTALL_RESP_ID       = ( 0x6019 ),
    MSG_ERASE_REQ_ID          = ( 0x1020 ),
    MSG_ERASE_RESP_ID         = ( 0x6020 ),
}wifiserver_msgId_t;

typedef struct
{
	u16_t taskIdx;
	u8_t  modificationType;
	u8_t  result;
}wifi_taskModifyMsg_t;

gos_result_t wifiServerInit (void_t);

#endif