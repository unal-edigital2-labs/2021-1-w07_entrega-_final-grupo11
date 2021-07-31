#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <irq.h>
#include <uart.h>
#include <console.h>
#include <generated/csr.h>


#include "delay.h"
#include "display.h"
#include "camara.h"
//#include <tensorflow/c/c_api.h>

static char *readstr(void)
{
	char c[2];
	static char s[64];
	static int ptr = 0;

	if(readchar_nonblock()) {
		c[0] = readchar();
		c[1] = 0;
		switch(c[0]) {
			case 0x7f:
			case 0x08:
				if(ptr > 0) {
					ptr--;
					putsnonl("\x08 \x08");
				}
				break;
			case 0x07:
				break;
			case '\r':
			case '\n':
				s[ptr] = 0x00;
				putsnonl("\n");
				ptr = 0;
				return s;
			default:
				if(ptr >= (sizeof(s) - 1))
					break;
				putsnonl(c);
				s[ptr] = c[0];
				ptr++;
				break;
		}
	}
	return NULL;
}

static char *get_token(char **str)
{
	char *c, *d;

	c = (char *)strchr(*str, ' ');
	if(c == NULL) {
		d = *str;
		*str = *str+strlen(*str);
		return d;
	}
	*c = 0;
	d = *str;
	*str = c+1;
	return d;
}

static void prompt(void)
{
	printf("RUNTIME>");
}

static void help(void)
{
	puts("Available commands:");
	puts("help                            - this command");
	puts("reboot                          - reboot CPU");
	puts("led                             - led test");
	puts("switch                          - switch test");
	puts("display                         - display test");
	puts("rgbled                          - rgb led test");
	puts("vga                             - vga test");
	puts("camara                          - camara test");
	puts("us				- ultraSound test");
	puts("PWM				- PWM test");
	puts("IR				- IR test");
	puts("w				- wheels test");
	puts("dir				- dir test");
}

static void reboot(void)
{
	ctrl_reset_write(1);
}

static void display_test(void)
{
	int i;
	signed char defill = 0;	//1->left, -1->right, 0->.
	
	char txtToDisplay[40] = {0, DISPLAY_0, DISPLAY_1,DISPLAY_2,DISPLAY_3,DISPLAY_4,DISPLAY_5,DISPLAY_6,DISPLAY_7,DISPLAY_8,DISPLAY_9,DISPLAY_A,DISPLAY_B,DISPLAY_C,DISPLAY_D,DISPLAY_E,DISPLAY_F,DISPLAY_G,DISPLAY_H,DISPLAY_I,DISPLAY_J,DISPLAY_K,DISPLAY_L,DISPLAY_M,DISPLAY_N,DISPLAY_O,DISPLAY_P,DISPLAY_Q,DISPLAY_R,DISPLAY_S,DISPLAY_T,DISPLAY_U,DISPLAY_V,DISPLAY_W,DISPLAY_X,DISPLAY_Y,DISPLAY_Z,DISPLAY_DP,DISPLAY_TR,DISPLAY_UR};
	
	printf("Test del los display de 7 segmentos... se interrumpe con el botton 1\n");

	while(!(buttons_in_read()&1)) {
		display(txtToDisplay);
		if(buttons_in_read()&(1<<1)) defill = ((defill<=-1) ? -1 : defill-1);
		if(buttons_in_read()&(1<<2)) defill = ((defill>=1) ? 1 : defill+1);
		if (defill > 0) {
			char tmp = txtToDisplay[0];
			for(i=0; i<sizeof(txtToDisplay)/sizeof(txtToDisplay[i]); i++) {
				txtToDisplay[i] = ((i==sizeof(txtToDisplay)/sizeof(txtToDisplay[i])-1) ? tmp : txtToDisplay[i+1]);
			}
		}
		else if(defill < 0) {
			char tmp = txtToDisplay[sizeof(txtToDisplay)/sizeof(txtToDisplay[0])-1];
			for(i=sizeof(txtToDisplay)/sizeof(txtToDisplay[i])-1; i>=0; i--) {
				txtToDisplay[i] = ((i==0) ? tmp : txtToDisplay[i-1]);
			}
		}
		delay_ms(500);
	}

}

static void led_test(void)
{
	unsigned int i;
	printf("Test del los leds... se interrumpe con el botton 1\n");
	while(!(buttons_in_read()&1)) {

	for(i=1; i<65536; i=i*2) {
		leds_out_write(i);
		delay_ms(50);
	}
	for(i=32768;i>1; i=i/2) {
		leds_out_write(i);
		delay_ms(50);
	}
	}
	
}


static void switch_test(void)
{
	unsigned short temp2 =0;
	printf("Test del los interruptores... se interrumpe con el botton 1\n");
	while(!(buttons_in_read()&1)) {
		unsigned short temp = switchs_in_read();
		if (temp2 != temp){
			printf("switch bus : %i\n", temp);
			leds_out_write(temp);
			temp2 = temp;
		}
	}
}

static void rgbled_test(void)
{
	unsigned int T = 128;
	
	ledRGB_1_r_period_write(T);
	ledRGB_1_g_period_write(T);
	ledRGB_1_b_period_write(T);

	ledRGB_1_r_enable_write(1);
	ledRGB_1_g_enable_write(1);
	ledRGB_1_b_enable_write(1);

	
	ledRGB_2_r_period_write(T);
	ledRGB_2_g_period_write(T);
	ledRGB_2_b_period_write(T);
	
	
	ledRGB_2_r_enable_write(1);
	ledRGB_2_g_enable_write(1);
	ledRGB_2_b_enable_write(1);

	for (unsigned int j=0; j<100; j++){
		ledRGB_1_g_width_write(j);
		for (unsigned int i=0; i<100; i++){
			ledRGB_1_r_width_write(100-i);
			ledRGB_1_b_width_write(i);	
			delay_ms(20);
		}	
	}
	


}


static void vga_test(void)
{
	int x,y;
	
	for(y=0; y<480; y++) {
		for(x=0; x<640; x++) {
			vga_cntrl_mem_we_write(0);
			vga_cntrl_mem_adr_write(y*640+x);
			if(x<640/3)	
				vga_cntrl_mem_data_w_write(((int)(x/10)%2^(int)(y/10)%2)*15);
			else if(x<2*640/3) 
				vga_cntrl_mem_data_w_write((((int)(x/10)%2^(int)(y/10)%2)*15)<<4);
			else 
				vga_cntrl_mem_data_w_write((((int)(x/10)%2^(int)(y/10)%2)*15)<<8);
			vga_cntrl_mem_we_write(1);
		}
	}
}

static void camara_test(void)
{
	unsigned short temp2 =0xFF;
	printf("Test del los camara... se interrumpe con el botton 1\n");
	while(!(buttons_in_read()&1)) {
		unsigned short temp = camara_cntrl_mem_px_data_read();
		if (temp2 != temp){
			printf("el bus de la camara es : %i\n", temp);
			printf("el boton de la camara esta en: %i\n",camara_cntrl_done_read());
			printf("la habilitacion de la interrupción esta en : %i %i %i\n",camara_cntrl_ev_enable_read(), camara_cntrl_ev_status_read(), camara_cntrl_ev_pending_read());
			camara_isr();
			temp2 = temp;
		}
	}
}

static int ultraSound_test(void)
{
	ultraSound_cntrl_init_write(1);
	delay_ms(2);
	while(true){
		if(ultraSound_cntrl_done_read() == 1){
			int d = ultraSound_cntrl_distance_read();
			printf("Distancia = %i\n", d);
			ultraSound_cntrl_init_write(0);
			return 1;
		} 
	}	
}

static void IR_test(void)
{
	printf("Test del infra rojo... se interrumpe con el botton 1\n");
	while(!(buttons_in_read()&1)){
		bool L = IR_cntrl_L_read();
		bool LC = IR_cntrl_LC_read();
		bool C = IR_cntrl_C_read();
		bool RC = IR_cntrl_RC_read();
		bool R = IR_cntrl_R_read();
		printf("---------------------------------------\n");
		printf("Izquierda = %i\n", L);
		printf("Izquierda centro= %i\n", LC);
		printf("Centro = %i\n", C);
		printf("Derecha centro = %i\n", RC);
		printf("Derecha = %i\n", R);
		printf("---------------------------------------\n");
		delay_ms(1000);
	}	
}


static void w_test(void){
	int state = 0;
	while(true){
		switch(state){
			case 0: 
				wheels_cntrl_state_write(0);
				delay_ms(1000);
				state = 1;
				break;
			case 1: 
				wheels_cntrl_state_write(1);
				delay_ms(1000);
				state = 2;
				break;
			case 2: 
				wheels_cntrl_state_write(2);
				delay_ms(1000);
				state = 3;
				
				break;
			case 3: 
				wheels_cntrl_state_write(3);
				delay_ms(1000);
				return;
				break; 
		}
	}
}

static void US(void){
	int state = 0;
	while(true){
		switch(state){
			case 0: 
				PWMUS_cntrl_pos_write(0);
				delay_ms(1000);
				if(ultraSound_test() == 1){
					state = 1;
				}
				break;
			case 1: 
				PWMUS_cntrl_pos_write(1);
				delay_ms(1000);
				if(ultraSound_test() == 1){
					state = 2;
				}
				break;
			case 2: 
				PWMUS_cntrl_pos_write(2);
				delay_ms(1000);
				if(ultraSound_test() == 1){
					state = 3;
				}
				break;
			case 3: 
				PWMUS_cntrl_pos_write(0);
				return;
				break; 
		}
	}
}

static void PWMUS_test(void)
{
	PWMUS_cntrl_pos_write(0);
	delay_ms(1000);
	PWMUS_cntrl_pos_write(1);
	delay_ms(1000);
	PWMUS_cntrl_pos_write(2);
	delay_ms(1000);
	PWMUS_cntrl_pos_write(3);
}


static void direction(void){
	while(!(buttons_in_read()&1)){
		bool L = IR_cntrl_L_read();
		bool LC = IR_cntrl_LC_read();
		bool C = IR_cntrl_C_read();
		bool RC = IR_cntrl_RC_read();
		bool R = IR_cntrl_R_read();

		bool IR[5] = {L, LC, C, RC, R};

		for(int i = 0; i<5; i++){
			printf("%i, ", IR[i]);
		}
		printf("\n");

		if(L==0 && LC==0 && C==1 && RC==0 && R==0)
			wheels_cntrl_state_write(0);
		else if(L==0 && LC==1 && C==1 && RC==0 && R==0)
			wheels_cntrl_state_write(0);
		else if(L==0 && LC==1 && C==1 && RC==0 && R==0)
			wheels_cntrl_state_write(2);
		else if(L==0 && LC==0 && C==1 && RC==1 && R==0)
			wheels_cntrl_state_write(1);
		else if(L==0 && LC==1 && C==0 && RC==0 && R==0)
			wheels_cntrl_state_write(2);
		else if(L==0 && LC==0 && C==0 && RC==1 && R==0)
			wheels_cntrl_state_write(1);
		else if(L==1 && LC==0 && C==0 && RC==0 && R==0)
			wheels_cntrl_state_write(2);
		else if(L==0 && LC==0 && C==0 && RC==0 && R==1)
			wheels_cntrl_state_write(1);
		else if(L==0 && LC==1 && C==1 && RC==1 && R==1)
			wheels_cntrl_state_write(3);
		else if(L==1 && LC==1 && C==1 && RC==1 && R==0)
			wheels_cntrl_state_write(3);
		else if(L==1 && LC==1 && C==1 && RC==1 && R==1)
			wheels_cntrl_state_write(3);
		
	}

}











static void console_service(void)
{
	char *str;
	char *token;

	str = readstr();
	if(str == NULL) return;
	token = get_token(&str);
	if(strcmp(token, "help") == 0)
		help();
	else if(strcmp(token, "reboot") == 0)
		reboot();
	else if(strcmp(token, "led") == 0)
		led_test();
	else if(strcmp(token, "switch") == 0)
		switch_test();
	else if(strcmp(token, "display") == 0)
		display_test();
	else if(strcmp(token, "rgbled") == 0)
		rgbled_test();
	else if(strcmp(token, "vga") == 0)
		vga_test();
	else if(strcmp(token, "camara") == 0)
		camara_test();
	else if(strcmp(token, "us") == 0)
		ultraSound_test();
	else if(strcmp(token, "PWM") == 0)
		PWMUS_test();
	else if(strcmp(token, "US") == 0)
		US();
	else if(strcmp(token, "IR") == 0)
		IR_test();
	else if(strcmp(token, "w") == 0)
		w_test();
	else if(strcmp(token, "dir") == 0)
		direction();
	prompt();
}

int main(void)
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();
	camara_init();
	wheels_cntrl_state_write(3);
	
	
	//printf("Hello from TensorFlow C library version %s\n", TF_Version());
	
	
	puts("\nSoC - RiscV project UNAL 2020-2-- CPU testing software  interrupt "__DATE__" "__TIME__"\n");
	help();
	prompt();

	while(1) {
		console_service();
	}

	return 0;
}
