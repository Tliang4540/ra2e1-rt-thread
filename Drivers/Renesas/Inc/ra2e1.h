#ifndef _RA2E1_H_
#define _RA2E1_H_

typedef enum
{
    Reset_IRQn              = -15,
    NonMaskableInt_IRQn     = -14,
    HardFault_IRQn          = -13,
    SVCall_IRQn             = -5,
    PendSV_IRQn             = -2,
    SysTick_IRQn            = -1
}IRQn_Type;

#include "bsp_elc.h"
#include "bsp_icu.h"
#include "renesas.h"
#include "bsp_io.h"
#include "bsp_clocks.h"
#include "bsp_sci_uart.h"
#include "bsp_register_protection.h"

void      R_BSP_RegisterProtectEnable(bsp_reg_protect_t regs_to_protect);
void      R_BSP_RegisterProtectDisable(bsp_reg_protect_t regs_to_unprotect);

#define OFS0_REV        0xA001A001
#define OFS0_IWDTSTR    (0x01 << 1)    //disable IWDT after reset
#define OFS0_IWDTOPS    (0x03 << 2)    //timeout period select 2048 cycles
#define OFS0_IWDTCKS    (0x0f << 4)    //IWDT-dedicated clock frequency division ratio select 1/128
#define OFS0_IWDTRP     (0x0f << 8)    //position select end=0% start=100%
#define OFS0_IWDTSRI    (0x01 << 12)   //enable reset
#define OFS0_IWDTCTL    (0x01 << 14)   //stop counting when in sleep mode
#define OFS0_IWDT       OFS0_IWDTSTR | OFS0_IWDTOPS | OFS0_IWDTCKS | OFS0_IWDTRP | OFS0_IWDTSRI | OFS0_IWDTCTL

#define OFS0_WDTSTR     (0x01 << 17)   //stop WDT after a reset(register start mode)
#define OFS0_WDTOPS     (0x03 << 18)   //timeout period select 16384 cycles
#define OFS0_WDTCKS     (0x0f << 20)   //WDT clock frequency division ratio select 1/128
#define OFS0_WDTRP      (0x0f << 24)   //position select end=0% start=100%
#define OFS0_WDTSRI     (0x01 << 28)   //enable reset
#define OFS0_WDTCTL     (0x01 << 30)   //stop counting when in sleep mode
#define OFS0_WDT        OFS0_WDTSTR | OFS0_WDTOPS | OFS0_WDTCKS | OFS0_WDTRP | OFS0_WDTSRI | OFS0_WDTCTL

#define ROM_REG_OFS0    OFS0_REV | OFS0_IWDT | OFS0_WDT

#define OFS1_REV        0x7fff8ec3
#define OFS1_LVDAS      (0x01 << 2)     //disable voltage monitor 0 reset after a reset
#define OFS1_VDSEL0     (0x03 << 3)     //voltage detection 0 level select Vdet0_3
#define OFS1_HOCOEN     (0x01 << 8)     //enable HOCO oscillation after areset
#define OFS1_HOCOFRQ    (0x04 << 12)    //HOCO Frequency setting 48Mhz
#define OFS1_ICSATS     (0x01 << 31)    //internal clock supply architecture type A

#define ROM_REG_OFS1    OFS1_REV | OFS1_LVDAS | OFS1_VDSEL0 | OFS1_HOCOEN | OFS1_HOCOFRQ | OFS1_ICSATS

#define ROM_REG_MPU_PC0_START   (0x000ffffc)
#define ROM_REG_MPU_PC0_END     (0x000fffff)
#define ROM_REG_MPU_PC1_START   (0x000ffffc)
#define ROM_REG_MPU_PC1_END     (0x000fffff)
#define ROM_REG_MPU_RG0_START   (0x000ffffc)
#define ROM_REG_MPU_RG0_END     (0x000fffff)
#define ROM_REG_MPU_RG1_START   (0x200ffffc)
#define ROM_REG_MPU_RG1_END     (0x200fffff)
#define ROM_REG_MPU_RG2_START   (0x407ffffc)
#define ROM_REG_MPU_RG2_END     (0x407fffff)
#define ROM_REG_MPU_RG3_START   (0x400dfffc)
#define ROM_REG_MPU_RG3_END     (0x400dffff)

#define ROM_REG_MPU_CONTROL     (0xffffffff)

#endif