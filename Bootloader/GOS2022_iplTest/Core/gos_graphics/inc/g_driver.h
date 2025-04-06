/*
 * g_driver.h
 *
 *  Created on: Jun 19, 2023
 *      Author: Gabor
 */

#ifndef GOS_GRAPHICS_INC_G_DRIVER_H_
#define GOS_GRAPHICS_INC_G_DRIVER_H_

#include "gos.h"
#include <g_window.h>

#define G_DRIVER_WINDOW_MAX_NUM ( 8U )

gos_result_t g_initGraphicsDriver (/*ili9341_t* lcd*/);

gos_result_t g_registerWindow (g_window_t* pWindow);

void_t g_activateWindow (g_window_t* pWindow);

void_t g_inactivateWindow (g_window_t* pWindow);

#endif /* GOS_GRAPHICS_INC_G_DRIVER_H_ */
