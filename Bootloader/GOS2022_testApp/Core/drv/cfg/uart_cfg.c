/*
 * uart_cfg.c
 *
 *  Created on: Feb 21, 2024
 *      Author: Gabor
 */

#include <drv_uart.h>
#include <drv_dma.h>
#include "stm32f4xx_hal.h"
#include "cfg_def.h"

GOS_EXTERN drv_dmaDescriptor_t dmaConfig [];

/**
 * TODO
 */
GOS_CONST drv_uartDescriptor_t uartConfig [] =
{
	// USART1 for shell.
	{
		.periphInstance = DRV_UART_INSTANCE_1,
		.baudRate       = 115200,
		.wordLength     = UART_WORDLENGTH_8B,
		.stopBits       = UART_STOPBITS_1,
		.parity         = UART_PARITY_NONE,
		.mode           = UART_MODE_TX_RX,
		.hwFlowControl  = UART_HWCONTROL_NONE,
		.overSampling   = UART_OVERSAMPLING_16,
		.dmaConfigTx    = &dmaConfig[DMA_CFG_USART1]
	},
	// UART5 for sysmon.
	{
		.periphInstance = DRV_UART_INSTANCE_5,
		.baudRate       = 115200,
		.wordLength     = UART_WORDLENGTH_8B,
		.stopBits       = UART_STOPBITS_1,
		.parity         = UART_PARITY_NONE,
		.mode           = UART_MODE_TX_RX,
		.hwFlowControl  = UART_HWCONTROL_NONE,
		.overSampling   = UART_OVERSAMPLING_16,
		.dmaConfigRx    = &dmaConfig[DMA_CFG_UART5_RX],
	},
	// UART4 for WEMOS
	{
		.periphInstance = DRV_UART_INSTANCE_4,
		.baudRate       = 115200,
		.wordLength     = UART_WORDLENGTH_8B,
		.stopBits       = UART_STOPBITS_1,
		.parity         = UART_PARITY_NONE,
		.mode           = UART_MODE_TX_RX,
		.hwFlowControl  = UART_HWCONTROL_NONE,
		.overSampling   = UART_OVERSAMPLING_16,
		.dmaConfigRx    = &dmaConfig[DMA_CFG_UART4_RX],
	}
};

/**
 * TODO
 */
drv_uartServiceTimeoutConfig_t uartServiceTmoConfig =
{
	.traceMutexTmo          = 2000u,
	.traceTriggerTmo        = 2000u,
	.traceUnsafeTransmitTmo = 1000u,
	.sysmonRxMutexTmo       = 2000u,
	.sysmonRxTriggerTmo     = 500u,
	.sysmonTxMutexTmo       = 2000u,
	.sysmonTxTriggerTmo     = 2000u
};

/**
 * TODO
 */
GOS_CONST drv_uartPeriphInstance_t  uartServiceConfig [] =
{
	[DRV_UART_SYSMON_WIRED_INSTANCE]    = DRV_UART_INSTANCE_5,
	[DRV_UART_SYSMON_WIRELESS_INSTANCE] = DRV_UART_INSTANCE_4,
	[DRV_UART_TRACE_INSTANCE]           = DRV_UART_INSTANCE_1,
	[DRV_UART_SHELL_INSTANCE]           = DRV_UART_INSTANCE_INVALID
};

/**
 * TODO
 */
u32_t uartConfigSize = sizeof(uartConfig);
