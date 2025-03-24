#include "wifiserver.h"
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include "messages.h"

#define LED_BUILTINN D4

extern "C"
{
    #include "device_config.h"
    #include "app_ipl.h"
}

#define WIFISERVER_CONNECT_MAX_ATTEMPTS ( 20 )

GOS_STATIC WiFiServer server(0);
GOS_STATIC device_config_t devConfig = {0};
GOS_STATIC u8_t rxBuffer [4096];

GOS_STATIC void_t wifiServerTask (void_t);

gos_taskDescriptor_t wifiServerTaskDesc =
{
  .taskFunction = wifiServerTask,
  .taskPriority = 3,
  .taskPrivilegeLevel = GOS_TASK_PRIVILEGED_USER
};

GOS_STATIC bool_t taskRegistered = GOS_FALSE;

/*
 * Function: wifiServerInit
 */
gos_result_t wifiServerInit (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t    serverInitResult = GOS_ERROR;
    u16_t           attemptCntr      = 0;

    /*
     * Function code.
     */
    (void_t) strcpy(wifiServerTaskDesc.taskName, "wifi_server_task");
    if (deviceConfigGet(&devConfig) == GOS_SUCCESS &&
        WiFi.config(devConfig.ipAddress, devConfig.gateway, devConfig.subnet) &&
        WiFi.mode(WIFI_STA))
    {
        WiFi.begin(devConfig.ssid, devConfig.pwd);

        while (WiFi.status() != WL_CONNECTED && attemptCntr < WIFISERVER_CONNECT_MAX_ATTEMPTS)
        {
            digitalWrite(LED_BUILTINN, !digitalRead(LED_BUILTINN));
            delay(1000);
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            server = WiFiServer(devConfig.port);
            server.begin();

            if (taskRegistered == GOS_FALSE)
            {
                serverInitResult = gos_taskRegister(&wifiServerTaskDesc, NULL);
                taskRegistered = GOS_TRUE;
            }
            else
            {
                serverInitResult = GOS_SUCCESS;
            }

            digitalWrite(LED_BUILTINN, LOW);
        }
        else
        {
            // Error.
            digitalWrite(LED_BUILTINN, HIGH);
        }
    }
    else
    {
        // Error.
    }

    return serverInitResult;
}

svl_ipcCpuLoadMsg_t cpuLoadMsg;
svl_ipcPingMsg_t pingMsg;
svl_pdhHwInfo_t hwInfoMsg;
svl_ipcSwInfoMsg_t swInfoMsg;
svl_ipcTaskNumberMsg_t taskNumMsg;
svl_ipcTaskDataMsg_t taskDataMsg;
svl_ipcTaskDataGetMsg_t taskDataGetMsg;
svl_ipcTaskVariableDataMsg_t taskVarDataMsg;
ledTestMessage_t testMsg;
svl_ipcRunTimeMsg_t runTimeMsg;
wifi_taskModifyMsg_t taskModifMsg;
svl_iplSystimeSetMessage_t sysTimeSetMsg;
svl_sdhBinaryDesc_t binaryInfo; 

/*
 *
 */
GOS_STATIC void_t wifiServerTask (void_t)
{
    WiFiClient client = server.available();
    u8_t connectionLostCntr = 0;
    u16_t messageId = 0;

    while (client.localPort() != devConfig.port)
    {
        client = server.available();
        gos_taskSleep(5);
    }

	for (;;)
	{
		if (client.connected())
		{
            if (client.available() > 0)
            {                
                // Get message bytes.
                client.readBytes(rxBuffer, client.available());

                // Get message ID.
                messageId = ((rxBuffer[0] << 8) + rxBuffer[1]);

                gos_traceTraceFormatted(GOS_TRUE, "WiFi server message received with ID: %u.\r\n", messageId);
                // Handle message.
                switch (messageId)
                {
                    case MSG_LED_TEST_ID:
                    {
                        memcpy(&testMsg, &rxBuffer[2], sizeof(testMsg));

                        if (testMsg.ledState > 0)
                        {
                            digitalWrite(LED_BUILTIN, HIGH);
                        }
                        else
                        {
                            digitalWrite(LED_BUILTIN, LOW);
                        }
                        break;
                    }
                    case MSG_SYSTEM_RESET_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        iplSysReset();
                        gos_taskSleep(500);
                        digitalWrite(LED_BUILTIN, LOW);

                        break;
                    }
                    case MSG_CPU_LOAD_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        iplGetCpuLoad(&(cpuLoadMsg.cpuLoad));
                        client.write((u8_t*)&cpuLoadMsg, sizeof(cpuLoadMsg));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_PING_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        iplPing(&pingMsg.pingResult);
                        client.write((u8_t*)&pingMsg, sizeof(pingMsg));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_RUNTIME_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        iplGetRunTime(&runTimeMsg.sysRunTime);
                        client.write((u8_t*)&runTimeMsg, sizeof(runTimeMsg));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_TASK_VAR_DATA_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        memcpy(&taskDataGetMsg, &rxBuffer[2], sizeof(taskDataGetMsg));
                        iplGetTaskVarData(taskDataGetMsg.taskIdx, (void_t*)&taskVarDataMsg);
                        client.write((u8_t*)&taskVarDataMsg, sizeof(taskVarDataMsg));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_SWINFO_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        iplGetSoftwareInfo((void_t*)&swInfoMsg);
                        client.write((u8_t*)&swInfoMsg, sizeof(swInfoMsg));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_HWINFO_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        iplGetHardwareInfo((void_t*)&hwInfoMsg);
                        client.write((u8_t*)&hwInfoMsg, sizeof(hwInfoMsg));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_TASK_NUM_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        iplGetTaskNum(&taskNumMsg.taskNumber);
                        client.write((u8_t*)&taskNumMsg, sizeof(taskNumMsg));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_TASK_DATA_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        memcpy(&taskDataGetMsg, &rxBuffer[2], sizeof(taskDataGetMsg));
                        iplGetTaskData(taskDataGetMsg.taskIdx, (void_t*)&taskDataMsg);
                        client.write((u8_t*)&taskDataMsg, sizeof(taskDataMsg));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_TASK_MODIFY_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        memcpy(&taskModifMsg, &rxBuffer[2], sizeof(taskModifMsg));
                        iplGetTaskModify(taskModifMsg.taskIdx, taskModifMsg.modificationType, &taskModifMsg);
                        client.write((u8_t*)&taskModifMsg, sizeof(taskModifMsg));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_SYNC_TIME_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        memcpy(&sysTimeSetMsg, &rxBuffer[2], sizeof(sysTimeSetMsg));
                        iplSetSysTime(&sysTimeSetMsg);
                        //client.write((u8_t*)&sysTimeSetMsg, sizeof(sysTimeSetMsg));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_BINARY_NUM_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        u16_t binNum;
                        iplGetBinaryNum(&binNum);
                        client.write((u8_t*)&binNum, sizeof(binNum));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_BINARY_INFO_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        u16_t index;
                        memcpy(&index, &rxBuffer[2], sizeof(index));
                        iplGetBinaryInfo(index, &binaryInfo);
                        client.write((u8_t*)&binaryInfo, sizeof(binaryInfo));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_DOWNLOAD_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        u8_t result = 0;
                        iplSendDownloadRequest(&rxBuffer[2], &result);
                        client.write((u8_t*)&result, sizeof(result));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_BINARY_CHUNK_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        svl_sdhChunkDesc_t result;
                        iplSendBinaryChunk(&rxBuffer[2], &result);
                        client.write((u8_t*)&result, sizeof(result));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_INSTALL_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        u16_t index;
                        memcpy(&index, &rxBuffer[2], sizeof(index));
                        iplSendInstallRequest(&index);
                        client.write((u8_t*)&index, sizeof(index));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    case MSG_ERASE_REQ_ID:
                    {
                        digitalWrite(LED_BUILTIN, HIGH);
                        u16_t index;
                        bool_t defragment;
                        memcpy(&index, &rxBuffer[2], sizeof(index));
                        memcpy(&defragment, (rxBuffer + 2 + sizeof(index)), sizeof(defragment));
                        iplSendEraseRequest(&index, defragment);
                        client.write((u8_t*)&index, sizeof(index));
                        digitalWrite(LED_BUILTIN, LOW);
                        break;
                    }
                    default: break;
                }
            }
            else
            {

            }

           gos_taskSleep(10);
		}
		else
		{
            connectionLostCntr++;
			gos_taskSleep(10);

            if (connectionLostCntr > 5)
            {
                connectionLostCntr = 0;
                client = server.available();
                while (client.localPort() != devConfig.port)
                {
                    client = server.available();
                    gos_taskSleep(5);
                }
            }
		}
	}
}

/*GOS_STATIC gos_result_t wifiServerGcpReceiver (u8_t* pTarget, u16_t targetSize)
{
    return GOS_ERROR;
}

GOS_STATIC gos_result_t wifiServerGcpTransmitter (u8_t* pMessage, u16_t messageSize)
{
    Serial.write(pMessage, messageSize);
    return GOS_SUCCESS;
}*/