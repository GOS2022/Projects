// IPL board-specific implementation.

#include "app.h"
#include "svl_ipl.h"
#include "drv_uart.h"

GOS_STATIC gos_result_t app_iplBdSpecTransmit (u8_t* pData, u32_t dataSize, u32_t timeout);
GOS_STATIC gos_result_t app_iplBdSpecReceive (u8_t* pData, u32_t dataSize, u32_t timeout);

gos_result_t app_iplBdSpecInit (void_t)
{
	return svl_iplConfigure(app_iplBdSpecTransmit, app_iplBdSpecReceive);
}

GOS_STATIC gos_result_t app_iplBdSpecTransmit (u8_t* pData, u32_t dataSize, u32_t timeout)
{
	return drv_uartTransmitIT(DRV_UART_INSTANCE_4, pData, dataSize, timeout, timeout);
}

GOS_STATIC gos_result_t app_iplBdSpecReceive (u8_t* pData, u32_t dataSize, u32_t timeout)
{
	if (timeout < 0xFFFFFFFFu)
	{
		return drv_uartReceiveBlocking(DRV_UART_INSTANCE_4, pData, dataSize, timeout);
	}
	else
	{
		return drv_uartReceiveDMA(DRV_UART_INSTANCE_4, pData, dataSize, timeout, timeout);
	}
}
