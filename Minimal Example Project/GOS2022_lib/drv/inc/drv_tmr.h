#ifndef DRV_TMR_H
#define DRV_TMR_H

/*
 * Includes
 */
#include <gos.h>

/*
 * Type definitions
 */
/**
 * TIM periphery instance enum.
 */
typedef enum
{
    DRV_TMR_INSTANCE_1,
    DRV_TMR_INSTANCE_2,
    DRV_TMR_INSTANCE_3,
    DRV_TMR_INSTANCE_4,
    DRV_TMR_INSTANCE_5,
    DRV_TMR_INSTANCE_6,
    DRV_TMR_INSTANCE_7,
    DRV_TMR_INSTANCE_8,
    DRV_TMR_INSTANCE_9,
    DRV_TMR_INSTANCE_10,
    DRV_TMR_INSTANCE_11,
    DRV_TMR_INSTANCE_12,
    DRV_TMR_INSTANCE_13,
    DRV_TMR_INSTANCE_14,
    DRV_TMR_NUM_OF_INSTANCES
}drv_tmrPeriphInstance_t;

/**
 * TIM descriptor type.
 */
typedef struct
{
    drv_tmrPeriphInstance_t periphInstance;
    u32_t                   prescaler;
    u32_t                   repetitionCounter;
    u32_t                   counterMode;
    u32_t                   period;
    u32_t                   clockDivision;
    u32_t                   autoReloadPreload;
    bool_t                  useClockConfig;
    u32_t                   clockSource;
    bool_t                  useMasterConfig;
    u32_t                   masterOutputTrigger;
    u32_t                   masterSlaveMode;
    void_t                  (*periodCallback)(void_t);
}drv_tmrDescriptor_t;

/**
 * TMR service instance enum.
 */
typedef enum
{
    DRV_TMR_SYSTIMER_INSTANCE = 0, //!< System timer instance.
}drv_tmrServiceInstance_t;

// TODO
gos_result_t drv_tmrInit (void_t);

// TODO
gos_result_t drv_tmrInitInstance (u8_t tmrInstanceIndex);

// TODO
gos_result_t drv_tmrStart (drv_tmrPeriphInstance_t instance, u32_t mutexTmo);

// TODO
gos_result_t drv_tmrStartIT (drv_tmrPeriphInstance_t instance, u32_t mutexTmo);

// TODO
gos_result_t drv_tmrStartDMA (drv_tmrPeriphInstance_t instance, u32_t* pData, u16_t dataLength, u32_t mutexTmo);

// TODO
gos_result_t drv_tmrGetValue (drv_tmrPeriphInstance_t instance, u32_t* pValue);

#endif
