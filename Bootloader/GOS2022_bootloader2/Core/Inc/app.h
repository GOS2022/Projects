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

gos_result_t app_iplBdSpecInit (void_t);

gos_result_t app_pdhBdSpecInit (void_t);

gos_result_t app_sdhBdSpecInit (void_t);

gos_result_t app_ledInit (void_t);

gos_result_t app_bld_init (void_t);

#endif /* INC_APP_H_ */
