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
//                                      (c) Ahmed Gazar, 2025
//
//*************************************************************************************************
//! @file       svl_sysmon.c
//! @author     Ahmed Gazar
//! @date       2025-06-18
//! @version    1.0
//!
//! @brief      GOS2022 Library / System Monitoring Service source.
//! @details    For a more detailed description of this service, please refer to @ref svl_sysmon.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-06-18    Ahmed Gazar     Initial version created
//*************************************************************************************************
//
// Copyright (c) 2025 Ahmed Gazar
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
#include <Arduino.h>
#include "svl_sysmon.h"

extern "C"{
#include <gos_gcp.h>
}

#define RX_TMO_MS ( 3000u )

gos_result_t drv_sysmonWirelessTransmit (u8_t* pData, u16_t size);
gos_result_t drv_sysmonWirelessReceive (u8_t* pBuffer, u16_t size);

gos_result_t svl_sysmonInit (void_t)
{
    gos_gcpInit();
    //Serial.begin(115200);
    return gos_gcpRegisterPhysicalDriver(0u, drv_sysmonWirelessTransmit, drv_sysmonWirelessReceive);
}

gos_result_t svl_sysmonSendReceiveMessage (u16_t txMessageId, void_t* pTxPayload, u16_t txPayloadSize,
                                           u16_t* pRxMessageId, void_t* pRxPayload, u16_t* rxPayloadSize)
{
    int result = GOS_SUCCESS;
    Serial.begin(115200);
    // Clear buffer.
    //int i = Serial.available();
    //byte tmpBuff [i];
    //Serial.readBytes(tmpBuff, i);

    result &= gos_gcpTransmitMessage(0u, txMessageId, pTxPayload, txPayloadSize, 0xffff);
    result &= gos_gcpReceiveMessage(0u, pRxMessageId, pRxPayload, rxPayloadSize, 0xffff);
    //Serial.end();

    if (result != GOS_SUCCESS)
    {
        result = GOS_ERROR;
    }

    return (gos_result_t)result;
}

/*
 * Function: drv_sysmonWirelessTransmit
 */
gos_result_t drv_sysmonWirelessTransmit (u8_t* pData, u16_t size)
{
    /*
     * Local variables.
     */
    gos_result_t             uartTransmitResult = GOS_SUCCESS;

    Serial.write(pData, size);

    gos_taskSleep(35);

    return uartTransmitResult;
}

/*
 * Function: drv_sysmonWirelessReceive
 */
gos_result_t drv_sysmonWirelessReceive (u8_t* pBuffer, u16_t size)
{
    /*
     * Local variables.
     */
    gos_result_t uartReceiveResult  = GOS_SUCCESS;
    u32_t tickStart = gos_kernelGetSysTicks();

    while (!Serial.available() && (gos_kernelGetSysTicks() - tickStart) < RX_TMO_MS);
    if ((gos_kernelGetSysTicks() - tickStart) >= RX_TMO_MS)
    {
        uartReceiveResult = GOS_ERROR;
    }
    else
    {
        Serial.readBytes(pBuffer, size);
    }

    gos_taskSleep(35);

    return uartReceiveResult;
}