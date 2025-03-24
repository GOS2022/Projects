#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include "gos.h"

#define DEVICE_WIFI_SSID_MAX_LENGTH  ( 32 )
#define DEVICE_WIFI_PWD_MAX_LENGTH   ( 32 )
#define DEVICE_IP_ADDR_ELEMENTS      ( 4 )
#define DEVICE_GATEWAY_ADDR_ELEMENTS ( 4 )
#define DEVICE_SUBNET_ADDR_ELEMENTS  ( 4 )

typedef struct
{
    char_t ssid      [DEVICE_WIFI_SSID_MAX_LENGTH];
    char_t pwd       [DEVICE_WIFI_PWD_MAX_LENGTH];
    u8_t   ipAddress [DEVICE_IP_ADDR_ELEMENTS];
    u8_t   gateway   [DEVICE_GATEWAY_ADDR_ELEMENTS];
    u8_t   subnet    [DEVICE_SUBNET_ADDR_ELEMENTS];
    u16_t  port;
}device_config_t;

gos_result_t deviceConfigGet (device_config_t* pDevConfig);
gos_result_t deviceConfigSet (device_config_t* pDevConfig);

#endif