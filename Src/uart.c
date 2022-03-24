#include "uart.h"

static volatile uint32_t send_flag = 0;

void sci_uart_rxi_isr(void)
{
    bsp_irq_status_clear(ICU_USART_RX_IQR);
}

void sci_uart_txi_isr(void)
{
    bsp_irq_status_clear(ICU_USART_TX_IQR);
    send_flag = 0;
}

int uart_init(void)
{
    bsp_port_init(IOPORT_PORT_01_PIN_00, IOPORT_PERIPHERAL_SCI0_2_4_6_8, IO_PERIPHERAL_MODE);
    bsp_port_init(IOPORT_PORT_01_PIN_01, IOPORT_PERIPHERAL_SCI0_2_4_6_8, IO_PERIPHERAL_MODE);

    bsp_irq_disable(ICU_USART_RX_IQR);
    bsp_irq_disable(ICU_USART_TX_IQR);

    bsp_irq_status_clear(ICU_USART_RX_IQR);
    bsp_irq_status_clear(ICU_USART_TX_IQR);
/*
    NVIC_SetPriority(ICU_USART_RX_IQR, 4);
    NVIC_SetPriority(ICU_USART_TX_IQR, 4);*/

    bsp_sci_uart_init(R_SCI0, 
                      USART_DATA_LENGTH_8BIT, 
                      USART_STOPBITS_1, 
                      USART_NONE_PARITY,
                      USART_MODE_RX | USART_MODE_TX);

    bsp_irq_clear_pending(ICU_USART_RX_IQR);
    bsp_irq_clear_pending(ICU_USART_TX_IQR);
/*
    bsp_irq_enable(ICU_USART_RX_IQR);
    bsp_irq_enable(ICU_USART_TX_IQR);

    bsp_sci_irq_enable(R_SCI0, USART_IRQ_TIE);*/

    return 0;
}
INIT_BOARD_EXPORT(uart_init);

void rt_hw_console_output(const char *str)
{
    rt_enter_critical();
    while(*str != '\0')
    {
        if(*str == '\n')
        {
            while(bsp_sci_get_status(R_SCI0, USART_TDRE) == 0);
            R_SCI0->TDR = '\r';
        }
        while(bsp_sci_get_status(R_SCI0, USART_TDRE) == 0);
        R_SCI0->TDR = *str++;
    }
    rt_exit_critical();
}

char rt_hw_console_getchar(void)
{
    int ch = -1;

    if (bsp_sci_get_status(R_SCI0, USART_RDRF) != 0)
    {
        ch = (int)R_SCI0->RDR;
    }
    else
    {
        if(bsp_sci_get_status(R_SCI0, USART_ORER) != 0)
        {
            bsp_sci_clear_status(R_SCI0, USART_ORER);
        }
        rt_thread_mdelay(10);
    }
    return ch;
}
