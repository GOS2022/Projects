/*
 * cfg_def.h
 *
 *  Created on: Apr 2, 2024
 *      Author: Gabor
 */

#ifndef DRV_CFG_CFG_DEF_H_
#define DRV_CFG_CFG_DEF_H_

typedef enum
{
	DMA_CFG_UART5_RX,
	DMA_CFG_USART1,
	DMA_CFG_UART4_RX,
	DMA_CFG_ADC1,
	DMA_CFG_SPI3
}dma_cfgInstance_t;

typedef enum
{
	CFG_TMR_STEP,
	CFG_TMR_SYSTIMER
}cfg_tmrInstance_t;

#endif /* DRV_CFG_CFG_DEF_H_ */
