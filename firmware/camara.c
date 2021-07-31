#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camara.h"

#include <uart.h>
#include <irq.h>
#include <generated/csr.h>

void camara_isr(void)
{
	unsigned int stat;

	stat = camara_cntrl_ev_pending_read();

	if(stat) {
		uart_ev_pending_write(UART_EV_RX);
		puts("\nSE ACTIVO LA INTERRUPCION DE LA CAMARA  SIIIII \n");

	}
	
}
void camara_init(void)
{
	camara_cntrl_ev_pending_write(camara_cntrl_ev_pending_read());
	camara_cntrl_ev_enable_write(1);
	irq_setmask(irq_getmask() | (1 << CAMARA_CNTRL_INTERRUPT));
}



