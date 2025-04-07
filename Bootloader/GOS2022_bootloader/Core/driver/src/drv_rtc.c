/*
 * rtc_driver.c
 *
 *  Created on: Dec 24, 2022
 *      Author: Ahmed
 */

#include <drv_rtc.h>
#include "drv_i2c.h"

/*
 * Macros
 */
#define RTC_ADDRESS      (0xD0)
#define TEMP_SENSOR_ADDR (0xD0)

/*
 * Function prototypes
 */
GOS_STATIC u8_t  drv_rtcDec2bcd (u32_t dec);
GOS_STATIC u32_t drv_rtcBcd2dec (u8_t bcd);

/*
 * Function: drv_rtcTimeSet
 */
gos_result_t drv_rtcTimeSet (gos_time_t* pTime, u8_t dow)
{
	/*
	 * Local variables.
	 */
	gos_result_t setTimeResult = GOS_ERROR;
	u8_t         time [7];

	/*
	 * Function code.
	 */
	if (pTime != NULL)
	{
		time[0] = drv_rtcDec2bcd(pTime->seconds);
		time[1] = drv_rtcDec2bcd(pTime->minutes);
		time[2] = drv_rtcDec2bcd(pTime->hours);
		time[3] = drv_rtcDec2bcd(dow);
		time[4] = drv_rtcDec2bcd(pTime->days);
		time[5] = drv_rtcDec2bcd(pTime->months);
		time[6] = drv_rtcDec2bcd(pTime->years - 2000);

		setTimeResult = drv_i2cMemWrite(DRV_I2C_INSTANCE_1, RTC_ADDRESS, 0x00, 1, time, 7, GOS_MUTEX_ENDLESS_TMO, 100);
	}
	else
	{
		// NULL pointer.
	}

	return setTimeResult;
}

/*
 * Function: drv_rtcTimeGet
 */
gos_result_t drv_rtcTimeGet (gos_time_t* pTime, u8_t* dow)
{
	/*
	 * Local variables.
	 */
	gos_result_t getTimeResult = GOS_ERROR;
	u8_t         time [7];

	/*
	 * Function code.
	 */
	if (pTime != NULL)
	{
		getTimeResult = drv_i2cMemRead(DRV_I2C_INSTANCE_1, RTC_ADDRESS, 0x00, 1, time, 7, GOS_MUTEX_ENDLESS_TMO, 100);

		if (getTimeResult == GOS_SUCCESS)
		{
			pTime->seconds = drv_rtcBcd2dec(time[0]);
			pTime->minutes = drv_rtcBcd2dec(time[1]);
			pTime->hours   = drv_rtcBcd2dec(time[2]);
			if (dow != NULL)
			{
				*dow = drv_rtcBcd2dec(time[3]);
			}
			pTime->days    = drv_rtcBcd2dec(time[4]);
			pTime->months  = drv_rtcBcd2dec(time[5]);
			pTime->years   = 2000 + drv_rtcBcd2dec(time[6]);
		}
		else
		{
			// Communication error.
		}
	}
	else
	{
		// NULL pointer.
	}

	return getTimeResult;
}

/*
 * Function: drv_rtcTimeGet
 */
gos_result_t drv_rtcTempGet (u16_t* pTemp)
{
	/*
	 * Local variables.
	 */
	gos_result_t getTempResult = GOS_SUCCESS;
	u8_t         status        = 0u;
	u8_t         control       = 0u;
	u8_t         temp8 [2];

	/*
	 * Function code.
	 */
	if (pTemp != NULL)
	{
		getTempResult &= drv_i2cMemRead(DRV_I2C_INSTANCE_1, TEMP_SENSOR_ADDR, 0x0F, 1, &status, 1, GOS_MUTEX_ENDLESS_TMO, 100);

		if (!(status & 0x04) && getTempResult == GOS_SUCCESS)
		{
			getTempResult &= drv_i2cMemRead(DRV_I2C_INSTANCE_1, TEMP_SENSOR_ADDR, 0x0E, 1, &control, 1, GOS_MUTEX_ENDLESS_TMO, 100);
			getTempResult &= drv_i2cMemWrite(DRV_I2C_INSTANCE_1, TEMP_SENSOR_ADDR, 0x0E, 1, (u8_t*)(control | 0x20), 1, GOS_MUTEX_ENDLESS_TMO, 100);
			getTempResult &= drv_i2cMemRead(DRV_I2C_INSTANCE_1, TEMP_SENSOR_ADDR, 0x11, 1, temp8, 2, GOS_MUTEX_ENDLESS_TMO, 100);

			if (getTempResult == GOS_SUCCESS)
			{
				*pTemp = (u16_t)(((temp8[0]) + (temp8[1] >> 6) / 4.0) * 100);
			}
			else
			{
				// Error.
			}
		}
		else
		{
			getTempResult = GOS_ERROR;
		}
	}
	else
	{
		// NULL pointer.
		getTempResult = GOS_ERROR;
	}

	return getTempResult;
}

// TODO
GOS_STATIC u8_t drv_rtcDec2bcd (u32_t dec)
{
	/*
	 * Function code.
	 */
	return (u8_t)((dec / 10 * 16) + (dec % 10));
}

// TODO
GOS_STATIC u32_t drv_rtcBcd2dec (u8_t bcd)
{
	/*
	 * Function code.
	 */
	return (u32_t)((bcd / 16 * 10) + (bcd % 16));
}
