#ifndef _BSP_SCI_UART_H_
#define _BSP_SCI_UART_H_

#include "ra2e1.h"

#define BSP_POWER_OFF   1
#define BSP_POWER_ON    0

#define BSP_DTC_POWER(sw)   (R_SYSTEM->MSTPCRA_b.MSTPA22 = sw)

#define BSP_IIC_POWER(sw)   (R_MSTP->MSTPCRB_b.MSTPB9 = sw)
#define BSP_SPI_POWER(sw)   (R_MSTP->MSTPCRB_b.MSTPB19 = sw)
#define BSP_SCI9_POWER(sw)   (R_MSTP->MSTPCRB_b.MSTPB22 = sw)
#define BSP_SCI2_POWER(sw)   (R_MSTP->MSTPCRB_b.MSTPB29 = sw)
#define BSP_SCI1_POWER(sw)   (R_MSTP->MSTPCRB_b.MSTPB30 = sw)
#define BSP_SCI0_POWER(sw)   (R_MSTP->MSTPCRB_b.MSTPB31 = sw)

#define BSP_CAC_POWER(sw)   (R_MSTP->MSTPCRC_b.MSTPC0 = sw)
#define BSP_CRC_POWER(sw)   (R_MSTP->MSTPCRC_b.MSTPC1 = sw)
#define BSP_CTSU_POWER(sw)   (R_MSTP->MSTPCRC_b.MSTPC3 = sw)
#define BSP_DOC_POWER(sw)   (R_MSTP->MSTPCRC_b.MSTPC13 = sw)
#define BSP_ELC_POWER(sw)   (R_MSTP->MSTPCRC_b.MSTPC14 = sw)
#define BSP_TRNG_POWER(sw)   (R_MSTP->MSTPCRC_b.MSTPC28 = sw)
#define BSP_AES_POWER(sw)   (R_MSTP->MSTPCRC_b.MSTPC31 = sw)

#define BSP_AGT1_POWER(sw)   (R_MSTP->MSTPCRD_b.MSTPD2 = sw)
#define BSP_AGT0_POWER(sw)   (R_MSTP->MSTPCRD_b.MSTPD3 = sw)
#define BSP_GPT32_POWER(sw)   (R_MSTP->MSTPCRD_b.MSTPD5 = sw)
#define BSP_GPT16_POWER(sw)   (R_MSTP->MSTPCRD_b.MSTPD6 = sw)
#define BSP_POEG_POWER(sw)   (R_MSTP->MSTPCRD_b.MSTPD14 = sw)
#define BSP_ADC_POWER(sw)   (R_MSTP->MSTPCRD_b.MSTPD16 = sw)
#define BSP_ACMPLP_POWER(sw)   (R_MSTP->MSTPCRD_b.MSTPD29 = sw)

typedef enum 
{
    USART_NONE_PARITY = 0x00,
    USART_EVEN_PARITY = 0x20,
    USART_ODD_PARITY  = 0x30
    
}USART_Parity_enum;

typedef enum 
{
    USART_STOPBITS_1 = 0x00,
    USART_STOPBITS_2 = 0x08
}USART_StopBits_enum;

typedef enum 
{
    USART_DATA_LENGTH_9BIT = 0x00,
    USART_DATA_LENGTH_8BIT = 0x10
}USART_DataLength_enum;

typedef enum
{
    USART_MODE_RX = 0x10,
    USART_MODE_TX = 0x20
}USART_Mode_enum;

typedef enum
{
    USART_IRQ_RIE = 0x40,
    USART_IRQ_TIE = 0x80
}USART_IRQ_enum;

typedef enum
{
    USART_TEND = 0x04,
    USART_PER  = 0x08,
    USART_FER  = 0x10,
    USART_ORER = 0x20,
    USART_RDRF = 0x40,
    USART_TDRE = 0x80 
}USART_State_enum;

#define bsp_sci_get_status(sci, state)      (sci->SSR & state)
#define bsp_sci_clear_status(sci, state)           (sci->SSR &= ~state)

extern void bsp_sci_uart_init( R_SCI0_Type *R_SCIx, 
                        USART_DataLength_enum length, 
                        USART_StopBits_enum stopBits, 
                        USART_Parity_enum parity,
                        USART_Mode_enum mode);

extern void bsp_sci_irq_enable(R_SCI0_Type *R_SCIx, USART_IRQ_enum irq);
extern void bsp_sci_irq_disable(R_SCI0_Type *R_SCIx, USART_IRQ_enum irq);

#endif