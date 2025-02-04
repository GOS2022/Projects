/*
 * uart_cfg.c
 *
 *  Created on: Feb 21, 2024
 *      Author: Gabor
 */

#include <drv_uart.h>
#include <drv_dma.h>
#include <cfg_def.h>
#include "stm32f4xx_hal.h"

GOS_EXTERN drv_dmaDescriptor_t dmaConfig [];

/**
 * TODO
 */
GOS_CONST drv_uartDescriptor_t uartConfig [] =
{
	// USART2 for trace.
	{
		.periphInstance = DRV_UART_INSTANCE_2,
		.baudRate       = 115200,
		.wordLength     = UART_WORDLENGTH_8B,
		.stopBits       = UART_STOPBITS_1,
		.parity         = UART_PARITY_NONE,
		.mode           = UART_MODE_TX_RX,
		.hwFlowControl  = UART_HWCONTROL_NONE,
		.overSampling   = UART_OVERSAMPLING_16,
	},
};

/**
 * TODO
 */
drv_uartServiceTimeoutConfig_t uartServiceTmoConfig =
{
	.traceMutexTmo          = 2000u,
	.traceTriggerTmo        = 2000u,
	.traceUnsafeTransmitTmo = 1000u,
	.shellRxMutexTmo        = GOS_MUTEX_ENDLESS_TMO,
	.shellRxTriggerTmo      = GOS_TRIGGER_ENDLESS_TMO,
	.shellTxMutexTmo        = 2000u,
	.shellTxTriggerTmo      = 2000u,
	.sysmonRxMutexTmo       = 2000u,
	.sysmonRxTriggerTmo     = GOS_TRIGGER_ENDLESS_TMO,
	.sysmonTxMutexTmo       = 2000u,
	.sysmonTxTriggerTmo     = 2000u
};

/**
 * TODO
 */
GOS_CONST drv_uartPeriphInstance_t  uartServiceConfig [] =
{
	[DRV_UART_SYSMON_INSTANCE] = DRV_UART_INSTANCE_INVALID,
	[DRV_UART_TRACE_INSTANCE]  = DRV_UART_INSTANCE_2,
	[DRV_UART_SHELL_INSTANCE]  = DRV_UART_INSTANCE_INVALID
};

/**
 * TODO
 */
u32_t uartConfigSize = sizeof(uartConfig);
