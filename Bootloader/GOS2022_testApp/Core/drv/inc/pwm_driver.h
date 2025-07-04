/*
 * pwm_driver.h
 *
 *  Created on: Jul 3, 2025
 *      Author: Gabor
 */

#ifndef DRV_INC_PWM_DRIVER_H_
#define DRV_INC_PWM_DRIVER_H_

#include "gos.h"

gos_result_t pwm_driver_init (void_t);

void_t pwm_driver_set_duty (u8_t duty);

#endif /* DRV_INC_PWM_DRIVER_H_ */
