#include "ra2e1.h"

const uint32_t g_rom_registers[] __attribute__(( section(".rom_registers") )) = {
    (uint32_t)ROM_REG_OFS0,
    (uint32_t)ROM_REG_OFS1,
    (uint32_t)ROM_REG_MPU_PC0_START,
    (uint32_t)ROM_REG_MPU_PC0_END,
    (uint32_t)ROM_REG_MPU_PC1_START,
    (uint32_t)ROM_REG_MPU_PC1_END,
    (uint32_t)ROM_REG_MPU_RG0_START,
    (uint32_t)ROM_REG_MPU_RG0_END,
    (uint32_t)ROM_REG_MPU_RG1_START,
    (uint32_t)ROM_REG_MPU_RG1_END,
    (uint32_t)ROM_REG_MPU_RG2_START,
    (uint32_t)ROM_REG_MPU_RG2_END,
    (uint32_t)ROM_REG_MPU_RG3_START,
    (uint32_t)ROM_REG_MPU_RG3_END,
    (uint32_t)ROM_REG_MPU_CONTROL
};