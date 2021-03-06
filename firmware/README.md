1. [ MP3. ](#mp3)
2. [ Ultrasonido ](#us)
3. [ Cámara. ](#camara)
4. [ Movimiento. ](#movimiento)

<a name="mp3"></a>
### MP3

Para el mp3, lo que se planteó fue la ejecución de ciertos sonidos dependiendo de la dirección que tome nuestro robot cartógrafo. Para hacerlo, se realiza una función la cual debe incluir una cadena de datos específica, tomándola directamente del [documento](../datasheets/DFR0299-DFPlayer-Mini-Manual.pdf) del fabricante.

``` c
static void mp3(int track){
	// 1=Derecha
	// 2=Izquierda
	// 3=Recto
	// 4=Vuelta

	uart2_rxtx_write(0x7E); //Inicio cadena
	uart2_rxtx_write(0xFF); //Versión
	uart2_rxtx_write(0x06); //Longitud bytes
	uart2_rxtx_write(0x03); //Comando
	uart2_rxtx_write(0x00); //Feedback
	uart2_rxtx_write(0x00); //Parámetro 1
	uart2_rxtx_write(track); //Parámetro 2 (variable)
	uart2_rxtx_write(0xEF); //Fin de cadena
}

```

<a name="us"></a>
## Ultrasonido

Para la implementación del ultrasonido en C, se realizan 2 funciones, una para realizar una medición individual del ultrasonido y otra que implementa al servomotor y realiza tres mediciones, una en cada dirección. Para la primera:


``` c
static int ultraSound_test(void)
{
	ultraSound_cntrl_init_write(1);
	// Se esperan 2 ms para dar tiempo a que el registro done se actualice
	delay_ms(2);
	while(true){
		if(ultraSound_cntrl_done_read() == 1){
			// Si done es 1, se lee y se retorna la distancia
			int d = ultraSound_cntrl_distance_read();
			ultraSound_cntrl_init_write(0);
			return d;
		} 
	}	
}

```

Para la implementación con el servomotor:

``` c
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
```

<a name="camara"></a>
## Cámara

Para la implementación de la cámara en C fue necesario reducir el tamaño de la imagen por 100 * 75 ya que al utilizar 160 * 120 no es posible cargar el menú del Soc, se crea una función para que muestre por la VGA la imagen del tamaño especificado.

``` c

static void camara_test(void)
{
	 for(int y=0;y<480;y++){
	 	for(int x=0;x<640;x++){
	 		vga_cntrl_mem_we_write(0);
	 		vga_cntrl_mem_adr_write(y*640+x);
	 		if(x<100 && y<75){
	 			camara_cntrl_DP_RAM_addr_out_write(75*y+x);
	 			vga_cntrl_mem_data_w_write(camara_cntrl_data_mem_read());
	 		}else
	 			vga_cntrl_mem_data_w_write(0);
	 		vga_cntrl_mem_we_write(1);
	 	}	
	 }
}


``` 

<a name="movimiento"></a>
## Movimiento

El movimiento en el robot incluye las ruedas, los infrarrojos, el ultrasonido y el mp3. La sección de dirección en el main.c describe el funcionamiento en conjunto de estos periféricos. 

Para comenzar el código le indica a las ruedas que avancen hacia adelante, se inicializan variables como rotate y orientation, y se inicializa la matriz del mapa del laberinto.

``` c

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

```
Luego, se tienen las variables que leen los registros del infrarrojo para procesar qué es lo que "observa" el robot; además se incluye una función para resetear las respectivas variables en las que se incluyen la rotación, la orientación y la posición.

``` c
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
```

En los estados de rotate se define el movimiento del robot basándose en la lectura de los registros de cada infrarrojo. Por ejemplo, basandose en los valores de L, LC, C, RC y R, dependiendo de cuáles de ellos estén activos, el robot realizará los movimientos definidos en el módulo de las ruedas.

Considerando el siguiente extracto del código:

``` c
if(((L==0 && C==1 && R==0) || (L==1 && C==1 && R==1)) && L==0 && R==0){
	wheels_cntrl_state_write(0);
}else if((LC==1 && C==0) || (L==1 && C==0 && R==0) || (L==1 && C==1 && R==0) ){
	wheels_cntrl_state_write(1);
}else if((C==0 && RC==1) || (L==0 && C==0 && R==1) || (L==0 && C==1 && R==1)){
	wheels_cntrl_state_write(2);
}else if(L==1 && LC==1 && C==1 && RC==1 && R==1){
	wheels_cntrl_state_write(3);
```

Cuando todos los infrarrojos se encuentren activos, el robot entiende que está en una intersección, esto hace que el robot se quede quieto y llame la función del ultrasonido, donde este mide hacia todas las direcciones y registra las distancias en cada una, tambien envía los datos por bluetooth y muestras las distancias en los displays de 7 segmentos. Esto se muestra en el siguiente extracto del código:

``` c
}else if(L==1 && LC==1 && C==1 && RC==1 && R==1){
	wheels_cntrl_state_write(3);
	int *d = US();
	showD(d);
```

De donde se ejecuta la siguiente sección que se encarga de comparar las mediciones de los registros y decidir qué dirección debe tomar el robot. Cuando elije una dirección, la anuncia por la bocina del MP3:

``` c
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
	orientation = orientation + 2;
}
```

La variable orientation define en que dirección se esta moviendo el carro, de esta manera se puede establecer cual valor de la matriz se coloca en 1, dado que solo hay 4 direcciones, cuando realiza un giro completo se reinicia la dirección:

``` c
if(orientation >= 4)
	orientation = orientation - 4; 
else if(orientation < 0) 
	orientation = orientation + 4;
```

Finalmente se actualizan los datos de la matriz para ser enviada por bluetooth. Ejemplo de los datos que se envían:




Cuando se reciben los datos, se pueden mostrar de forma visual por medio de algún programa que interprete los strings que se obtienen por bluetooth, en este caso, se uso el lenguaje de programación Processing, de forma que el laberinto se ve de la siguiente manera:


![Alt text](/images/labP.png)


