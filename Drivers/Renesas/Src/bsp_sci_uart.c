#include "bsp_sci_uart.h"

#define BAUD	115200.0
#define PCLK    24000000.0
#define BRR_V   (PCLK / 16.0 / BAUD)
#define MDDR_V  ((256.0/((float)BRR_V / ((unsigned int)BRR_V)))+0.5)

void bsp_sci_uart_init( R_SCI0_Type *R_SCIx, 
                        USART_DataLength_enum length, 
                        USART_StopBits_enum stopBits, 
                        USART_Parity_enum parity,
                        USART_Mode_enum mode)
{
    BSP_SCI0_POWER(BSP_POWER_ON);
    
    R_SCIx->SCR = 0x00;         //close interrupt and disable receive transmit
    R_SCIx->SSR = 0x00;
    R_SCIx->FCR = 0x0000;       //close FIFO
    R_SCIx->SIMR1 = 0x00;       //close IIC
    R_SCIx->SIMR2 = 0x00;
    R_SCIx->SIMR3 = 0x00;
    R_SCIx->CDR   = 0x00;
    R_SCIx->SPMR  = 0x00;       //close SPI
    R_SCIx->SPTR  = 0x06;
    
    R_SCIx->SMR   = stopBits | parity;  //set stop bits and parity
    R_SCIx->SCMR  = 0xE2 | length;      //set data length
    R_SCIx->SEMR  = 0x34;               //bit7.RXDESEL:0 low level. 1 falling edge
                                        //bit6.BGDM:1 double speed
    //N = (float)(PCLKB / 16 / baud)
    //M = (float)N / (int)N
    //MDDR = 256/M
    //BRR = N
    R_SCIx->BRR  = (uint8_t)BRR_V - 1;
    R_SCIx->MDDR = ((uint8_t)MDDR_V) == 256 ? 255 : (uint8_t)MDDR_V;

    R_SCIx->SCR = mode;
}

void bsp_sci_irq_enable(R_SCI0_Type *R_SCIx, USART_IRQ_enum irq)
{
    R_SCIx->SCR |= (uint8_t)irq;
}

void bsp_sci_irq_disable(R_SCI0_Type *R_SCIx, USART_IRQ_enum irq)
{
    R_SCIx->SCR &= ~(uint8_t)irq;
}
