/*
 * app.h
 *
 *  Created on: Sep 15, 2022
 *      Author: Gabor
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "gos.h"
#include <string.h>
#include "stm32f4xx.h"
#include "bsp_exio_handler.h"

typedef enum
{
	APP_USER_BUTTON = 0,
	APP_DEBOUNCE_BUTTON_NUM
}app_button_t;

typedef struct
{
	bool_t buttonXBlocked [APP_DEBOUNCE_BUTTON_NUM];
	u16_t  buttonXBlockCounter [APP_DEBOUNCE_BUTTON_NUM];
}app_button_block_t;

typedef enum
{
	APP_SYSMON_MSG_RTC_SET_REQ = 0x8001,
	APP_SYSMON_MSG_RTC_SET_RESP = 0x8A01
}app_sysmonMsgId_t;

gos_result_t app_iplBdSpecInit (void_t);

gos_result_t app_pdhBdSpecInit (void_t);

gos_result_t app_sdhBdSpecInit (void_t);

gos_result_t app_mdiBdSpecInit (void_t);

gos_result_t app_ersBdSpecInit (void_t);

gos_result_t app_ledInit (void_t);

gos_result_t app_lcdInit (void_t);

gos_result_t app_debouncerInit (void_t);

gos_result_t app_debouncerActivate (void_t);

gos_result_t bsp_rtcHandlerInit (void_t);

gos_result_t bsp_rtcHandlerGetTemperature (u16_t* pTemp);

gos_result_t app_tftInit (void_t);

void_t       app_tftSetBrightness (u8_t percentage);

gos_result_t app_exioInit (void_t);

gos_result_t app_dhsTestInit (void_t);

#endif /* INC_APP_H_ */
