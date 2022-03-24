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

#ifndef BSP_CLOCKS_H
#define BSP_CLOCKS_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdint.h>
#include "ra2e1.h"

#define BSP_CFG_XTAL_HZ (16000000) /* XTAL 16000000Hz */
#define BSP_CFG_HOCO_HZ (48000000) /* HOCO 48MHz */
#define BSP_CFG_CLOCK_SOURCE (BSP_CLOCKS_SOURCE_CLOCK_HOCO) /* Clock Src: HOCO */
#define BSP_CFG_ICLK_DIV (BSP_CLOCKS_SYS_CLOCK_DIV_1) /* ICLK Div /1 */
#define BSP_CFG_PCLKB_DIV (BSP_CLOCKS_SYS_CLOCK_DIV_2) /* PCLKB Div /2 */
#define BSP_CFG_PCLKD_DIV (BSP_CLOCKS_SYS_CLOCK_DIV_1) /* PCLKD Div /1 */
#define BSP_CFG_CLKOUT_SOURCE (BSP_CLOCKS_CLOCK_DISABLED) /* CLKOUT Disabled */
#define BSP_CFG_CLKOUT_DIV (BSP_CLOCKS_SYS_CLOCK_DIV_1) /* CLKOUT Div /1 */

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* The following definitions are macros instead of enums because the values are used in preprocessor conditionals. */
/* Must match SCKCR.CKSEL values. */
#define BSP_CLOCKS_SOURCE_CLOCK_HOCO        (0) // The high speed on chip oscillator.
#define BSP_CLOCKS_SOURCE_CLOCK_MOCO        (1) // The middle speed on chip oscillator.
#define BSP_CLOCKS_SOURCE_CLOCK_LOCO        (2) // The low speed on chip oscillator.
#define BSP_CLOCKS_SOURCE_CLOCK_MAIN_OSC    (3) // The main oscillator.
#define BSP_CLOCKS_SOURCE_CLOCK_SUBCLOCK    (4) // The subclock oscillator.


/* Frequencies of clocks with fixed freqencies. */
#define BSP_LOCO_FREQ_HZ                       (32768U)   // LOCO frequency is fixed at 32768 Hz
#define BSP_SUBCLOCK_FREQ_HZ                   (32768U)   // Subclock frequency is 32768 Hz
#define BSP_MOCO_FREQ_HZ                       (8000000U) // MOCO frequency is fixed at 8 MHz

#if BSP_CLOCKS_SOURCE_CLOCK_HOCO == BSP_CFG_CLOCK_SOURCE
 #define BSP_STARTUP_SOURCE_CLOCK_HZ           (BSP_CFG_HOCO_HZ)
#elif BSP_CLOCKS_SOURCE_CLOCK_MOCO == BSP_CFG_CLOCK_SOURCE
 #define BSP_STARTUP_SOURCE_CLOCK_HZ           (BSP_MOCO_FREQ_HZ)
#elif BSP_CLOCKS_SOURCE_CLOCK_LOCO == BSP_CFG_CLOCK_SOURCE
 #define BSP_STARTUP_SOURCE_CLOCK_HZ           (BSP_LOCO_FREQ_HZ)
#elif BSP_CLOCKS_SOURCE_CLOCK_SUBCLOCK == BSP_CFG_CLOCK_SOURCE
 #define BSP_STARTUP_SOURCE_CLOCK_HZ           (BSP_SUBCLOCK_FREQ_HZ)
#elif BSP_CLOCKS_SOURCE_CLOCK_MAIN_OSC == BSP_CFG_CLOCK_SOURCE
 #define BSP_STARTUP_SOURCE_CLOCK_HZ           (BSP_CFG_XTAL_HZ)
#endif

/* Startup clock frequency of each system clock. These macros are only helpful if the system clock and dividers have
 * not changed since startup. These macros are not used in FSP modules except for the clock startup code. */
#define BSP_STARTUP_ICLK_HZ                    (BSP_STARTUP_SOURCE_CLOCK_HZ >> BSP_CFG_ICLK_DIV)
#define BSP_STARTUP_PCLKB_HZ                   (BSP_STARTUP_SOURCE_CLOCK_HZ >> BSP_CFG_PCLKB_DIV)
#define BSP_STARTUP_PCLKD_HZ                   (BSP_STARTUP_SOURCE_CLOCK_HZ >> BSP_CFG_PCLKD_DIV)

/* System clock divider options. */
#define BSP_CLOCKS_SYS_CLOCK_DIV_1             (0) // System clock divided by 1.
#define BSP_CLOCKS_SYS_CLOCK_DIV_2             (1) // System clock divided by 2.
#define BSP_CLOCKS_SYS_CLOCK_DIV_4             (2) // System clock divided by 4.
#define BSP_CLOCKS_SYS_CLOCK_DIV_8             (3) // System clock divided by 8.
#define BSP_CLOCKS_SYS_CLOCK_DIV_16            (4) // System clock divided by 16.
#define BSP_CLOCKS_SYS_CLOCK_DIV_32            (5) // System clock divided by 32.
#define BSP_CLOCKS_SYS_CLOCK_DIV_64            (6) // System clock divided by 64.
#define BSP_CLOCKS_SYS_CLOCK_DIV_128           (7) // System clock divided by 128 (available for CLKOUT only).

/* Configuration option used to disable clock output. */
#define BSP_CLOCKS_CLOCK_DISABLED              (0xFFU)

/* HOCO cycles per microsecond. */
#define BSP_PRV_HOCO_CYCLES_PER_US             (BSP_HOCO_HZ / 1000000U)

/* Maximum number of delay cycles required to ensure 1 us passes between setting PLLCCR and clearing PLLCR. */
#if BSP_HOCO_HZ < 48000000U
 #define BSP_PRV_MAX_HOCO_CYCLES_PER_US        (BSP_PRV_HOCO_CYCLES_PER_US)
#else
 #define BSP_PRV_MAX_HOCO_CYCLES_PER_US        (48U)
#endif

/* Create a mask of valid bits in SCKDIVCR. */
#define BSP_PRV_SCKDIVCR_ICLK_MASK             (7U << 24)

#if BSP_FEATURE_CGC_HAS_PCLKD
 #define BSP_PRV_SCKDIVCR_PCLKD_MASK           (7U << 0)
#else
 #define BSP_PRV_SCKDIVCR_PCLKD_MASK           (0U)
#endif

#if BSP_FEATURE_CGC_HAS_PCLKB
 #define BSP_PRV_SCKDIVCR_PCLKB_MASK           (7U << 8)
#else
 #define BSP_PRV_SCKDIVCR_PCLKB_MASK           (0U)
#endif

#define BSP_PRV_SCKDIVCR_MASK     (((BSP_PRV_SCKDIVCR_ICLK_MASK | BSP_PRV_SCKDIVCR_PCLKD_MASK) | BSP_PRV_SCKDIVCR_PCLKB_MASK))

/* Operating power control modes. */
#define BSP_PRV_OPERATING_MODE_HIGH_SPEED      (0U) // Should match OPCCR OPCM high speed
#define BSP_PRV_OPERATING_MODE_MIDDLE_SPEED    (1U) // Should match OPCCR OPCM middle speed
#define BSP_PRV_OPERATING_MODE_LOW_VOLTAGE     (2U) // Should match OPCCR OPCM low voltage
#define BSP_PRV_OPERATING_MODE_LOW_SPEED       (3U) // Should match OPCCR OPCM low speed
#define BSP_PRV_OPERATING_MODE_SUBOSC_SPEED    (4U) // Can be any value not otherwise used

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

/* Public functions defined in bsp.h */
void bsp_clock_init(void);             // Used internally by BSP

/* Used internally by CGC */

void bsp_prv_operating_mode_set(uint8_t operating_mode);
void bsp_prv_prepare_pll(uint32_t pll_freq_hz);
void bsp_prv_clock_set(uint32_t clock, uint32_t sckdivcr);

#endif
