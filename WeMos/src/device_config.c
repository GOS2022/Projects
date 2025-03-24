#include "device_config.h"
#include "string.h"

GOS_STATIC device_config_t deviceConfig = {0};

gos_result_t deviceConfigGet (device_config_t* pDevConfig)
{
    gos_result_t configGetResult = GOS_ERROR;

    if (pDevConfig != NULL)
    {
        // TODO
        pDevConfig->port = deviceConfig.port; //3000;

        pDevConfig->ipAddress[0] = deviceConfig.ipAddress[0]; // 192;
        pDevConfig->ipAddress[1] = deviceConfig.ipAddress[1]; //168;
        pDevConfig->ipAddress[2] = deviceConfig.ipAddress[2]; //8;
        pDevConfig->ipAddress[3] = deviceConfig.ipAddress[3]; //184;

        pDevConfig->gateway[0] = deviceConfig.gateway[0]; //192;
        pDevConfig->gateway[1] = deviceConfig.gateway[1]; //168;
        pDevConfig->gateway[2] = deviceConfig.gateway[2]; //8;
        pDevConfig->gateway[3] = deviceConfig.gateway[3]; //1;

        pDevConfig->subnet[0] = deviceConfig.subnet[0]; //255;
        pDevConfig->subnet[1] = deviceConfig.subnet[1]; //255;
        pDevConfig->subnet[2] = deviceConfig.subnet[2]; //0;
        pDevConfig->subnet[3] = deviceConfig.subnet[3]; //0;

        strcpy(pDevConfig->ssid, deviceConfig.ssid /*"HUAWEI_2.4G-Qwa6"*/);
        strcpy(pDevConfig->pwd,  deviceConfig.pwd /*"bXxACd4P"*/);

        configGetResult = GOS_SUCCESS;
    }

    return configGetResult;
}

gos_result_t deviceConfigSet (device_config_t* pDevConfig)
{
    (void_t) memcpy(&deviceConfig, pDevConfig, sizeof(*pDevConfig));
    return GOS_SUCCESS;
}