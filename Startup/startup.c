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
#include <stdint.h>
/* Defines function pointers to be used with vector table. */
typedef void (* exc_ptr_t)(void);

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
void    Reset_Handler(void);
void    Default_Handler(void);
int     main(void);
int     entry(void);
void    SystemInit(void);
/*******************************************************************************************************************//**
 * MCU starts executing here out of reset. Main stack pointer is set up already.
 **********************************************************************************************************************/
void Reset_Handler (void)
{
    /* Initialize system using BSP. */
    SystemInit();

    /* Call user application. */
    entry();

    while (1)
    {
        /* Infinite Loop. */
    }
}

/*******************************************************************************************************************//**
 * Default exception handler.
 **********************************************************************************************************************/
void Default_Handler (void)
{
    while (1);
}

/* Main stack */
static uint8_t g_main_stack[0x400] __attribute__((aligned((8)))) \
__attribute__((section(".stack"))) __attribute__((__used__));

/* Heap */
static uint8_t g_heap[0x200]  __attribute__((aligned((8)))) \
__attribute__((section(".heap"))) __attribute__((__used__));

#define WEAK_REF_ATTRIBUTE    __attribute__((weak, alias("Default_Handler")))

void NMI_Handler(void) WEAK_REF_ATTRIBUTE;  
void HardFault_Handler(void) WEAK_REF_ATTRIBUTE;
void MemManage_Handler(void) WEAK_REF_ATTRIBUTE;
void BusFault_Handler(void) WEAK_REF_ATTRIBUTE;
void UsageFault_Handler(void) WEAK_REF_ATTRIBUTE;
void SecureFault_Handler(void) WEAK_REF_ATTRIBUTE;
void SVC_Handler(void) WEAK_REF_ATTRIBUTE;
void DebugMon_Handler(void) WEAK_REF_ATTRIBUTE;
void PendSV_Handler(void) WEAK_REF_ATTRIBUTE;
void SysTick_Handler(void) WEAK_REF_ATTRIBUTE;

/* Vector table. */
const exc_ptr_t __Vectors[16] __attribute__((section(".fixed_vectors"))) __attribute__((__used__)) =
{
    (exc_ptr_t) (&g_main_stack[0] + 0x400), /*      Initial Stack Pointer     */
    Reset_Handler,                                             /*      Reset Handler             */
    NMI_Handler,                                               /*      NMI Handler               */
    HardFault_Handler,                                         /*      Hard Fault Handler        */
    MemManage_Handler,                                         /*      MPU Fault Handler         */
    BusFault_Handler,                                          /*      Bus Fault Handler         */
    UsageFault_Handler,                                        /*      Usage Fault Handler       */
    SecureFault_Handler,                                       /*      Secure Fault Handler      */
    0,                                                         /*      Reserved                  */
    0,                                                         /*      Reserved                  */
    0,                                                         /*      Reserved                  */
    SVC_Handler,                                               /*      SVCall Handler            */
    DebugMon_Handler,                                          /*      Debug Monitor Handler     */
    0,                                                         /*      Reserved                  */
    PendSV_Handler,                                            /*      PendSV Handler            */
    SysTick_Handler,                                           /*      SysTick Handler           */
};

/** @} (end addtogroup BSP_MCU) */
