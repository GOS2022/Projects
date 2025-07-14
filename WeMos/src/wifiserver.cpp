#include "wifiserver.h"
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include "messages.h"
#include "svl_sysmon.h"

#define LED_BUILTINN D4

extern "C"
{
    #include "device_config.h"
    #include "app_ipl.h"
    #include "app.h"
}

#define WIFISERVER_CONNECT_MAX_ATTEMPTS ( 20 )

GOS_STATIC WiFiServer server(0);
GOS_STATIC device_config_t devConfig = {0};
GOS_STATIC u8_t rxBuffer [4096];
GOS_STATIC u8_t txBuffer [2048];

GOS_STATIC void_t wifiServerTask (void_t);

gos_taskDescriptor_t wifiServerTaskDesc =
{
  .taskFunction = wifiServerTask,
  .taskPriority = 3,
  .taskPrivilegeLevel = GOS_TASK_PRIVILEGED_USER,
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
        app_setStateFeedBack(APP_STATE_CONNECTING);
        gos_traceTrace(GOS_TRUE, "Connecting to network..."); 
        WiFi.begin(devConfig.ssid, devConfig.pwd);

        while (WiFi.status() != WL_CONNECTED && attemptCntr < WIFISERVER_CONNECT_MAX_ATTEMPTS)
        {
            digitalWrite(LED_BUILTINN, !digitalRead(LED_BUILTINN));
            delay(1000);
            gos_traceTrace(GOS_FALSE, ".");
            //gos_taskSleep(1000);
        }

        gos_traceTrace(GOS_FALSE, "\r\n");

        if (WiFi.status() == WL_CONNECTED)
        {
            gos_traceTrace(GOS_FALSE, "Connected.\r\n");
            app_setStateFeedBack(APP_STATE_CONNECTED);
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


/*
 *
 */
GOS_STATIC void_t wifiServerTask (void_t)
{
    WiFiClient client = server.available();
    u8_t connectionLostCntr = 0;
    u16_t messageId = 0;
    u16_t messageLength = 0;

    u16_t rxMessageId = 0;
    u16_t rxMessageLength = 0;

    while (client.localPort() != devConfig.port)
    {
        client = server.available();
    }

	for (;;)
	{
		if (client.connected())
		{
            if (client.available() > 0)
            {
                messageId = 0;
                messageLength = 0;
                rxMessageId = 0;
                rxMessageLength = 0;
                memset(txBuffer, 0, 2048);

                // Get message bytes.
                client.readBytes(rxBuffer, client.available());

                // Get message ID.
                messageId = ((rxBuffer[0] << 8) + rxBuffer[1]);

                // Get message length.
                messageLength = ((rxBuffer[2] << 8) + rxBuffer[3]);

                // Serve request.
                gos_traceTraceFormatted(GOS_TRUE, "WiFi server message received with ID: %u.\r\n", messageId);
                digitalWrite(LED_BUILTIN, HIGH);
                    
                if (svl_sysmonSendReceiveMessage(messageId, rxBuffer + 4, messageLength, &rxMessageId, txBuffer, &rxMessageLength)!= GOS_SUCCESS)
                {                
                    gos_traceTraceFormatted(GOS_TRUE, "Communication failed.");                 
                }
                else
                {
                    gos_traceTraceFormatted(GOS_TRUE, "Replying with ID: %u, Length: %u\r\n", rxMessageId, rxMessageLength);
                }   
             
                client.write(txBuffer, rxMessageLength);
                digitalWrite(LED_BUILTIN, LOW);
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
                }
            }
		}
	}
}