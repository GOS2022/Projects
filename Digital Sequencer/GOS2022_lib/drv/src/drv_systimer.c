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
//                                      (c) Ahmed Gazar, 2024
//
//*************************************************************************************************
//! @file       drv_systimer.c
//! @author     Ahmed Gazar
//! @date       2024-05-09
//! @version    1.0
//!
//! @brief      GOS2022 Library / System timer driver source.
//! @details    For a more detailed description of this driver, please refer to @ref drv_systimer.h
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-05-09    Ahmed Gazar     Initial version created.
//*************************************************************************************************
//
// Copyright (c) 2024 Ahmed Gazar
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
#include <drv_systimer.h>
#include <drv_tmr.h>

/*
 * External variables
 */
/**
 * TMR service configuration (shall be defined by user).
 */
GOS_EXTERN GOS_CONST drv_tmrPeriphInstance_t tmrServiceConfig [];

/*
 * Function: drv_systimerStart
 */
gos_result_t drv_systimerStart (void_t)
{
    /*
     * Local variables.
     */
    gos_result_t            systimerStartResult = GOS_ERROR;
    drv_tmrPeriphInstance_t instance            = 0u;

    /*
     * Function code.
     */
    if (tmrServiceConfig != NULL)
    {
        instance = tmrServiceConfig[DRV_TMR_SYSTIMER_INSTANCE];

        systimerStartResult = drv_tmrStart(instance, GOS_MUTEX_ENDLESS_TMO);
    }
    else
    {
        // Configuration array is NULL.
    }

    return systimerStartResult;
}

/*
 * Function: drv_systimerGetValue
 */
gos_result_t drv_systimerGetValue (u16_t* pValue)
{
    /*
     * Local variables.
     */
    gos_result_t            systimerGetValueResult = GOS_ERROR;
    drv_tmrPeriphInstance_t instance               = 0u;
    u32_t                   temp                   = 0u;

    /*
     * Function code.
     */
    if (tmrServiceConfig != NULL && pValue != NULL)
    {
        instance = tmrServiceConfig[DRV_TMR_SYSTIMER_INSTANCE];
        systimerGetValueResult = drv_tmrGetValue(instance, &temp);
        *pValue = (u16_t)temp;
    }
    else
    {
        // Configuration array is NULL or output parameter is NULL.
    }

    return systimerGetValueResult;
}
