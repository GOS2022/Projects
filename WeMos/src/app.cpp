#include "wifiserver.h"
#include <string.h>
#include "svl_sysmon.h"
#include <Arduino.h>

extern "C" {
#include "gos.h"
#include "gos_task.h"
#include "device_config.h"
#include "app.h"
}

/**
 * Unified string length.
 */
#define PDH_STRING_LENGTH             ( 48u )

/**
 * IP address number of elements (IPv4).
 */
#define PDH_IP_ADDR_ELEMENT_NUM       ( 4u )

#define LED_RED    D7
#define LED_YELLOW D6
#define LED_GREEN  D5

/**
 * PDH WiFi configuration structure.
 */
typedef struct __attribute__((packed))
{
    char_t ssid      [PDH_STRING_LENGTH];       //!< WiFi SSID.
    char_t pwd       [PDH_STRING_LENGTH];       //!< WiFi password.
    u8_t   ipAddress [PDH_IP_ADDR_ELEMENT_NUM]; //!< IP address.
    u8_t   gateway   [PDH_IP_ADDR_ELEMENT_NUM]; //!< Gateway.
    u8_t   subnet    [PDH_IP_ADDR_ELEMENT_NUM]; //!< Sub-net.
    u16_t  port;
}svl_pdhWifiCfg_t;

GOS_STATIC void_t app_task (void_t);

gos_taskDescriptor_t appTaskDesc =
{
  .taskFunction = app_task,
  .taskPriority = 9,
  .taskPrivilegeLevel = GOS_TASK_PRIVILEGED_USER,
};

gos_result_t app_init (void_t)
{
    int result = GOS_SUCCESS;

    pinMode(LED_RED, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);

    (void_t) strcpy(appTaskDesc.taskName, "app_daemon");
    
    result &= gos_taskRegister(&appTaskDesc, NULL);
    
    return result == GOS_SUCCESS ? GOS_SUCCESS : GOS_ERROR;
}

void_t app_setStateFeedBack (app_state_t requiredState)
{
    switch (requiredState)
    {
        case APP_STATE_CONFIGURATION:
        {
            digitalWrite(LED_RED, HIGH);
            digitalWrite(LED_YELLOW, HIGH);
            digitalWrite(LED_GREEN, LOW);
            break;
        }
        case APP_STATE_CONNECTING:
        {
            digitalWrite(LED_RED, LOW);
            digitalWrite(LED_YELLOW, HIGH);
            digitalWrite(LED_GREEN, LOW);
            break;
        }
        case APP_STATE_CONNECTED:
        {
            digitalWrite(LED_RED, LOW);
            digitalWrite(LED_YELLOW, LOW);
            digitalWrite(LED_GREEN, HIGH);
            break;
        }
        case APP_STATE_CONNECTION_LOST:
        {
            digitalWrite(LED_RED, LOW);
            digitalWrite(LED_YELLOW, HIGH);
            digitalWrite(LED_GREEN, HIGH);
            break;
        }
        case APP_STATE_UNINITIALIZED:        
        default:
        {
            digitalWrite(LED_RED, HIGH);
            digitalWrite(LED_YELLOW, LOW);
            digitalWrite(LED_GREEN, LOW);
            break;
        }
    }
}

GOS_STATIC void_t app_task (void_t)
{
    app_setStateFeedBack(APP_STATE_UNINITIALIZED);
    gos_taskSleep(5000);
    svl_sysmonInit();

    // First get WiFi configuration from embedded side.
    svl_pdhWifiCfg_t wifiConfiguration = {0};
    u16_t rxMsgId = 0u;
    u16_t rxLength;

    gos_traceTrace(GOS_FALSE, "\r\n");
    gos_traceTrace(GOS_TRUE, "Connecting to embedded client...\r\n");
    app_setStateFeedBack(APP_STATE_CONNECTING);
#if 1
    while (svl_sysmonSendReceiveMessage(0x2003, NULL, 0u, &rxMsgId, (void_t*)&wifiConfiguration, &rxLength) != GOS_SUCCESS ||
          rxMsgId != 0x2A03)
    {        
        gos_traceTraceFormatted(GOS_TRUE, "Connection failed. Rx message ID: 0x%04x. Retrying...\r\n", rxMsgId);
        gos_taskSleep(1000);
    }

    // Retrieve configuration.
    app_setStateFeedBack(APP_STATE_CONFIGURATION);    
    device_config_t config;
    (void_t) memcpy(&config.gateway, &wifiConfiguration.gateway, sizeof(config.gateway));
    (void_t) memcpy(&config.subnet, &wifiConfiguration.subnet, sizeof(config.subnet));
    (void_t) memcpy(&config.ipAddress, &wifiConfiguration.ipAddress, sizeof(config.ipAddress));
    config.port = wifiConfiguration.port;
    (void_t) strcpy(config.ssid, wifiConfiguration.ssid);
    (void_t) strcpy(config.pwd, wifiConfiguration.pwd);
    (void_t) deviceConfigSet(&config);

    (void_t) gos_traceTraceFormatted(GOS_TRUE,
                        "\r\nConfiguration received:\r\n"
                        "SSID:      \t%s\r\n"
                        "IP address:\t%u.%u.%u.%u\r\n"
                        "Port:      \t%u\r\n"
                        "Subnet:    \t%u.%u.%u.%u\r\n"
                        "Gateway:   \t%u.%u.%u.%u\r\n",    
                        config.ssid,
                        config.ipAddress[0],
                        config.ipAddress[1],
                        config.ipAddress[2],
                        config.ipAddress[3],
                        config.port,
                        config.subnet[0],
                        config.subnet[1],
                        config.subnet[2],
                        config.subnet[3],
                        config.gateway[0],
                        config.gateway[1],
                        config.gateway[2],
                        config.gateway[3] 
    );

    // Now that WiFi is configured, start WiFiServer.
    wifiServerInit();
#endif
    for (;;)
    {
        gos_taskSuspend(appTaskDesc.taskId);
    }
}