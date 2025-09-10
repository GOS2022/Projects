//*************************************************************************************************
//
//                            #####             #####             #####
//                          #########         #########         #########
//                         ##                ##       ##       ##
//                        ##                ##         ##        #####
//                        ##     #####      ##         ##           #####
//                         ##       ##       ##       ##                ##
//                          #########         #########         #########
//                            #####             #####             #####
//
//                                      (c) Ahmed Gazar, 2025
//
//*************************************************************************************************
//! @file       drv_ds3231.c
//! @author     Ahmed Gazar
//! @date       2025-04-09
//! @version    1.0
//!
//! @brief      GOS2022 Library / DS3231 real time clock driver source.
//! @details    For a more detailed description of this driver, please refer to @ref drv_ds3231.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2025-04-09    Ahmed Gazar     Initial version created.
//*************************************************************************************************
//
// Copyright (c) 2025 Ahmed Gazar
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//*************************************************************************************************
/*
 * Includes
 */
#include <drv_ds3231.h>

/*
 * Macros
 */
/**
 * Time register address.
 */
#define DS3231_REGISTER_TIME     ( 0x00 )

/**
 * Control register address.
 */
#define DS3231_REGISTER_CONT     ( 0x0E )

/**
 * Status register address.
 */
#define DS3231_REGISTER_STAT     ( 0x0F )

/**
 * Temperature register address.
 */
#define DS3231_REGISTER_TEMP     ( 0x11 )

/**
 * Bitmask for convert temperature bit in control register.
 */
#define DS3231_CONV_TEMP_BITMASK ( 0x20 )

/**
 * Bitmask for busy flag bit in status register.
 */
#define DS3231_BUSY_BITMASK      ( 0x04 )

/*
 * Function prototypes
 */
GOS_STATIC gos_result_t drv_ds3231Read    (drv_ds3231Descriptor_t* pDevice, u16_t reg, u8_t* pData, u16_t size);
GOS_STATIC gos_result_t drv_ds3231Write   (drv_ds3231Descriptor_t* pDevice, u16_t reg, u8_t* pData, u16_t size);
GOS_STATIC u8_t         drv_ds3231Dec2bcd (u32_t dec);
GOS_STATIC u32_t        drv_ds3231Bcd2dec (u8_t bcd);

/*
 * Function: drv_ds3231Init
 */
gos_result_t drv_ds3231Init (void_t* pDevice)
{
    /*
     * Local variables.
     */
    gos_result_t ds3231InitResult = GOS_SUCCESS;

    /*
     * Function code.
     */
    return ds3231InitResult;
}

/*
 * Function: drv_ds3231GetTime
 */
gos_result_t drv_ds3231GetTime (void_t* pDevice, gos_time_t* pTime, u8_t* dow)
{
	/*
	 * Local variables.
	 */
	gos_result_t getTimeResult = GOS_ERROR;
	u8_t         time [7];

	/*
	 * Function code.
	 */
	if ((pDevice != NULL) && (pTime != NULL))
	{
		getTimeResult = drv_ds3231Read((drv_ds3231Descriptor_t*)pDevice, DS3231_REGISTER_TIME, time, 7);

		if (getTimeResult == GOS_SUCCESS)
		{
			pTime->seconds = drv_ds3231Bcd2dec(time[0]);
			pTime->minutes = drv_ds3231Bcd2dec(time[1]);
			pTime->hours   = drv_ds3231Bcd2dec(time[2]);
			if (dow != NULL)
			{
				*dow = drv_ds3231Bcd2dec(time[3]);
			}
			pTime->days    = drv_ds3231Bcd2dec(time[4]);
			pTime->months  = drv_ds3231Bcd2dec(time[5]);
			pTime->years   = 2000 + drv_ds3231Bcd2dec(time[6]);
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
 * Function: drv_ds3231SetTime
 */
gos_result_t drv_ds3231SetTime (void_t* pDevice, gos_time_t* pTime, u8_t dow)
{
	/*
	 * Local variables.
	 */
	gos_result_t setTimeResult = GOS_ERROR;
	u8_t         time [7];

	/*
	 * Function code.
	 */
	if ((pDevice != NULL) && (pTime != NULL))
	{
		time[0] = drv_ds3231Dec2bcd(pTime->seconds);
		time[1] = drv_ds3231Dec2bcd(pTime->minutes);
		time[2] = drv_ds3231Dec2bcd(pTime->hours);
		time[3] = drv_ds3231Dec2bcd(dow);
		time[4] = drv_ds3231Dec2bcd(pTime->days);
		time[5] = drv_ds3231Dec2bcd(pTime->months);
		time[6] = drv_ds3231Dec2bcd(pTime->years - 2000);

		setTimeResult = drv_ds3231Write((drv_ds3231Descriptor_t*)pDevice, DS3231_REGISTER_TIME, time, 7);
	}
	else
	{
		// NULL pointer.
	}

	return setTimeResult;
}

/*
 * Function: drv_ds3231GetTemperature
 */
gos_result_t drv_ds3231GetTemperature (void_t* pDevice, u16_t* pTemp)
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
	if ((pDevice != NULL) && (pTemp != NULL))
	{
		GOS_CONCAT_RESULT(getTempResult, drv_ds3231Read((drv_ds3231Descriptor_t*)pDevice, DS3231_REGISTER_STAT, &status, 1));

		if (getTempResult == GOS_SUCCESS)
		{
			if (((status & DS3231_BUSY_BITMASK) == 0u))
			{
				GOS_CONCAT_RESULT(getTempResult, drv_ds3231Read((drv_ds3231Descriptor_t*)pDevice, DS3231_REGISTER_CONT, &control, 1));
				GOS_CONCAT_RESULT(getTempResult, drv_ds3231Write((drv_ds3231Descriptor_t*)pDevice, DS3231_REGISTER_CONT, (u8_t*)(control | DS3231_CONV_TEMP_BITMASK), 1));
				GOS_CONCAT_RESULT(getTempResult, drv_ds3231Read((drv_ds3231Descriptor_t*)pDevice, DS3231_REGISTER_TEMP, temp8, 2));

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
				getTempResult = GOS_BUSY;
			}
		}
		else
		{
			// Error.
		}
	}
	else
	{
		// NULL pointer.
		getTempResult = GOS_ERROR;
	}

	return getTempResult;
}

/**
 * TODO
 * @param pDevice
 * @param reg
 * @param pData
 * @return
 */
GOS_STATIC gos_result_t drv_ds3231Read (drv_ds3231Descriptor_t* pDevice, u16_t reg, u8_t* pData, u16_t size)
{
	/*
	 * Function code.
	 */
    return drv_i2cMemRead(pDevice->i2cInstance, ((pDevice->deviceAddress) << 1),
            reg, 1, pData, size, pDevice->readMutexTmo, pDevice->readTriggerTmo);
}

/**
 * TODO
 * @param reg
 * @param pData
 * @return
 */
GOS_STATIC gos_result_t drv_ds3231Write (drv_ds3231Descriptor_t* pDevice, u16_t reg, u8_t* pData, u16_t size)
{
	/*
	 * Function code.
	 */
    return drv_i2cMemWrite(pDevice->i2cInstance, ((pDevice->deviceAddress) << 1),
            reg, 1, pData, size, pDevice->writeMutexTmo, pDevice->writeTriggerTmo);
}

// TODO
GOS_STATIC u8_t drv_ds3231Dec2bcd (u32_t dec)
{
	/*
	 * Function code.
	 */
	return (u8_t)((dec / 10 * 16) + (dec % 10));
}

// TODO
GOS_STATIC u32_t drv_ds3231Bcd2dec (u8_t bcd)
{
	/*
	 * Function code.
	 */
	return (u32_t)((bcd / 16 * 10) + (bcd % 16));
}
