#include <generated/csr.h>
#include <irq.h>
#include <uart.h>

#include "camara.h"

extern void periodic_isr(void);

void isr(void);
void isr(void)
{
	unsigned int irqs;

	irqs = irq_pending() & irq_getmask();
	leds_out_write(irqs);
	if(irqs & (1 << UART_INTERRUPT))
		uart_isr();

	if(irqs & (1 << CAMARA_CNTRL_INTERRUPT))
		camara_isr();
}
