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

static void bluetooth_write(char *str){
	for(int i = 0;i<strlen(str);i++){
		uart1_rxtx_write(str[i]);
		delay_ms(1);
	}
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
	puts("b				- bluetooth config test");
	puts("mp				- mp3 test");
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
	while(!(buttons_in_read()&1)){
		//printf("%i \n", camara_cntrl_data_mem_read());
	}
}

static int ultraSound_test(void)
{
	ultraSound_cntrl_init_write(1);
	// Se esperan 2 ms para dar tiempo a que el registro done se actualice
	delay_ms(2);
	while(true){
		if(ultraSound_cntrl_done_read() == 1){
			int d = ultraSound_cntrl_distance_read();
			ultraSound_cntrl_init_write(0);
			return d;
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

		bool IR[5] = {L, LC, C, RC, R};

		for(int i = 0; i<5; i++){
			printf("%i, ", IR[i]);
		}
		printf("\n");
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
				state = 4;
				break; 
			case 4: 
				wheels_cntrl_state_write(4);
				delay_ms(1000);
				wheels_cntrl_state_write(3);
				return;
				break; 

		}
	}
}

static int * US(void){
	int state = 0;
	static int d[3];
	while(true){
		switch(state){
			case 0: 
				// Se mueve el servomotor a 0° (mirando a la derecha)
				PWMUS_cntrl_pos_write(0);
				//Se da tiempo a que el servotor se posicione
				delay_ms(1000);
				//Se llama a la función ultraSound_test() y se guarda en la primera posición del array
				d[0] = ultraSound_test();
				state = 1;
				break;
			case 1: 
				// Se repite el proceso pero a 90°
				PWMUS_cntrl_pos_write(1);
				delay_ms(1000);
				d[1] = ultraSound_test();
				state = 2;
				break;
			case 2: 
				PWMUS_cntrl_pos_write(2);
				delay_ms(1000);
				d[2] = ultraSound_test();
				state = 3;
				break;
			case 3: 
				// Se repite el proceso a 180°
				PWMUS_cntrl_pos_write(0);
				delay_ms(1000);
				// Se imprimen las distancias por el serial y por el bluetooth
				char distances[3];
				printf("----------\n");
				bluetooth_write("----------\n");
				for(int i = 2; i>=0; i--){
					printf("%d", d[i]);
					sprintf(distances, "%d", d[i]);
					bluetooth_write(distances);
					if(i>0){
						printf(" - ");
						bluetooth_write(" - ");
					}
				}
				bluetooth_write("\n");
				printf("\n");
				// Se retorna el arreglo con las 3 mediciones
				return d;
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

static void showD(int d[8]){ 
	char distances[8];
	distances[0] = d[0]/10; 
	distances[1] = d[0]%10; 

	distances[2] = DISPLAY_C;

	distances[3] = d[1]/10;
	distances[4] = d[1]%10;

	distances[5] = DISPLAY_C;

	distances[6] = d[2]/10;
	distances[7] = d[2]%10;

	for(int i = 0; i<8; i++){
		switch(distances[i]){
			case 0: distances[i] = DISPLAY_0;
				break;
			case 1: distances[i] = DISPLAY_1;
				break;
			case 2: distances[i] = DISPLAY_2;
				break;
			case 3: distances[i] = DISPLAY_3;
				break;
			case 4: distances[i] = DISPLAY_4;
				break;
			case 5: distances[i] = DISPLAY_5;
				break;
			case 6: distances[i] = DISPLAY_6;
				break;
			case 7: distances[i] = DISPLAY_7;
				break;
			case 8: distances[i] = DISPLAY_8;
				break;
			case 9: distances[i] = DISPLAY_9;
				break;
		}
	}
	display(distances);	
}

static void mp3(int track){
	// 1=Derecha
	// 2=Izquierda
	// 3=Recto
	// 4=Vuelta

	uart2_rxtx_write(0x7E);
	uart2_rxtx_write(0xFF);
	uart2_rxtx_write(0x06);
	uart2_rxtx_write(0x03);
	uart2_rxtx_write(0x00);
	uart2_rxtx_write(0x00);
	uart2_rxtx_write(track);
	uart2_rxtx_write(0xEF);
}
 

static void mp_test(void){

	//Volumen
	uart2_rxtx_write(0x7E);
	uart2_rxtx_write(0xFF);
	uart2_rxtx_write(0x06);
	uart2_rxtx_write(0x06);
	uart2_rxtx_write(0x00);
	uart2_rxtx_write(0x00);
	uart2_rxtx_write(0x30);
	uart2_rxtx_write(0xEF);

	//DERECHA
	//start
	uart2_rxtx_write(0x7E);
	uart2_rxtx_write(0xFF);
	//length
	uart2_rxtx_write(0x06);
	//cmd
	uart2_rxtx_write(0x03);
	//feedback
	uart2_rxtx_write(0x00);
	//track
	uart2_rxtx_write(0x00);
	uart2_rxtx_write(1);
	//end
	uart2_rxtx_write(0xEF);
	delay_ms(2000);

	//IZQUIERDA
	//start
	uart2_rxtx_write(0x7E);
	uart2_rxtx_write(0xFF);
	//length
	uart2_rxtx_write(0x06);
	//cmd
	uart2_rxtx_write(0x03);
	//feedback
	uart2_rxtx_write(0x00);
	//track
	uart2_rxtx_write(0x00);
	uart2_rxtx_write(2);
	//end
	uart2_rxtx_write(0xEF);
	delay_ms(2000);

	//RECTO
	//start
	uart2_rxtx_write(0x7E);
	uart2_rxtx_write(0xFF);
	//length
	uart2_rxtx_write(0x06);
	//cmd
	uart2_rxtx_write(0x03);
	//feedback
	uart2_rxtx_write(0x00);
	//track
	uart2_rxtx_write(0x00);
	uart2_rxtx_write(3);
	//end
	uart2_rxtx_write(0xEF);
	delay_ms(2000);

	//Vuelta
	//start
	uart2_rxtx_write(0x7E);
	uart2_rxtx_write(0xFF);
	//length
	uart2_rxtx_write(0x06);
	//cmd
	uart2_rxtx_write(0x03);
	//feedback
	uart2_rxtx_write(0x00);
	//track
	uart2_rxtx_write(0x00);
	uart2_rxtx_write(4);
	//end
	uart2_rxtx_write(0xEF);
	delay_ms(2000);
	
}


static void direction(void){
	wheels_cntrl_state_write(0);
	int rotate = 0;
	int orientation = 0;
	int posV = 1;
    int posH = 0;
	int map[10][10];

	for(int i=0;i<10;i++){
        for(int k=0;k<10;k++){  
            map[i][k] = 0;
        }    
    }

	char *tempMap = "A";
	while(!(buttons_in_read()&1)){
		bool L = IR_cntrl_L_read();
		bool LC = IR_cntrl_LC_read();
		bool C = IR_cntrl_C_read();
		bool RC = IR_cntrl_RC_read();
		bool R = IR_cntrl_R_read();

		if(buttons_in_read()&2){
			rotate = 0;
			orientation = 0;
			posV = 1;
			posH = 0;
			for(int i=0;i<10;i++){
				for(int k=0;k<10;k++){  
					map[i][k] = 0;
				}    
			}
			wheels_cntrl_state_write(0);
		}

		

		if(rotate == 2){
			wheels_cntrl_state_write(4);
			//01110
			if(L==0 && LC==1 && C==1 && RC==1 && R==0){
				rotate = 0;
				//01100
			}else if(L==0 && LC==1 && C==1 && RC==0 && R==0){
				rotate = 0;
				
				//00110
			}else if(L==0 && LC==0 && C==1 && RC==1 && R==0){
				rotate = 0;
				
				//01000
			}
		}else{
				//00100
			if(L==0 && LC==0 && C==1 && RC==0 && R==0){
				wheels_cntrl_state_write(0);
				rotate = 0;
				
				//01110
			}else if(L==0 && LC==1 && C==1 && RC==1 && R==0){
				wheels_cntrl_state_write(0);
				rotate = 0;
				
				//01100
			}else if(L==0 && LC==1 && C==1 && RC==0 && R==0){
				wheels_cntrl_state_write(0);
				rotate = 0;
				
				//00110
			}else if(L==0 && LC==0 && C==1 && RC==1 && R==0){
				wheels_cntrl_state_write(0);
				rotate = 0;
				
				//01000
			}else if(L==0 && LC==1 && C==0 && RC==0 && R==0){
				wheels_cntrl_state_write(1);
				rotate = 0;
				
				//00010
			}else if(L==0 && LC==0 && C==0 && RC==1 && R==0){
				wheels_cntrl_state_write(2);
				rotate = 0;
				
				//10000
			}else if(L==1 && LC==0 && C==0 && RC==0 && R==0){
				wheels_cntrl_state_write(1);

				//00001
			}else if(L==0 && LC==0 && C==0 && RC==0 && R==1){
				wheels_cntrl_state_write(2);

			}else if(L==1 && LC==1 && C==1 && RC==1 && R==1 && rotate==0){
				wheels_cntrl_state_write(3);
				rotate = 1;
				int *d = US();
				showD(d);

			//d[0] derecha - d[1] centro - d[2] izquierda 
				if(d[0] >= 35 && d[1] >= 35 && d[2] >= 35){
					mp3(1);
					wheels_cntrl_state_write(2);
					delay_ms(400);
					orientation = orientation + 1;
				}else if(d[0] >= 35 && d[1] >= 35 && d[2] < 35){
					mp3(1);
					wheels_cntrl_state_write(2);
					delay_ms(400);
					orientation = orientation + 1;
				}else if(d[0] >= 35 && d[1] < 35 && d[2] < 35){
					mp3(1);
					wheels_cntrl_state_write(2);
					delay_ms(400);
					orientation = orientation + 1;
				}else if(d[0] < 35 && d[1] >= 35 && d[2] >= 35){
					
					wheels_cntrl_state_write(0);
					delay_ms(200);
					mp3(3);
				}else if(d[0] < 35 && d[1] < 35 && d[2] >= 35){
					mp3(2);
					wheels_cntrl_state_write(1);
					delay_ms(200);
					orientation = orientation - 1;
				}else if(d[0] < 35 && d[1] >= 35 && d[2] < 35){			
					wheels_cntrl_state_write(0);
					delay_ms(200);
					mp3(3);
				}else if(d[0] < 35 && d[1] < 35 && d[2] < 35){			
					mp3(4);
					wheels_cntrl_state_write(5);
					delay_ms(200);
					wheels_cntrl_state_write(4);
					delay_ms(500);
					rotate = 2;
					orientation = orientation + 2;
				}
				
				if(orientation >= 4)
					orientation = orientation - 4; 
				else if(orientation < 0) 
					orientation = orientation + 4;

				//Arreglar
				if(orientation == 0){
					posH++;
				}else if(orientation == 1){
					posV++;
				}else if(orientation == 2){
					posH--;
				}else if(orientation == 3){
					posV--;
				}

				//Desplazamiento de la matriz
				if(posV < 0){
					for(int i=10;i>=0;i--){
						for(int k=0;k<10;k++){  
							if(i == 0){
								map[i][k] = 0;
							}else{
								map[i][k] = map[i-1][k];
							}   
						} 
					}
					posV++;
				}

				if(posH < 0){
					for(int i=0;i<10;i++){
						for(int k=10;k>=0;k--){  
							if(k == 0){
								map[i][k] = 0;
							}else{
								map[i][k] = map[i][k-1];
							}   
						} 
					}
					posH++;
				}

				for(int i=0;i<10;i++){
					for(int k=0;k<10;k++){
						map[posV][posH] = 1;
						if(posV!=0 && posH != 0){
							if(map[posV][posH-1]==0)
								map[posV][posH-1]=2;
							if(map[posV][posH+1]==0)
								map[posV][posH+1]=2;
							if(map[posV-1][posH]==0)
								map[posV-1][posH]=2;
							if(map[posV+1][posH]==0)
								map[posV+1][posH]=2;
							if(map[posV-1][posH-1]==0)
								map[posV-1][posH-1]=2;
							if(map[posV-1][posH+1]==0)
								map[posV-1][posH+1]=2;
							if(map[posV+1][posH-1]==0)
								map[posV+1][posH-1]=2;
							if(map[posV+1][posH+1]==0)
								map[posV+1][posH+1]=2;
						}else if(posV!=0 && posH == 0){
							if(map[posV-1][posH]==0)
								map[posV-1][posH]=2;
							if(map[posV+1][posH]==0)
								map[posV+1][posH]=2;
							if(map[posV-1][posH+1]==0)
								map[posV-1][posH+1]=2;
							if(map[posV+1][posH+1]==0)
								map[posV+1][posH+1]=2;
							if(map[posV][posH+1]==0)
								map[posV][posH+1]=2;
						}else if(posV==0 && posH != 0){
							if(map[posV+1][posH-1]==0)
								map[posV+1][posH-1]=2;
							if(map[posV+1][posH+1]==0)
								map[posV+1][posH+1]=2;
							if(map[posV+1][posH]==0)
								map[posV+1][posH]=2;
							if(map[posV][posH-1]==0)
								map[posV][posH-1]=2;
							if(map[posV][posH+1]==0)
								map[posV][posH+1]=2;
							if(map[posV][posH+1]==0)
								map[posV][posH+1]=2;
						}
						printf("%i",  map[i][k]);
						sprintf(tempMap, "%d", map[i][k]);
						bluetooth_write(tempMap);
					}
					printf("\n");
					bluetooth_write("\n");
				}
			}
		}		
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
	else if(strcmp(token, "mp") == 0)
		mp_test();

		
	prompt();
}

int main(void)
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();

	//camara_init();
	wheels_cntrl_state_write(3);

	//Volumen MP3
	uart2_rxtx_write(0x7E);
	uart2_rxtx_write(0xFF);
	uart2_rxtx_write(0x06);
	uart2_rxtx_write(0x06);
	uart2_rxtx_write(0x00);
	uart2_rxtx_write(0x00);
	uart2_rxtx_write(0x30);
	uart2_rxtx_write(0xEF);

	//direction();
	
	puts("\nSoC - RiscV project UNAL 2021-1 -- CPU testing software  interrupt "__DATE__" "__TIME__"\n");
	help();
	prompt();
	
	while(1) {
		console_service();
	}

	return 0;
}
