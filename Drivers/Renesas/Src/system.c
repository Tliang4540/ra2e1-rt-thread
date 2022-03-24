/***********************************************************************************************************************
 * Copyright [2020-2021] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include <string.h>
#include <stdint.h>
#include "ra2e1.h"
/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Mask to select CP bits( 0xF00000 ) */
#define CP_MASK                                       (0xFU << 20)

/* Value to write to OAD register of MPU stack monitor to enable NMI when a stack overflow is detected. */
#define BSP_STACK_POINTER_MONITOR_NMI_ON_DETECTION    (0xA500U)

/* Key code for writing PRCR register. */
#define BSP_PRV_PRCR_KEY                              (0xA500U)
#define BSP_PRV_PRCR_PRC1_UNLOCK                      ((BSP_PRV_PRCR_KEY) | 0x2U)
#define BSP_PRV_PRCR_LOCK                             ((BSP_PRV_PRCR_KEY) | 0x0U)

#define BSP_PRV_STACK_LIMIT                          ((uint32_t) &__StackLimit)
#define BSP_PRV_STACK_TOP                            ((uint32_t) &__StackTop)

#define BSP_TZ_STACK_SEAL_VALUE                       (0xFEF5EDA5)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/** System Clock Frequency (Core Clock) */
uint32_t SystemCoreClock = 0U;


/* Generated by linker. */
extern uint32_t __etext;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __StackLimit;
extern uint32_t __StackTop;

/* Initialize static constructors */
extern void (* __init_array_start[])(void);
extern void (* __init_array_end[])(void);
extern void * __Vectors[];
extern void R_BSP_SecurityInit(void);

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
//真随机数发生器初始化,不初始化会有额外功耗
static void bsp_reset_trng_circuit(void);

/*******************************************************************************************************************//**
 * Initialize the MCU and the runtime environment.
 **********************************************************************************************************************/
void SystemInit (void)
{
    /* VTOR is in undefined state out of RESET:
     * https://developer.arm.com/documentation/100235/0004/the-cortex-m33-peripherals/system-control-block/system-control-block-registers-summary?lang=en.
     * Set the Secure/Non-Secure VTOR to the vector table address based on the build. This is skipped for non-secure
     * projects because SCB_NS->VTOR is set by the secure project before the non-secure project runs. */
    SCB->VTOR = (uint32_t) &__Vectors;

    /* Configure system clocks. */
    bsp_clock_init();

    /* To prevent an undesired current draw, this MCU requires a reset
     * of the TRNG circuit after the clocks are initialized */
    bsp_reset_trng_circuit();

    /* Disable MSP monitoring  */
    R_MPU_SPMON->SP[0].CTL = 0;

    /* Setup NMI interrupt  */
    R_MPU_SPMON->SP[0].OAD = BSP_STACK_POINTER_MONITOR_NMI_ON_DETECTION;

    /* Setup start address  */
    R_MPU_SPMON->SP[0].SA = BSP_PRV_STACK_LIMIT;

    /* Setup end address  */
    R_MPU_SPMON->SP[0].EA = BSP_PRV_STACK_TOP;

    /* Set SPEEN bit to enable NMI on stack monitor exception. NMIER bits cannot be cleared after reset, so no need
     * to read-modify-write. */
    R_ICU->NMIER = R_ICU_NMIER_SPEEN_Msk;

    /* Enable MSP monitoring  */
    R_MPU_SPMON->SP[0].CTL = 1U;

    /* Initialize C runtime environment. */
    /* Zero out BSS */
    memset(&__bss_start__, 0U, ((uint32_t) &__bss_end__ - (uint32_t) &__bss_start__));

    /* Copy initialized RAM data from ROM to RAM. */
    memcpy(&__data_start__, &__etext, ((uint32_t) &__data_end__ - (uint32_t) &__data_start__));

    /* Initialize static constructors */
    int32_t count = __init_array_end - __init_array_start;
    for (int32_t i = 0; i < count; i++)
    {
        __init_array_start[i]();
    }

    /* Initialize SystemCoreClock variable. */
    SystemCoreClockUpdate();

    /* Initialize ELC events that will be used to trigger NVIC interrupts. */
    bsp_irq_cfg();
}

/*******************************************************************************************************************//**
 * Disable TRNG circuit to prevent unnecessary current draw which may otherwise occur when the Crypto module
 * is not in use.
 **********************************************************************************************************************/
static void bsp_reset_trng_circuit (void)
{
    volatile uint8_t read_port = 0U;
    uint32_t old_mask_level = 0U; 
    /* Release register protection for low power modes (per RA2A1 User's Manual (R01UH0888EJ0100) Figure 11.13 "Example
     * of initial setting flow for an unused circuit") */
    (void)((read_port));
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_OM_LPC_BATT);

    /* Enable TRNG function (disable stop function) */
    old_mask_level = __get_PRIMASK(); 
    __set_PRIMASK(1); 
    R_MSTP->MSTPCRC &= ~(1U << 28U);; 
    R_MSTP->MSTPCRC; 
    __set_PRIMASK(old_mask_level);

    /* Wait for at least 3 PCLKB cycles */
    read_port = R_PFS->PORT[0].PIN[0].PmnPFS_b.PODR;
    read_port = R_PFS->PORT[0].PIN[0].PmnPFS_b.PODR;
    read_port = R_PFS->PORT[0].PIN[0].PmnPFS_b.PODR;

    /* Disable TRNG function */
    old_mask_level = __get_PRIMASK(); 
    __set_PRIMASK(1); 
    R_MSTP->MSTPCRC |= (1U << 28U);; 
    R_MSTP->MSTPCRC; 
    __set_PRIMASK(old_mask_level);

    /* Reapply register protection for low power modes (per RA2A1 User's Manual (R01UH0888EJ0100) Figure 11.13 "Example
     * of initial setting flow for an unused circuit") */
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_OM_LPC_BATT);
}

/** @} (end addtogroup BSP_MCU) */