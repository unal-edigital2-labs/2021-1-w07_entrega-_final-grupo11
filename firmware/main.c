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
	puts("camara                          - camara test");
	puts("us				- ultraSound test");
	puts("PWM				- PWM test");
	puts("IR				- IR test");
	puts("w				- wheels test");
	puts("dir				- dir test");
	puts("mp				- mp3 test");
	puts("b				- bluetooth test");
}

static void reboot(void)
{
	ctrl_reset_write(1);
}


static void camara_test(void)
{

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

	for(int i=1;i<=4;i++){
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
		uart2_rxtx_write(i);
		//end
		uart2_rxtx_write(0xEF);
		delay_ms(2000);
	}	
}


static void direction(void){
	wheels_cntrl_state_write(0);
	int orientation = 0;
	int posV = 0;
    int posH = 0;
	int map[10][10];

	//Inicialización de la matriz del laberinto
	for(int i=0;i<10;i++){
        for(int k=0;k<10;k++){  
            map[i][k] = 0;
        }    
    }

	char *tempMap = "A";
	while(true){
		bool L = IR_cntrl_L_read();
		bool LC = IR_cntrl_LC_read();
		bool C = IR_cntrl_C_read();
		bool RC = IR_cntrl_RC_read();
		bool R = IR_cntrl_R_read();

		if(buttons_in_read()&2){
			orientation = 0;
			posV = 1;
			posH = 0;
			for(int i=0;i<10;i++){
				for(int k=0;k<10;k++){
					map[i][k] = 0;
					printf("%i",  map[i][k]);
					sprintf(tempMap, "%d", map[i][k]);
					bluetooth_write(tempMap);
				}
				printf("\n");
				bluetooth_write("\n");
			}
			wheels_cntrl_state_write(0);
		}		


		//Dirección
		//Estados de los infrarrojos
		if(((L==0 && C==1 && R==0) || (L==1 && C==1 && R==1)) && L==0 && R==0){
			wheels_cntrl_state_write(0);
		}else if((LC==1 && C==0) || (L==1 && C==0 && R==0) || (L==1 && C==1 && R==0) ){
			wheels_cntrl_state_write(1);
		}else if((C==0 && RC==1) || (L==0 && C==0 && R==1) || (L==0 && C==1 && R==1)){
			wheels_cntrl_state_write(2);
		}else if(L==1 && LC==1 && C==1 && RC==1 && R==1){
			wheels_cntrl_state_write(3);
			int *d = US();
			showD(d);

			//Rotación
			//d[0] derecha - d[1] centro - d[2] izquierda 
			if(d[0] >= 35){
				mp3(1);
				wheels_cntrl_state_write(2);
				delay_ms(400);
				orientation = orientation + 1;
			}else if(d[0] < 35 && d[1] >= 35){
				wheels_cntrl_state_write(0);
				delay_ms(200);
				mp3(3);
			}else if(d[0] < 35 && d[1] < 35 && d[2] >= 35){
				mp3(2);
				wheels_cntrl_state_write(1);
				delay_ms(400);
				orientation = orientation - 1;
			}else{			
				mp3(4);
				wheels_cntrl_state_write(5);
				delay_ms(200);
				wheels_cntrl_state_write(4);
				delay_ms(500);
				while(LC!=1 && C!=1 && RC!=1){
					wheels_cntrl_state_write(4);
				}
				orientation = orientation + 2;
			}
			
			//Reset de la orientación
			if(orientation >= 4)
				orientation = orientation - 4; 
			else if(orientation < 0) 
				orientation = orientation + 4;

			

			//Desplazamiento de la matriz
			if(posV < 0){
				for(int i=10;i>=0;i--){
					for(int k=0;k<10;k++){  
						if(i != 0){
							map[i][k] = map[i-1][k];
						}else{
							map[i][k] = 0;
						}   
					} 
				}
				posV++;
			}

			if(posH < 0){
				for(int i=0;i<10;i++){
					for(int k=10;k>=0;k--){  
						if(k != 0){
							map[i][k] = map[i][k-1];
						}else{
							map[i][k] = 0;
						}   
					} 
				}
				posH++;
			}

			//Actualización del mapa y envío por bluetooth
			for(int i=0;i<10;i++){
				for(int k=0;k<10;k++){
					map[posV][posH] = 1;
					printf("%i",  map[i][k]);
					sprintf(tempMap, "%d", map[i][k]);
					bluetooth_write(tempMap);
				}
				printf("\n");
				bluetooth_write("\n");
			}

			//Orientación del carro en el laberinto
			if(orientation == 0){
				posH++;
			}else if(orientation == 1){
				posV++;
			}else if(orientation == 2){
				posH--;
			}else if(orientation == 3){
				posV--;
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
	else if(strcmp(token, "b") == 0)
		bluetooth_write("Prueba de funcionamiento del bluetooth \n");
		
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
