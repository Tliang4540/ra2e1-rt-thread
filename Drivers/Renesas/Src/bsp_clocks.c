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

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "bsp_clocks.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Key code for writing PRCR register. */
#define BSP_PRV_PRCR_KEY                        (0xA500U)
#define BSP_PRV_PRCR_UNLOCK                     ((BSP_PRV_PRCR_KEY) | 0x3U)
#define BSP_PRV_PRCR_LOCK                       ((BSP_PRV_PRCR_KEY) | 0x0U)

#define BSP_PRV_MAXIMUM_HOCOWTR_HSTS            ((uint8_t) 0x6U)

/* Wait state definitions for MEMWAIT. */
#define BSP_PRV_MEMWAIT_ZERO_WAIT_CYCLES        (0U)
#define BSP_PRV_MEMWAIT_TWO_WAIT_CYCLES         (1U)
#define BSP_PRV_MEMWAIT_MAX_ZERO_WAIT_FREQ      (32000000U)

/* Wait state definitions for FLDWAITR. */
#define BSP_PRV_FLDWAITR_ONE_WAIT_CYCLES        (0U)
#define BSP_PRV_FLDWAITR_TWO_WAIT_CYCLES        (1U)
#define BSP_PRV_FLDWAITR_MAX_ONE_WAIT_FREQ      (32000000U)

/* Temporary solution until R_FACI is added to renesas.h. */
#define BSP_PRV_FLDWAITR_REG_ACCESS             (*((volatile uint8_t *) (0x407EFFC4U)))

/* The number of clocks is used to size the g_clock_freq array. */
#define BSP_PRV_NUM_CLOCKS                     ((uint8_t) BSP_CLOCKS_SOURCE_CLOCK_SUBCLOCK + 1U)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
static uint8_t bsp_clock_set_prechange(uint32_t requested_freq_hz);
static void    bsp_clock_set_postchange(uint32_t updated_freq_hz, uint8_t new_rom_wait_state);
static void bsp_prv_operating_mode_opccr_set(uint8_t operating_mode);
static void bsp_prv_clock_set_hard_reset(void);

/* This array stores the clock frequency of each system clock. This section of RAM should not be initialized by the C
 * runtime environment. This is initialized and used in bsp_clock_init, which is called before the C runtime
 * environment is initialized. */
static uint32_t g_clock_freq[BSP_PRV_NUM_CLOCKS]  __attribute__((section(".noinit"))) __attribute__((__used__));

/*******************************************************************************************************************//**
 * @internal
 * @addtogroup BSP_MCU_PRV Internal BSP Documentation
 * @ingroup RENESAS_INTERNAL
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Changes the operating speed in OPCCR.  Assumes the LPM registers are unlocked in PRCR and cache is off.
 *
 * @param[in]  operating_mode  Desired operating mode, must be one of the BSP_PRV_OPERATING_MODE_* macros, cannot be
 *                             BSP_PRV_OPERATING_MODE_SUBOSC_SPEED
 **********************************************************************************************************************/
static void bsp_prv_operating_mode_opccr_set (uint8_t operating_mode)
{
    /* If the desired operating mode is already set, return. */
    if (operating_mode == R_SYSTEM->OPCCR)
    {
        return;
    }

    /* On some MCUs, the HOCO must be stable before updating OPCCR.OPCM. */
    if (0U == R_SYSTEM->HOCOCR)
    {
        /* Wait for HOCO to stabilize. */
        while(R_SYSTEM->OSCSF_b.HOCOSF != 1U);
    }

    /* Wait for transition to complete. */
    while(R_SYSTEM->OPCCR_b.OPCMTSF != 0U);

    /* Apply requested operating speed mode. */
    R_SYSTEM->OPCCR = operating_mode;

    /* Wait for transition to complete. */
    while(R_SYSTEM->OPCCR_b.OPCMTSF != 0U);
}

/***********************************************************************************************************************
 * Changes the operating speed mode.  Assumes the LPM registers are unlocked in PRCR and cache is off.
 *
 * @param[in]  operating_mode  Desired operating mode, must be one of the BSP_PRV_OPERATING_MODE_* macros
 **********************************************************************************************************************/
void bsp_prv_operating_mode_set (uint8_t operating_mode)
{
    if (BSP_PRV_OPERATING_MODE_SUBOSC_SPEED == operating_mode)
    {
        /* Wait for transition to complete. */
        while(R_SYSTEM->SOPCCR_b.SOPCMTSF != 0U);

        /* Set subosc speed mode. */
        R_SYSTEM->SOPCCR = 0x1U;

        /* Wait for transition to complete. */
        while(R_SYSTEM->SOPCCR_b.SOPCMTSF != 0U);
    }
    else
    {
        /* Wait for transition to complete. */
        while(R_SYSTEM->SOPCCR_b.SOPCMTSF != 0U);

        /* Exit subosc speed mode first. */
        R_SYSTEM->SOPCCR = 0U;

        /* Wait for transition to complete. Check the entire register here since it should be set to 0 at this point.
         * Checking the entire register is slightly more efficient. This will also hang the program if the LPM
         * registers are not unlocked, which can help catch programming errors. */
        while(R_SYSTEM->SOPCCR != 0U);

        bsp_prv_operating_mode_opccr_set(operating_mode);
    }
}

/*******************************************************************************************************************//**
 * Update SystemCoreClock variable based on current clock settings.
 **********************************************************************************************************************/
void SystemCoreClockUpdate (void)
{
    uint32_t clock_index = R_SYSTEM->SCKSCR;
    SystemCoreClock = g_clock_freq[clock_index] >> R_SYSTEM->SCKDIVCR_b.ICK;
}

/*******************************************************************************************************************//**
 * Applies system core clock source and divider changes.  The MCU is expected to be in high speed mode during this
 * configuration and the CGC registers are expected to be unlocked in PRCR.
 *
 * @param[in] clock                    Desired system clock
 * @param[in] sckdivcr                 Value to set in SCKDIVCR register
 **********************************************************************************************************************/
void bsp_prv_clock_set (uint32_t clock, uint32_t sckdivcr)
{
    uint32_t iclk_div = (sckdivcr >> 24) & 7;

    /* Adjust the MCU specific wait state right before the system clock is set, if the system clock frequency to be
     * set is higher than before. */
    uint32_t clock_freq_hz_post_change = g_clock_freq[clock] >> iclk_div;
    uint8_t  new_rom_wait_state        = bsp_clock_set_prechange(clock_freq_hz_post_change);

    /* In order to avoid a system clock (momentarily) higher than expected, the order of switching the clock and
     * dividers must be so that the frequency of the clock goes lower, instead of higher, before being correct. */

    /* If the current ICLK divider is less (higher frequency) than the requested ICLK divider, set the divider
     * first. */
    sckdivcr = sckdivcr & BSP_PRV_SCKDIVCR_MASK;
    if (R_SYSTEM->SCKDIVCR_b.ICK < iclk_div)
    {
        /* Set the system dividers */
        R_SYSTEM->SCKDIVCR = sckdivcr;

        /* Set the system source clock */
        R_SYSTEM->SCKSCR = (uint8_t) clock;
    }
    /* If the current ICLK divider is greater (lower frequency) than the requested ICLK divider, set the clock
     * source first. If the ICLK divider is the same, order does not matter. */
    else
    {
        /* Set the system source clock */
        R_SYSTEM->SCKSCR = (uint8_t) clock;

        /* Set the system dividers */
        R_SYSTEM->SCKDIVCR = sckdivcr;
    }

    /* Clock is now at requested frequency. */

    /* Update the CMSIS core clock variable so that it reflects the new ICLK frequency. */
    SystemCoreClock = clock_freq_hz_post_change;

    /* Adjust the MCU specific wait state soon after the system clock is set, if the system clock frequency to be
     * set is lower than previous. */
    bsp_clock_set_postchange(SystemCoreClock, new_rom_wait_state);
}

static void bsp_prv_clock_set_hard_reset (void)
{
    /* The MCU must be in high speed mode to set wait states to 2. High speed mode is the default out of reset. */
    R_SYSTEM->MEMWAIT = BSP_PRV_MEMWAIT_TWO_WAIT_CYCLES;

    /* The MCU must be in high speed mode to set wait states to 2. High speed mode is the default out of reset. */
    BSP_PRV_FLDWAITR_REG_ACCESS = BSP_PRV_FLDWAITR_TWO_WAIT_CYCLES;

    /* In order to avoid a system clock (momentarily) higher than expected, the order of switching the clock and
     * dividers must be so that the frequency of the clock goes lower, instead of higher, before being correct. */

    /* ICLK divider at reset is lowest possible, so set dividers first. */

    /* Set the system source clock */
    R_SYSTEM->SCKSCR = BSP_CFG_CLOCK_SOURCE;

    /* Set the system dividers after setting the system clock source if ICLK divisor is smaller than reset value. */
    R_SYSTEM->SCKDIVCR = (BSP_CFG_PCLKD_DIV << 24) | (BSP_CFG_PCLKB_DIV << 8) | BSP_CFG_ICLK_DIV;
    
    /* Clock is now at requested frequency. */

    /* Update the CMSIS core clock variable so that it reflects the new ICLK frequency. */
    SystemCoreClockUpdate();

    /* ROM wait states are 0 by default.  No change required here. */
}

/*******************************************************************************************************************//**
 * Initializes variable to store system clock frequencies.
 **********************************************************************************************************************/
static void bsp_clock_freq_var_init (void)
{
    g_clock_freq[BSP_CLOCKS_SOURCE_CLOCK_HOCO] = BSP_CFG_HOCO_HZ;
    g_clock_freq[BSP_CLOCKS_SOURCE_CLOCK_MOCO] = BSP_MOCO_FREQ_HZ;
    g_clock_freq[BSP_CLOCKS_SOURCE_CLOCK_LOCO] = BSP_LOCO_FREQ_HZ;
    g_clock_freq[BSP_CLOCKS_SOURCE_CLOCK_MAIN_OSC] = BSP_CFG_XTAL_HZ;
    g_clock_freq[BSP_CLOCKS_SOURCE_CLOCK_SUBCLOCK] = BSP_SUBCLOCK_FREQ_HZ;

    /* The SystemCoreClock needs to be updated before calling R_BSP_SoftwareDelay. */
    SystemCoreClockUpdate();
}

/*******************************************************************************************************************//**
 * Initializes system clocks.  Makes no assumptions about current register settings.
 **********************************************************************************************************************/
void bsp_clock_init (void)
{
    /* Unlock CGC and LPM protection registers. */
    R_SYSTEM->PRCR = (uint16_t) BSP_PRV_PRCR_UNLOCK;

    /* Disable the flash prefetch buffer. */
    R_FACI_LP->PFBER = 0;

    bsp_clock_freq_var_init();

    /* Configure main oscillator drive. */
    R_SYSTEM->MOMCR = 0;

    /* Set the main oscillator wait time. */
    R_SYSTEM->MOSCWTCR = (uint8_t) 9;

    /* If the board has a subclock, set the subclock drive and start the subclock if the subclock is stopped.  If the
     * subclock is running, the subclock drive is assumed to be set appropriately. */
    if (R_SYSTEM->SOSCCR)
    {
        /* Configure the subclock drive if the subclock is not already running. */
        R_SYSTEM->SOMCR  = 0;
        R_SYSTEM->SOSCCR = 0U;
    }

    /* Some MCUs do not start in high speed mode. */
    bsp_prv_operating_mode_opccr_set(BSP_PRV_OPERATING_MODE_HIGH_SPEED);

    /* Start source clock. */
    R_SYSTEM->HOCOCR = 0U;

    /* Wait for HOCO to stabilize. */
    while(R_SYSTEM->OSCSF_b.HOCOSF != 1U);

    bsp_prv_clock_set_hard_reset();

    /* Lock CGC and LPM protection registers. */
    R_SYSTEM->PRCR = (uint16_t) BSP_PRV_PRCR_LOCK;

    R_FACI_LP->PFBER = 1;
}

/*******************************************************************************************************************//**
 * Increases the ROM and RAM wait state settings to the minimum required based on the requested clock change.
 *
 * @param[in] requested_freq_hz        New core clock frequency after the clock change.
 *
 * @return The wait states for FLWT required after the clock change (or 0 if FLWT does not exist).
 **********************************************************************************************************************/
static uint8_t bsp_clock_set_prechange (uint32_t requested_freq_hz)
{
    uint8_t new_rom_wait_state = 0U;

    (void)((requested_freq_hz));

    if (requested_freq_hz > BSP_PRV_MEMWAIT_MAX_ZERO_WAIT_FREQ)
    {
        /* The MCU must be in high speed mode to set wait states to 2. The MCU should already be in high speed mode as
         * a precondition to bsp_prv_clock_set. */
        R_SYSTEM->MEMWAIT = BSP_PRV_MEMWAIT_TWO_WAIT_CYCLES;
    }

    if (requested_freq_hz > BSP_PRV_FLDWAITR_MAX_ONE_WAIT_FREQ)
    {
        /* The MCU must be in high speed mode to set wait states to 2. The MCU should already be in high speed mode as
         * a precondition to bsp_prv_clock_set. */
        BSP_PRV_FLDWAITR_REG_ACCESS = BSP_PRV_FLDWAITR_TWO_WAIT_CYCLES;
    }

    return new_rom_wait_state;
}

/*******************************************************************************************************************//**
 * Decreases the ROM and RAM wait state settings to the minimum supported based on the applied clock change.
 *
 * @param[in] updated_freq_hz          New clock frequency after clock change
 * @param[in] new_rom_wait_state       Optimal value for FLWT if it exists, 0 if FLWT does not exist on the MCU
 **********************************************************************************************************************/
static void bsp_clock_set_postchange (uint32_t updated_freq_hz, uint8_t new_rom_wait_state)
{
    /* These variables are unused for some MCUs. */
    (void)(new_rom_wait_state);
    (void)(updated_freq_hz);

    if (updated_freq_hz <= BSP_PRV_MEMWAIT_MAX_ZERO_WAIT_FREQ)
    {
        R_SYSTEM->MEMWAIT = BSP_PRV_MEMWAIT_ZERO_WAIT_CYCLES;
    }

    if (updated_freq_hz <= BSP_PRV_FLDWAITR_MAX_ONE_WAIT_FREQ)
    {
        BSP_PRV_FLDWAITR_REG_ACCESS = BSP_PRV_FLDWAITR_ONE_WAIT_CYCLES;
    }
}

/** @} (end addtogroup BSP_MCU_PRV) */
