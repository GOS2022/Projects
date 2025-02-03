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

#endif /* INC_APP_H_ */
