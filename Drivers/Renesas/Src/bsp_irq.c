#include "ra2e1.h"

void sci_uart_rxi_isr(void);
void sci_uart_txi_isr(void);
void icu_irq7_isr(void);

const icu_event_t g_interrupt_event_link_select[32] =
{
    [ICU_IRQ_NUMBER_0] = ICU_EVENT_SCI0_RXI,
    [ICU_IRQ_NUMBER_1] = ICU_EVENT_SCI0_TXI,
    [ICU_IRQ_NUMBER_7] = ICU_EVENT_ICU_IRQ7
};

const fsp_vector_t g_vector_table[32] __attribute__((section(".application_vectors"))) __attribute__((__used__)) =
{
    [ICU_IRQ_NUMBER_0] = sci_uart_rxi_isr,
    [ICU_IRQ_NUMBER_1] = sci_uart_txi_isr,
    [ICU_IRQ_NUMBER_7] = icu_irq7_isr
};

void bsp_irq_cfg(void)
{
    for(uint32_t i = 0;i < 32;i++)
    {
        R_ICU->IELSR[i] = g_interrupt_event_link_select[i];
    }
}