#ifndef _UART_H_
#define _UART_H_

#include "ra2e1.h"
#include "rtthread.h"

#define ICU_USART_RX_IQR        ICU_IRQ_NUMBER_0
#define ICU_USART_TX_IQR        ICU_IRQ_NUMBER_1

int uart_init(void);
void uart_send(char *buf);

#endif