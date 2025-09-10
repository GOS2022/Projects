/*
 * gos_lib.c
 *
 *  Created on: Mar 26, 2025
 *      Author: Ahmed
 */

#include "gos_lib.h"
#include "stdlib.h"

GOS_STATIC char_t year_str [] = { BUILD_YEAR_CH0, BUILD_YEAR_CH1, BUILD_YEAR_CH2, BUILD_YEAR_CH3, '\0' };
GOS_STATIC char_t month_str [] = { BUILD_MONTH_CH0, BUILD_MONTH_CH1, '\0' };
GOS_STATIC char_t day_str [] = { BUILD_DAY_CH0, BUILD_DAY_CH1, '\0' };

/**
 * TODO
 * @return
 */
void_t gos_libGetBuildDate (gos_year_t* pYear, gos_month_t* pMonth, gos_day_t* pDay)
{
	/*
	 * Function code.
	 */
	*pYear = (gos_year_t)atoi(year_str);
	*pMonth = (gos_month_t)atoi(month_str);
	*pDay = (gos_day_t)atoi(day_str);
}
