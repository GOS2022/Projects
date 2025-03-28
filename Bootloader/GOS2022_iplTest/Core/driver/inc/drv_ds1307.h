/*
 * drv_ds1307.h
 *
 *  Created on: Mar 26, 2025
 *      Author: Ahmed
 */

#ifndef DRIVER_INC_DRV_DS1307_H_
#define DRIVER_INC_DRV_DS1307_H_

#include "gos.h"

gos_result_t drv_ds1307TimeSet (gos_time_t* pTime, u8_t dow);

gos_result_t drv_ds1307TimeGet (gos_time_t* pTime, u8_t* dow);

#endif /* DRIVER_INC_DRV_DS1307_H_ */
