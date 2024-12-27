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
//! @file       drv_dac.h
//! @author     Ahmed Gazar
//! @date       2024-05-08
//! @version    1.0
//!
//! @brief      GOS2022 Library / DAC driver header.
//! @details    This component provides access to the DAC peripheries.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2024-05-08    Ahmed Gazar     Initial version created.
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
#ifndef DRV_DAC_H
#define DRV_DAC_H

/*
 * Includes
 */
#include <gos.h>
#include <drv_dma.h>
#include <drv_tmr.h>

/*
 * Type definitions
 */
/**
 * DAC periphery instance enum.
 */
typedef enum
{
    DRV_DAC_INSTANCE_1 = 0,                        //!< DAC1
    DRV_DAC_NUM_OF_INSTANCES                       //!< Number of DAC instances.
}drv_dacPeriphInstance_t;

/**
 * DAC descriptor type.
 */
typedef struct
{
    drv_dacPeriphInstance_t periphInstance;        //!< Periphery instance.
    u32_t                   trigger;               //!< TODO
    u32_t                   outputBuffer;          //!< TODO
    u32_t                   channel;               //!< TODO
    u32_t                   dataAlignment;         //!< TODO
    drv_dmaDescriptor_t*    dmaConfig;             //!< DMA configuration.
    drv_tmrDescriptor_t*    tmrConfig;             //!< TMR configuration.
}drv_dacDescriptor_t;

// TODO
gos_result_t drv_dacInit (
        void_t
        );

// TODO
gos_result_t drv_dacInitInstance (
        u8_t dacInstanceIndex
        );

// TODO
gos_result_t drv_dacStart (
    drv_dacPeriphInstance_t instance,
    u32_t*                  pBuffer,
    u32_t                   buffLen
    );

// TODO
/*gos_result_t drv_adcGetValueIT (
        drv_adcPeriphInstance_t instance, u16_t* pValue,
        u32_t                   mutexTmo, u32_t  triggerTmo
        );
        */
#endif
