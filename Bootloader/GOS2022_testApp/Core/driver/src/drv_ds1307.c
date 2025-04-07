/*
 * drv_ds1307.c
 *
 *  Created on: Mar 26, 2025
 *      Author: Ahmed
 */
#include "drv_ds1307.h"
#include "drv_i2c.h"
#include "string.h"

#define DS1307_ADDRESS (0x68 << 1) //(0x68 << 1) ///< I2C address for DS1307
//#define DS1307_CONTROL 0x07 ///< Control register
//#define DS1307_NVRAM 0x08   ///< Start of RAM registers - 56 bytes, 0x08 to 0x3f

/*
 * Function prototypes
 */
GOS_STATIC u8_t  drv_rtcDec2bcd (u32_t dec);
GOS_STATIC u32_t drv_rtcBcd2dec (u8_t bcd);

/*
 * Function: drv_ds1307TimeSet
 */
gos_result_t drv_ds1307TimeSet (gos_time_t* pTime, u8_t dow)
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
		time[0] = 0x7F & drv_rtcDec2bcd(pTime->seconds);
		time[1] = drv_rtcDec2bcd(pTime->minutes);
		time[2] = drv_rtcDec2bcd(pTime->hours);
		time[3] = 1;
		time[4] = drv_rtcDec2bcd(pTime->days);
		time[5] = drv_rtcDec2bcd(pTime->months);
		time[6] = drv_rtcDec2bcd(pTime->years - 2000);

		setTimeResult = drv_i2cMemWrite(DRV_I2C_INSTANCE_1, DS1307_ADDRESS, 0x00, 1, time, 7, GOS_MUTEX_ENDLESS_TMO, 500);
		//setTimeResult = drv_i2cTransmitBlocking(DRV_I2C_INSTANCE_1, DS1307_ADDRESS, time, 8, GOS_MUTEX_ENDLESS_TMO, 300);
	}
	else
	{
		// NULL pointer.
	}

	return setTimeResult;
}

/*
 * Function: drv_ds1307TimeGet
 */
gos_result_t drv_ds1307TimeGet (gos_time_t* pTime, u8_t* dow)
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
		(void_t) memset((void_t*)time, 0, sizeof(time));
		getTimeResult = drv_i2cMemRead(DRV_I2C_INSTANCE_1, DS1307_ADDRESS, 0x00, 1, time, 7, GOS_MUTEX_ENDLESS_TMO, 500);

		if (getTimeResult == GOS_SUCCESS)
		{
			pTime->seconds = drv_rtcBcd2dec(0x7F & time[0]);
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
