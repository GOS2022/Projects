/*
 * rtc_driver.h
 *
 *  Created on: Dec 24, 2022
 *      Author: Ahmed
 */

#ifndef DRIVER_INC_DRV_RTC_H_
#define DRIVER_INC_DRV_RTC_H_

#include "gos.h"

gos_result_t drv_rtcTimeSet (gos_time_t* pTime, u8_t dow);

gos_result_t drv_rtcTimeGet (gos_time_t* pTime, u8_t* dow);

gos_result_t drv_rtcTempGet (u16_t* pTemp);

#endif /* DRIVER_INC_DRV_RTC_H_ */
