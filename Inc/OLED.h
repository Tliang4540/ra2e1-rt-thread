#ifndef _OLED_H_
#define _OLED_H_

#include "ra2e1.h"

#define        OLED_DC            R_PORT2
#define        OLED_DC_Pin        BSP_IO_PIN_06

#define        Set_DC             bsp_set_pin(OLED_DC, OLED_DC_Pin)
#define        Clr_DC             bsp_reset_pin(OLED_DC, OLED_DC_Pin)

#define        OLED_CS            R_PORT2
#define        OLED_CS_Pin        BSP_IO_PIN_01

#define        Set_CS             bsp_set_pin(OLED_CS, OLED_CS_Pin)
#define        Clr_CS             bsp_reset_pin(OLED_CS, OLED_CS_Pin)

#define        OLED_CLK           R_PORT3
#define        OLED_CLK_Pin       BSP_IO_PIN_00

#define        Set_CLK            bsp_set_pin(OLED_CLK, OLED_CLK_Pin)
#define        Clr_CLK            bsp_reset_pin(OLED_CLK, OLED_CLK_Pin)

#define        OLED_DIN           R_PORT3
#define        OLED_DIN_Pin       BSP_IO_PIN_02

#define        Set_DIN            bsp_set_pin(OLED_DIN, OLED_DIN_Pin)
#define        Clr_DIN            bsp_reset_pin(OLED_DIN, OLED_DIN_Pin)

extern void OLED_Init(void);
extern void Set_ShowAddr(uint8_t x, uint8_t y);
extern void Cls(void);
extern void Show_Bmp(uint8_t *bmp, uint8_t sx, uint8_t sy, uint8_t x, uint8_t y);
extern void Display(char *str, uint8_t x, uint8_t y);
extern void ShowBrand(void);
extern void OLED_Switch(uint8_t s);
extern void ShowPower(uint8_t vol, uint8_t x, uint8_t y);

#endif
