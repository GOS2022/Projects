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
//                                      (c) Ahmed Gazar, 2023
//
//*************************************************************************************************
//! @file       gos_port.h
//! @author     Ahmed Gazar
//! @date       2026-07-19
//! @version    1.1
//!
//! @brief      GOS port header.
//! @details    This header contains the platform-specific ported definitions of the OS.
//*************************************************************************************************
// History
// ------------------------------------------------------------------------------------------------
// Version    Date          Author          Description
// ------------------------------------------------------------------------------------------------
// 1.0        2023-07-12    Ahmed Gazar     Initial version created.
// 1.1        2026-07-19    Ahmed Gazar     *    Context switch reworked with EXC_RETURN-aware
//                                               save/restore, MSP alignment handling, and
//                                               optional FP context save/restore
//                                          *    SVC/PendSV flow updated (direct SVC #255 handling,
//                                               special-case PendSV path, and dedicated
//                                               gos_ported_svcHandlerMain usage)
//                                          +    FPU context configuration and stronger barrier
//                                               sequencing added for startup/fault handling
//*************************************************************************************************
#ifndef GOS_PORT_H
#define GOS_PORT_H
/*
 * Includes
 */
#include "gos_kernel.h"
#include "gos_config.h"

/*
 * Macros
 */
/*
 * ARM_CORTEX_M4 definitions
 */
#if (CFG_TARGET_CPU == ARM_CORTEX_M4)

/**
 * Processor reset function.
 */
#define gos_ported_procReset() (                                                                                 \
{                                                                                                                \
    GOS_ASM ("dsb 0xF":::"memory");                                                                              \
    *(u32_t*)(0xE000ED0CUL) = (u32_t)((0x5FAUL << 16U) | (*(u32_t*)(0xE000ED0CUL) & (7UL << 8U)) | (1UL << 2U)); \
    GOS_ASM ("dsb 0xF":::"memory");                                                                              \
}                                                                                                                \
)

/**
 * Reschedule function.
 */
#define gos_ported_reschedule(privilege) (                        \
{                                                                 \
    if (privilege == GOS_PRIVILEGED)                              \
    {                                                             \
        /* Trigger PendSV directly. */                            \
        ICSR |= (1 << 28);                                        \
    }                                                             \
    else                                                          \
    {                                                             \
        /* Call Supervisor exception to get Privileged access. */ \
        GOS_ASM("SVC #255");                                      \
    }                                                             \
}                                                                 \
)

/**
 * Pend SV handler function name.
 */
#define gos_ported_pendSVHandler GOS_NAKED PendSV_Handler

/**
 * Context-switch function.
 */
#if defined(__VFP_FP__) && !defined(__SOFTFP__)
#define GOS_PORT_SAVE_FP_CONTEXT() (          \
{                                             \
    GOS_ASM("TST LR, #0x10");                 \
    GOS_ASM("IT EQ");                         \
    GOS_ASM("VSTMDBEQ R0!, {S16-S31}");       \
}                                             \
)
#define GOS_PORT_RESTORE_FP_CONTEXT() (       \
{                                             \
    GOS_ASM("TST LR, #0x10");                 \
    GOS_ASM("IT EQ");                         \
    GOS_ASM("VLDMIAEQ R0!, {S16-S31}");       \
}                                             \
)
#else
#define GOS_PORT_SAVE_FP_CONTEXT()        GOS_ASM("NOP")
#define GOS_PORT_RESTORE_FP_CONTEXT()     GOS_ASM("NOP")
#endif

#define gos_ported_doContextSwitch() (                                      \
{                                                                           \
    /* Keep MSP 8-byte aligned before calling C helpers. */                \
    GOS_ASM("PUSH {R3, LR}");                                               \
                                                                            \
    /* Save current task context; LR is its incoming EXC_RETURN. */        \
    GOS_ASM("MRS R0, PSP");                                                 \
    GOS_PORT_SAVE_FP_CONTEXT();                                             \
    GOS_ASM("SUB R0, R0, #4");                                              \
    GOS_ASM("STMDB R0!, {R4-R11, LR}");                                     \
    GOS_ASM("BL gos_kernelSaveCurrentPsp");                                 \
                                                                            \
    GOS_ASM("BL gos_kernelSelectNextTask");                                 \
    GOS_ASM("BL gos_kernelGetCurrentPsp");                                  \
                                                                            \
    /* LR now becomes the selected task's saved EXC_RETURN. */             \
    GOS_ASM("LDMIA R0!, {R4-R11, LR}");                                     \
    GOS_ASM("ADD R0, R0, #4");                                              \
    GOS_PORT_RESTORE_FP_CONTEXT();                                          \
    GOS_ASM("MSR PSP, R0");                                                 \
                                                                            \
    /* Discard incoming EXC_RETURN; do not overwrite selected-task LR. */  \
    GOS_ASM("ADD SP, SP, #8");                                              \
    GOS_ASM("BX LR");                                                       \
}                                                                           \
)

/**
 * SVC handler function name.
 */
#define gos_ported_svcHandler GOS_NAKED SVC_Handler

/*
 * SVC #255 is handled directly in gos_ported_svcHandler().
 * Do not branch from naked SVC assembly to a normal C dispatcher:
 * LR contains EXC_RETURN and must remain untouched until BX LR.
 */

/**
 * SVC handler function.
 */
#define gos_ported_handleSVC() (                \
{                                               \
    /* Check LR to know which stack is used. */ \
    GOS_ASM("TST LR, #4");                      \
    /* 2 next instructions are conditional. */  \
    GOS_ASM("ITE EQ");                          \
    /* Save MSP if bit 2 is 0. */               \
    GOS_ASM("MRSEQ R0, MSP");                   \
    /* Save PSP if bit 2 is 1. */               \
    GOS_ASM("MRSNE R0, PSP");                   \
                                                \
    /* Pass R0 as the argument. */              \
    GOS_ASM("B gos_ported_svcHandlerMain");     \
}                                               \
)

/**
 * SVC handler main function.
 */
#define gos_ported_handleSVCMain(sp) (                     \
{                                                          \
    /* Get the address of the instruction saved in PC. */  \
    u8_t* pInstruction = (u8_t*)(sp[6]);                   \
                                                           \
    /* Go back 2 bytes (16-bit opcode). */                 \
    pInstruction -= 2;                                     \
                                                           \
    /* Get the opcode, in little-endian. */                \
    u8_t svcNum = *pInstruction;                           \
                                                           \
    switch (svcNum)                                        \
    {                                                      \
        case 0xFF:                                         \
            /* Trigger PendSV. */                          \
            ICSR |= (1 << 28);                             \
            break;                                         \
        default: break;                                    \
    }                                                      \
}                                                          \
)

/**
 * PendSV handler function.
 */
#define gos_ported_handlePendSV() (                    \
{                                                      \
    /* Keep MSP 8-byte aligned while special-case C code runs. */ \
    GOS_ASM("PUSH {R3, LR}");                           \
    GOS_ASM("BL gos_kernelPendSvHandleSpecialCases");  \
    GOS_ASM("CMP R0, #0");                             \
    GOS_ASM("BEQ 1f");                                 \
                                                       \
    /* Special case: return with the original EXC_RETURN. */ \
    GOS_ASM("POP {R3, LR}");                           \
    GOS_ASM("BX LR");                                  \
                                                       \
    GOS_ASM("1:");                                     \
    GOS_ASM("POP {R3, LR}");                           \
    gos_ported_doContextSwitch();                      \
}                                                      \
)

/**
 * System tick handler function name.
 */
#define gos_ported_sysTickInterrupt SysTick_Handler

/**
 * Fault handler enable function.
 */
#define gos_ported_enableFaultHandlers() (  \
{                                           \
    SHCSR |= (1 << 16); /* Memory Fault */  \
    SHCSR |= (1 << 17); /* Bus Fault    */  \
    SHCSR |= (1 << 18); /* Usage Fault  */  \
    GOS_ASM("dsb" ::: "memory");            \
    GOS_ASM("isb");                         \
}                                           \
)

#if defined(__VFP_FP__) && !defined(__SOFTFP__)
/**
 * Disable lazy FPU stacking so EXC_RETURN and the saved FP frame agree.
 */
#define gos_ported_configureFpuContext() (                 \
{                                                          \
    *(volatile u32_t*)0xE000EF34u &= ~(1u << 30);          \
    GOS_ASM("dsb" ::: "memory");                           \
    GOS_ASM("isb");                                        \
}                                                          \
)
#else
#define gos_ported_configureFpuContext() do { } while (0)
#endif

/**
 * Kernel start initialization function.
 */
#define gos_ported_kernelStartInit() (                            \
{                                                                 \
    GOS_ASM("BL gos_kernelGetCurrentPsp");                        \
    /* The first task is entered by a C call, not exception return. */ \
    GOS_ASM("ADD R0, R0, #72"); /* 18 words: SW frame, pad, HW frame. */ \
    GOS_ASM("MSR PSP, R0");                                      \
                                                                  \
    /* Change to use PSP. */                                      \
    GOS_ASM("MRS R0, CONTROL");                                   \
    GOS_ASM("ORR R0, R0, #2"); /* set bit[1] SPSEL */             \
    GOS_ASM("MSR CONTROL, R0");                                   \
    GOS_ASM("isb");                                               \
                                                                  \
    /* Move to unprivileged level. */                             \
    GOS_ASM("MRS R0, CONTROL");                                   \
    GOS_ASM("ORR R0, R0, #1");                                    \
    GOS_ASM("MSR CONTROL, R0");                                   \
    GOS_ASM("isb");                                               \
}                                                                 \
)

#endif
#endif
