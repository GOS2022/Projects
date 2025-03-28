#include <SoftwareSerial.h>
#include <Arduino.h>
#include "wifiserver.h"

extern "C" {
#include "gos.h"
#include "drv_crc.h"
#include "app_ipl.h"
#include "device_config.h"
#include "gos_task.h"
}

//#define SS_RX D2
//#define SS_TX D3

#define IPL_RX_BUFF_SIZE (1024)
/**
 * Ping OK value.
 */
#define SVL_IPL_PING_OK ( 40u )
//#define CFG_TASK_MAX_NAME_LENGTH        ( 32 )

typedef char_t   gos_taskName_t [CFG_TASK_MAX_NAME_LENGTH];    //!< Task name type.
typedef u32_t    gos_taskCSCounter_t;                          //!< Context-switch counter type.
typedef u16_t    gos_taskStackSize_t;                          //!< Task stack size type.

/**
 * IPL state machine states.
 */
typedef enum
{
	SVL_IPL_STATE_NOT_CONFIGURED, //!< Driver functions are missing.
	SVL_IPL_STATE_DISCOVER_START, //!< Discovery start.
	SVL_IPL_STATE_DISCOVER,       //!< Discovery (device detection).
	SVL_IPL_STATE_CONFIG_START,   //!< Configuration start.
	SVL_IPL_STATE_CONFIG,         //!< Configuration.
	SVL_IPL_STATE_CONNECT_START,  //!< Connection start.
	SVL_IPL_STATE_CONNECT,        //!< Connection (to the configured network).
	SVL_IPL_STATE_CONNECTED       //!< Connected.
}svl_iplState_t;

/**
 * IPL task modification type.
 */
typedef enum
{
	IPL_TASK_MODIFY_SUSPEND = 0, //!< Suspend.
	IPL_TASK_MODIFY_RESUME  = 1, //!< Resume.
	IPL_TASK_MODIFY_DELETE  = 2, //!< Delete.
	IPL_TASK_MODIFY_BLOCK   = 3, //!< Block.
	IPL_TASK_MODIFY_UNBLOCK = 4, //!< Unblock.
	IPL_TASK_MODIFY_WAKEUP  = 5  //!< Wake up.
}svl_iplTaskModifyType_t;

GOS_STATIC svl_iplState_t iplState = SVL_IPL_STATE_DISCOVER;

typedef struct __attribute__((packed))
{
	char_t masterDeviceId [32];
	char_t slaveDeviceId [32];
}app_iplDiscoveryMessage_t;

#define SVL_IPL_DEVCFG_WIFI_SSID_MAX_LENGTH  ( 48 )
#define SVL_IPL_DEVCFG_WIFI_PWD_MAX_LENGTH   ( 48 )
#define SVL_IPL_DEVCFG_IP_ADDR_ELEMENTS      ( 4 )
#define SVL_IPL_DEVCFG_GATEWAY_ADDR_ELEMENTS ( 4 )
#define SVL_IPL_DEVCFG_SUBNET_ADDR_ELEMENTS  ( 4 )

/**
 * IPL device configuration message.
 */
typedef struct __attribute__((packed))
{
    char_t ssid      [SVL_IPL_DEVCFG_WIFI_SSID_MAX_LENGTH];  //!< WiFi SSID.
    char_t pwd       [SVL_IPL_DEVCFG_WIFI_PWD_MAX_LENGTH];   //!< WiFi password.
    u8_t   ipAddress [SVL_IPL_DEVCFG_IP_ADDR_ELEMENTS];      //!< IP address.
    u8_t   gateway   [SVL_IPL_DEVCFG_GATEWAY_ADDR_ELEMENTS]; //!< Gateway.
    u8_t   subnet    [SVL_IPL_DEVCFG_SUBNET_ADDR_ELEMENTS];  //!< Sub-net.
    u16_t  port;                                             //!< Port.
}svl_iplDeviceConfigMessage_t;

GOS_STATIC void_t iplTask (void_t);

gos_taskDescriptor_t iplTaskDesc =
{
  .taskFunction = iplTask,
  .taskPriority = 9,
  .taskPrivilegeLevel = GOS_TASK_PRIVILEGED_USER
};

/**
 * IPL connect message.
 */
typedef struct
{
	bool_t connectSuccess;
}svl_iplConnectMessage_t;

typedef struct
{
	u16_t cpuLoad;
}svl_iplCpuLoadMsg_t;

typedef struct
{
    u8_t result;
}svl_iplPingMsg_t;

typedef struct
{
	gos_runtime_t sysRunTime;
}svl_iplRunTimeMsg_t;

typedef struct
{
	u16_t taskNumber;
}svl_iplTaskNumberMsg_t;

typedef struct __attribute__((packed))
{
    gos_taskState_t          taskState;                  //!< Task state.
    gos_taskPrio_t           taskPriority;               //!< Task priority.
    gos_taskCSCounter_t      taskCsCounter;              //!< Task context-switch counter.
    gos_runtime_t            taskRunTime;                //!< Task run-time.
    u16_t                    taskCpuUsageMax;            //!< Task CPU usage max value in [% x 100].
    u16_t                    taskCpuUsage;               //!< Task processor usage in [% x 100].
    gos_taskStackSize_t      taskStackMaxUsage;          //!< Task stack usage.
}app_iplTaskVariableDataMsg_t;

/**
 * Task data message structure.
 */
typedef struct __attribute__((packed))
{
    gos_taskState_t          taskState;                  //!< Task state.
    gos_taskPrio_t           taskPriority;               //!< Task priority.
    gos_taskPrio_t           taskOriginalPriority;       //!< Task original priority.
    gos_taskPrivilegeLevel_t taskPrivilegeLevel;         //!< Task privilege level.
    gos_taskName_t           taskName;                   //!< Task name.
    gos_tid_t                taskId;                     //!< Task ID (internal).
    gos_taskCSCounter_t      taskCsCounter;              //!< Task context-switch counter.
    gos_taskStackSize_t      taskStackSize;              //!< Task stack size.
    gos_runtime_t            taskRunTime;                //!< Task run-time.
    u16_t                    taskCpuUsageLimit;          //!< Task CPU usage limit in [% x 100].
    u16_t                    taskCpuUsageMax;            //!< Task CPU usage max value in [% x 100].
    u16_t                    taskCpuUsage;               //!< Task processor usage in [% x 100].
    gos_taskStackSize_t      taskStackMaxUsage;          //!< Task max. stack usage.
}app_iplTaskDataMsg_t;

/**
 * IPL task data get message.
 */
typedef struct
{
	u16_t taskIdx;
}svl_iplTaskDataGetMsg_t;

/**
 * IPL task modify message type.
 */
typedef struct
{
	u16_t taskIdx;
	u8_t  modificationType;
	u8_t  result;
}svl_iplTaskModifyMsg_t;

/**
 * IPL message header.
 */
typedef struct
{
    u32_t messageId;     //!< Message ID.
    u32_t messageLength; //!< Message length in bytes.
    u32_t reserved;      //!< Reserved for future use.
    u32_t messageCrc;    //!< Message CRC.
}svl_iplMsgHeader_t;

/**
 * IPL message IDs.
 */
typedef enum
{
    IPL_MSG_ID_DISCOVERY         = 0x01,  //!< Discovery request.
    IPL_MSG_ID_DISCOVERY_ACK     = 0xA01, //!< Discovery acknowledge.
    IPL_MSG_ID_CONFIG            = 0x11,  //!< Configuration request.
    IPL_MSG_ID_CONFIG_ACK        = 0xA11, //!< Configuration acknowledge.
    IPL_MSG_ID_CONNECT           = 0x21,  //!< Connect request.
    IPL_MSG_ID_CONNECT_ACK       = 0xA21, //!< Connect acknowledge.
    IPL_MSG_ID_CPU_LOAD          = 0x31,  //!< CPU load request.
    IPL_MSG_ID_CPU_LOAD_ACK      = 0xA31, //!< CPU load acknowledge.
	IPL_MSG_ID_RESET             = 0x41,  //!< Reset request.
    IPL_MSG_ID_PING              = 0x51,  //!< Ping request.
    IPL_MSG_ID_PING_ACK          = 0xA51, //!< Ping acknowledge.
	IPL_MSG_ID_TASK_NUM          = 0x61,  //!< Task number request.
	IPL_MSG_ID_TASK_NUM_ACK      = 0xA61, //!< Task number acknowledge.
	IPL_MSG_ID_TASK_DATA         = 0x71,  //!< Task data request.
	IPL_MSG_ID_TASK_DATA_ACK     = 0xA71, //!< Task data acknowledge.
	IPL_MSG_ID_TASK_VAR_DATA     = 0x81,  //!< Task variable data request.
	IPL_MSG_ID_TASK_VAR_DATA_ACK = 0xA81, //!< Task variable data acknowledge.
	IPL_MSG_ID_RUNTIME           = 0x91,  //!< Runtime request.
	IPL_MSG_ID_RUNTIME_ACK       = 0xA91, //!< Runtime acknowledge.
	IPL_MSG_ID_SWINFO            = 0xA1,  //!< Software info request.
	IPL_MSG_ID_SWINFO_ACK        = 0xAA1, //!< Software info acknowledge.
	IPL_MSG_ID_TASK_MODIFY       = 0xB1,  //!< Task modify request.
	IPL_MSG_ID_TASK_MODIFY_ACK   = 0xAB1, //!< Task modify acknowledge.
    IPL_MSG_ID_HWINFO            = 0xC1,  //!< Hardware info request.
    IPL_MSG_ID_HWINFO_ACK        = 0xAC1, //!< Hardware info acknowledge.
    IPL_MSG_ID_SYNC_TIME         = 0xD1,  //!< System time synchronization request.
	IPL_MSG_ID_SYNC_TIME_ACK     = 0xAD1, //!< System time synchronization acknowledge.

    IPL_MSG_ID_BINARY_NUM_REQ    = 0x02,
    IPL_MSG_ID_BINARY_NUM_RESP   = 0xA02,
    IPL_MSG_ID_BINARY_INFO_REQ   = 0x12,
    IPL_MSG_ID_BINARY_INFO_RESP  = 0xA12,
    IPL_MSG_ID_DOWNLOAD_REQ      = 0x22,
    IPL_MSG_ID_DOWNLOAD_RESP     = 0xA22,
    IPL_MSG_ID_BINARY_CHUNK_REQ  = 0x32,
    IPL_MSG_ID_BINARY_CHUNK_RESP = 0xA32,
    IPL_MSG_ID_SW_INSTALL_REQ    = 0x42,
    IPL_MSG_ID_SW_INSTALL_RESP   = 0xA42,
    IPL_MSG_ID_ERASE_REQ         = 0x52,
    IPL_MSG_ID_ERASE_RESP        = 0xA52
}svl_iplMsgId_t;

typedef struct
{
    u8_t dummy;
}svl_iplResetMsg_t;

GOS_STATIC svl_iplMsgHeader_t header;
GOS_STATIC svl_iplCpuLoadMsg_t cpuMessage;
GOS_STATIC svl_iplTaskNumberMsg_t taskNumberMessage;
GOS_STATIC svl_iplTaskDataGetMsg_t taskDataGetMsg;
GOS_STATIC svl_iplRunTimeMsg_t runTimeMessage;
GOS_STATIC svl_iplPingMsg_t pingMessage;
GOS_STATIC svl_iplResetMsg_t resetMessage;
GOS_STATIC svl_iplTaskModifyMsg_t taskModifyMsg;
GOS_STATIC u8_t iplRxBuffer [IPL_RX_BUFF_SIZE];
GOS_STATIC svl_iplSystimeSetMessage_t sysTimeSetMsg;

gos_result_t app_iplInit (void_t)
{
    (void_t) strcpy(iplTaskDesc.taskName, "app_ipl_task");
    return gos_taskRegister(&iplTaskDesc, NULL);
}

GOS_STATIC void_t iplTask (void_t)
{
    gos_taskSleep(1000);
    Serial.begin(115200);

    for (;;)
    {
        switch (iplState)
        {
            case SVL_IPL_STATE_DISCOVER:
            {
                gos_traceTrace(GOS_TRUE, "IPL discovery start...\r\n");
                while (!Serial.available()){ gos_taskSleep(5); }

                app_iplDiscoveryMessage_t receivedMsg;

                // Get header.
                Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
                // Receive message bytes.
                Serial.readBytes(iplRxBuffer, header.messageLength);

                if (header.messageId == IPL_MSG_ID_DISCOVERY && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
                {
                    gos_traceTrace(GOS_TRUE, "Discovery message received.\r\n");
                    // Convert buffer to received message.
                    (void_t) memcpy((void_t*)&receivedMsg, (void_t*)iplRxBuffer, header.messageLength);

                    // Fill out slave ID.
                    (void_t) strcpy(receivedMsg.slaveDeviceId, "WeMos D1-01");

                    // Update CRC.
                    (void_t) drv_crcGetCrc32((u8_t*)&receivedMsg, sizeof(receivedMsg), &header.messageCrc);

                    // Update message ID.
                    header.messageId = IPL_MSG_ID_DISCOVERY_ACK;

                    // Send header and data.
                    Serial.write((u8_t*)&header, sizeof(header));
                    Serial.write((u8_t*)&receivedMsg, sizeof(receivedMsg));

                    iplState = SVL_IPL_STATE_CONFIG;
                }
                else
                {
                    // Nothing to do. Wrong message.
                    gos_traceTrace(GOS_TRUE, "Discovery failed.\r\n");
                }
                break;
            }

            case SVL_IPL_STATE_CONFIG:
            {
                gos_traceTrace(GOS_TRUE, "IPL configuration start...\r\n");
                while (!Serial.available()){ gos_taskSleep(5); }

                svl_iplDeviceConfigMessage_t receivedMsg;

                // Get header.
                Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
                // Receive message bytes.
                Serial.readBytes(iplRxBuffer, header.messageLength);

                if (header.messageId == IPL_MSG_ID_CONFIG && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
                {
                    // Convert buffer to received message.
                    (void_t) memcpy((void_t*)&receivedMsg, (void_t*)iplRxBuffer, header.messageLength);

                    gos_traceTraceFormatted(GOS_TRUE,
                        "\r\nConfiguration received:\r\n"
                        "SSID:      \t%s\r\n"
                        "IP address:\t%u.%u.%u.%u\r\n"
                        "Port:      \t%u\r\n"
                        "Subnet:    \t%u.%u.%u.%u\r\n"
                        "Gateway:   \t%u.%u.%u.%u\r\n",                                         
                        receivedMsg.ssid,
                        receivedMsg.ipAddress[0],
                        receivedMsg.ipAddress[1],
                        receivedMsg.ipAddress[2],
                        receivedMsg.ipAddress[3],
                        receivedMsg.port,
                        receivedMsg.subnet[0],
                        receivedMsg.subnet[1],
                        receivedMsg.subnet[2],
                        receivedMsg.subnet[3],
                        receivedMsg.gateway[0],
                        receivedMsg.gateway[1],
                        receivedMsg.gateway[2],
                        receivedMsg.gateway[3]                        
                    );

                    // Retrieve configuration.
                    device_config_t config;
                    (void_t) memcpy(&config.gateway, &receivedMsg.gateway, sizeof(config.gateway));
                    (void_t) memcpy(&config.subnet, &receivedMsg.subnet, sizeof(config.subnet));
                    (void_t) memcpy(&config.ipAddress, &receivedMsg.ipAddress, sizeof(config.ipAddress));
                    config.port = receivedMsg.port;
                    (void_t) strcpy(config.ssid, receivedMsg.ssid);
                    (void_t) strcpy(config.pwd, receivedMsg.pwd);

                    (void_t) deviceConfigSet(&config);

                    // Update message ID.
                    header.messageId = IPL_MSG_ID_CONFIG_ACK;

                    // Send header and data.
                    Serial.write((u8_t*)&header, sizeof(header));
                    Serial.write((u8_t*)&receivedMsg, sizeof(receivedMsg));

                    iplState = SVL_IPL_STATE_CONNECT;
                }
                else
                {
                    // Nothing to do. Wrong message.
                    gos_traceTrace(GOS_TRUE, "Configuration failed.\r\n");
                    iplState = SVL_IPL_STATE_DISCOVER;
                }
                break;
            }

            case SVL_IPL_STATE_CONNECT:
            {
                gos_traceTrace(GOS_TRUE, "Connection started...\r\n");
                while(!Serial.available()){ gos_taskSleep(5); }
                //if (Serial.available())
                {
                    svl_iplConnectMessage_t resultMsg;

                    // Get header.
                    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
                    // Receive message bytes.
                    Serial.readBytes(iplRxBuffer, header.messageLength);

                    if (header.messageId == IPL_MSG_ID_CONNECT && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
                    {
                        // Update message ID.
                        header.messageId = IPL_MSG_ID_CONNECT_ACK;
                        header.messageLength = sizeof(resultMsg);

                        if (wifiServerInit() == GOS_SUCCESS)
                        {
                            // Return success message.
                            gos_traceTrace(GOS_TRUE, "Connection successful.\r\n");
                            resultMsg.connectSuccess = GOS_TRUE;
                            iplState = SVL_IPL_STATE_CONNECTED;
                        }
                        else
                        {
                            // Return error message.
                            gos_traceTrace(GOS_TRUE, "Connection failed.\r\n");
                            resultMsg.connectSuccess = GOS_FALSE;
                        }
                
                        // Update CRC.
                        (void_t) drv_crcGetCrc32((u8_t*)&resultMsg, sizeof(resultMsg), &header.messageCrc);

                        // Send header and data.
                        Serial.write((u8_t*)&header, sizeof(header));
                        Serial.write((u8_t*)&resultMsg, sizeof(resultMsg));
                    }
                    else
                    {
                        // Nothing to do. Wrong message.
                        Serial.begin(115200);
                        iplState = SVL_IPL_STATE_DISCOVER;
                    }
                }
                /*else
                {
                    // Wait.
                }*/

                break;
            }

            case SVL_IPL_STATE_CONNECTED:
            {
                // Now WiFi server is master.
                break;
            }
        
            default:
                break;
        }

        gos_taskSleep(100);
    }
}

void_t iplRestart (void_t)
{
    iplState = SVL_IPL_STATE_DISCOVER;
}

gos_result_t iplGetCpuLoad (u16_t* pLoad)
{
    gos_result_t getResult = GOS_ERROR;

    header.messageId = IPL_MSG_ID_CPU_LOAD;
    header.messageLength = sizeof(cpuMessage);
    (void_t) drv_crcGetCrc32((u8_t*)&cpuMessage, sizeof(cpuMessage), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&cpuMessage, sizeof(cpuMessage));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_CPU_LOAD_ACK && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)&cpuMessage, (void_t*)iplRxBuffer, header.messageLength);

        if (pLoad != NULL)
        {
            *pLoad = cpuMessage.cpuLoad;
            getResult = GOS_SUCCESS;
        }
        else
        {
            // Cannot save.
        }
    }
    else
    {
        // Wrong message.
    }

    return getResult;
}

void_t iplSysReset ()
{    
    header.messageId = IPL_MSG_ID_RESET;
    header.messageLength = sizeof(resetMessage);
    (void_t) drv_crcGetCrc32((u8_t*)&resetMessage, sizeof(resetMessage), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&resetMessage, sizeof(resetMessage));

    delay(1000);
    ESP.reset();
}

gos_result_t iplPing (u8_t* pResult)
{
    gos_result_t pingResult = GOS_ERROR;
    
    header.messageId = IPL_MSG_ID_PING;
    header.messageLength = sizeof(pingMessage);
    (void_t) drv_crcGetCrc32((u8_t*)&pingMessage, sizeof(pingMessage), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&pingMessage, sizeof(pingMessage));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_PING_ACK && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)&pingMessage, (void_t*)iplRxBuffer, header.messageLength);

        if (pResult != NULL)
        {
            *pResult = pingMessage.result;
            pingResult = GOS_SUCCESS;
        }
        else
        {
            // Cannot save.
        }
    }
    else
    {
        // Wrong message.
    }

    return pingResult;
}

gos_result_t iplGetRunTime (gos_runtime_t* pRunTime)
{
    gos_result_t runtimeResult = GOS_ERROR;
    
    header.messageId = IPL_MSG_ID_RUNTIME;
    header.messageLength = sizeof(runTimeMessage);
    (void_t) drv_crcGetCrc32((u8_t*)&runTimeMessage, sizeof(runTimeMessage), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&runTimeMessage, sizeof(runTimeMessage));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_RUNTIME_ACK && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)&runTimeMessage, (void_t*)iplRxBuffer, header.messageLength);

        if (pRunTime != NULL)
        {
            *pRunTime = runTimeMessage.sysRunTime;
            runtimeResult = GOS_SUCCESS;
        }
        else
        {
            // Cannot save.
        }
    }
    else
    {
        // Wrong message.
    }

    return runtimeResult;
}

gos_result_t iplGetTaskNum (u16_t* pTaskNum)
{
    gos_result_t taskNumGetResult = GOS_ERROR;
    
    header.messageId = IPL_MSG_ID_TASK_NUM;
    header.messageLength = sizeof(taskNumberMessage);
    (void_t) drv_crcGetCrc32((u8_t*)&taskNumberMessage, sizeof(taskNumberMessage), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&taskNumberMessage, sizeof(taskNumberMessage));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_TASK_NUM_ACK && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)&taskNumberMessage, (void_t*)iplRxBuffer, header.messageLength);

        if (pTaskNum != NULL)
        {
            *pTaskNum = taskNumberMessage.taskNumber;
            taskNumGetResult = GOS_SUCCESS;
        }
        else
        {
            // Cannot save.
        }
    }
    else
    {
        // Error.
    }

    return taskNumGetResult;
}

gos_result_t iplGetTaskData (u16_t taskIdx, void_t* pTaskData)
{
    gos_result_t taskDataGetResult = GOS_ERROR;
    
    header.messageId = IPL_MSG_ID_TASK_DATA;
    header.messageLength = sizeof(taskDataGetMsg);
    taskDataGetMsg.taskIdx = taskIdx;
    (void_t) drv_crcGetCrc32((u8_t*)&taskDataGetMsg, sizeof(taskDataGetMsg), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&taskDataGetMsg, sizeof(taskDataGetMsg));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_TASK_DATA_ACK && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)pTaskData, (void_t*)iplRxBuffer, header.messageLength);

        taskDataGetResult = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return taskDataGetResult;
}

gos_result_t iplGetTaskVarData (u16_t taskIdx, void_t* pTaskVarData)
{
    gos_result_t taskVarDataGetResult = GOS_ERROR;
    
    header.messageId = IPL_MSG_ID_TASK_VAR_DATA;
    header.messageLength = sizeof(taskDataGetMsg);
    taskDataGetMsg.taskIdx = taskIdx;
    (void_t) drv_crcGetCrc32((u8_t*)&taskDataGetMsg, sizeof(taskDataGetMsg), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&taskDataGetMsg, sizeof(taskDataGetMsg));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_TASK_VAR_DATA_ACK && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)pTaskVarData, (void_t*)iplRxBuffer, header.messageLength);

        taskVarDataGetResult = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return taskVarDataGetResult;
}

gos_result_t iplGetHardwareInfo (void_t* pHwInfo)
{
    gos_result_t hwInfoGetResult = GOS_ERROR;
    u8_t dummy;
    
    header.messageId = IPL_MSG_ID_HWINFO;
    header.messageLength = sizeof(dummy);

    (void_t) drv_crcGetCrc32((u8_t*)&dummy, sizeof(dummy), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&dummy, sizeof(dummy));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_HWINFO_ACK && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)pHwInfo, (void_t*)iplRxBuffer, header.messageLength);

        hwInfoGetResult = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return hwInfoGetResult;
}

gos_result_t iplGetSoftwareInfo (void_t* pSwInfo)
{
    gos_result_t swInfoGetResult = GOS_ERROR;
    u8_t dummy;
    
    header.messageId = IPL_MSG_ID_SWINFO;
    header.messageLength = sizeof(dummy);

    (void_t) drv_crcGetCrc32((u8_t*)&dummy, sizeof(dummy), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&dummy, sizeof(dummy));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_SWINFO_ACK && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)pSwInfo, (void_t*)iplRxBuffer, header.messageLength);

        swInfoGetResult = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return swInfoGetResult;
}

gos_result_t iplGetTaskModify (u16_t taskIdx, u8_t modificationType, void_t* pTaskModify)
{
    gos_result_t taskModifyResult = GOS_ERROR;
    
    taskModifyMsg.taskIdx = taskIdx;
    taskModifyMsg.modificationType = modificationType;
    taskModifyMsg.result = 10;
    
    header.messageId = IPL_MSG_ID_TASK_MODIFY;
    header.messageLength = sizeof(taskModifyMsg);

    (void_t) drv_crcGetCrc32((u8_t*)&taskModifyMsg, sizeof(taskModifyMsg), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&taskModifyMsg, sizeof(taskModifyMsg));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_TASK_MODIFY_ACK && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)pTaskModify, (void_t*)iplRxBuffer, header.messageLength);

        taskModifyResult = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return taskModifyResult;
}

gos_result_t iplSetSysTime (void_t* pSysTime)
{
    gos_result_t setSysTimeResult = GOS_ERROR;
    
    memcpy(&sysTimeSetMsg, pSysTime, sizeof(sysTimeSetMsg));

    header.messageId = IPL_MSG_ID_SYNC_TIME;
    header.messageLength = sizeof(sysTimeSetMsg);

    (void_t) drv_crcGetCrc32((u8_t*)&sysTimeSetMsg, sizeof(sysTimeSetMsg), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&sysTimeSetMsg, sizeof(sysTimeSetMsg));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_SYNC_TIME_ACK && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        setSysTimeResult = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return setSysTimeResult;
}

gos_result_t iplGetBinaryNum (u16_t* pBinaryNum)
{
    gos_result_t result = GOS_ERROR;
    u8_t dummy = 0;

    header.messageId = IPL_MSG_ID_BINARY_NUM_REQ;
    header.messageLength = sizeof(dummy);

    (void_t) drv_crcGetCrc32((u8_t*)&dummy, sizeof(dummy), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&dummy, sizeof(dummy));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_BINARY_NUM_RESP && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)pBinaryNum, (void_t*)iplRxBuffer, sizeof(u16_t));

        result = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return result;
}

gos_result_t iplGetBinaryInfo (u16_t index, svl_sdhBinaryDesc_t* pBinaryInfo)
{
    gos_result_t result = GOS_ERROR;
    
    header.messageId = IPL_MSG_ID_BINARY_INFO_REQ;
    header.messageLength = sizeof(index);

    (void_t) drv_crcGetCrc32((u8_t*)&index, sizeof(index), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&index, sizeof(index));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_BINARY_INFO_RESP && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)pBinaryInfo, (void_t*)iplRxBuffer, sizeof(svl_sdhBinaryDesc_t));

        result = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return result;
}

gos_result_t iplSendDownloadRequest (void_t* pBinaryInfo, u8_t* pResult)
{
    gos_result_t result = GOS_ERROR;
    svl_sdhBinaryDesc_t binaryDesc;
    memcpy(&binaryDesc, pBinaryInfo, sizeof(binaryDesc));

    header.messageId = IPL_MSG_ID_DOWNLOAD_REQ;
    header.messageLength = sizeof(svl_sdhBinaryDesc_t);

    (void_t) drv_crcGetCrc32((u8_t*)&binaryDesc, sizeof(binaryDesc), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&binaryDesc, sizeof(binaryDesc));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_DOWNLOAD_RESP && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)pResult, (void_t*)iplRxBuffer, header.messageLength);

        result = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return result;
}

u8_t chunkBuffer [2048];
#define CHUNK_SIZE ( 1024u )

gos_result_t iplSendBinaryChunk (void_t* pBinaryChunk, void_t* pResult)
{
    gos_result_t result = GOS_ERROR;
    //svl_sdhChunkDesc_t chunkDesc;
    memcpy(chunkBuffer, pBinaryChunk, CHUNK_SIZE + sizeof(svl_sdhChunkDesc_t));

    header.messageId = IPL_MSG_ID_BINARY_CHUNK_REQ;
    header.messageLength = sizeof(svl_sdhChunkDesc_t) + CHUNK_SIZE;

    (void_t) drv_crcGetCrc32((u8_t*)chunkBuffer, header.messageLength, &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)chunkBuffer, header.messageLength);

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_BINARY_CHUNK_RESP && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)pResult, (void_t*)iplRxBuffer, header.messageLength);

        result = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return result;
}

gos_result_t iplSendInstallRequest (u16_t* pIndex)
{
    gos_result_t result = GOS_ERROR;
    u16_t index = *pIndex;

    header.messageId = IPL_MSG_ID_SW_INSTALL_REQ;
    header.messageLength = sizeof(index);

    (void_t) drv_crcGetCrc32((u8_t*)&index, sizeof(index), &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)&index, sizeof(index));

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 5000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_SW_INSTALL_RESP && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)pIndex, (void_t*)iplRxBuffer, sizeof(*pIndex));

        result = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return result;
}

gos_result_t iplSendEraseRequest (u16_t* pIndex, bool_t defragment)
{
    gos_result_t result = GOS_ERROR;
    u16_t index = *pIndex;
    u8_t payload[] = { (u8_t)(index >> 8), (u8_t)(index & 0x00ff), defragment };

    header.messageId = IPL_MSG_ID_ERASE_REQ;
    header.messageLength = sizeof(index) + sizeof(defragment);

    (void_t) drv_crcGetCrc32((u8_t*)payload, 3, &header.messageCrc);

    // Send header and data.
    Serial.write((u8_t*)&header, sizeof(header));
    delay(20);
    Serial.write((u8_t*)payload, 3);

    u32_t tickStart = gos_kernelGetSysTicks();
    while (!Serial.available())
    {
        if ((gos_kernelGetSysTicks() - tickStart) > 15000)
        {
            return GOS_ERROR;
        }
    }
    
    // Get header.
    Serial.readBytes((u8_t*)&header, sizeof(svl_iplMsgHeader_t));
    // Receive message bytes.
    Serial.readBytes(iplRxBuffer, header.messageLength);

    if (header.messageId == IPL_MSG_ID_ERASE_RESP && drv_crcCheckCrc32(iplRxBuffer, header.messageLength, header.messageCrc, NULL) == DRV_CRC_CHECK_OK)
    {
        // Convert buffer to received message.
        (void_t) memcpy((void_t*)pIndex, (void_t*)iplRxBuffer, sizeof(*pIndex));

        result = GOS_SUCCESS;
    }
    else
    {
        // Error.
    }

    return result;
}