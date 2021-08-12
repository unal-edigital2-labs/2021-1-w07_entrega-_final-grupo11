# W07_Entrega-_final

## Integrantes 

- David Ariza

- Juan Rubiano

- Julian Villalobos

- Ronneth Briceño

- Camilo Zambrano

## Introducción

Este repositorio contiene la entrega final del proyecto de la materia Electrónica Digital II de la Universidad Nacional de Colombia. La propuesta del proyecto consiste en un robot cartográfico que recorre un laberinto identificando ciertos puntos de importancia, mapeando la distribución del laberinto mientras lo recorre. El robot contiene un módulo Bluetooth para la transmisión de datos y un módulo mp3 que indica en qué sentido se mueve el robot. Se utilizan los lenguajes de programación: Verilog, Python y C. 

Cada módulo se encuentra explicado en la documentación dentro de la carpeta module, mientras que en este documento se explicará la integración y el funcionamiento conjunto de los periféricos.

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
### Prueba funcional



## Movimiento

El movimiento en el robot incluye las ruedas, los infrarrojos, el ultrasonido y el mp3. La sección de dirección en el main.c describe el funcionamiento en conjunto de estos periféricos. 

Para comenzar el código le indica a las ruedas que avancen hacia adelante, se inicializan variables como rotate y orientation, y se inicializa la matriz del mapa del laberinto.

``` c

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

```
Luego, se tienen las variables que leen los registros del infrarrojo para procesar qué es lo que "observa" el robot; además se incluye una función para resetear las respectivas variables en las que se incluyen la rotación, la orientación y la posición.

``` c
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
```

En los estados de rotate se define el movimiento del robot basándose en la lectura de los registros de cada infrarrojo. Por ejemplo, basandose en los valores de L, LC, C, RC y R, dependiendo de cuáles de ellos estén activos, el robot realizará los movimientos definidos en el módulo de las ruedas.

Considerando el siguiente extracto del código:

``` c
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
```

El rotate es utilizado para centrar el robot en el caso de que se encuentre "inclinado" en el laberinto, estos comandos se ejecutan mientras los infrarrojos no se encuentren activos en su totalidad (es decir, mientras algunas de las variables mostradas anteriormente, como R o L, sigan encontrándose en 0). 

Cuando todos los infrarrojos se encuentren activos, el robot entiende que está en una intersección, esto hace que el robot se quede quieto y llame la función del ultrasonido, donde este mide hacia todas las direcciones y registra las distancias en cada una. Esto se muestra en el siguiente extracto del código:

``` c
}else if(L==1 && LC==1 && C==1 && RC==1 && R==1 && rotate==0){
		wheels_cntrl_state_write(3);
		rotate = 1;
		int *d = US();
		showD(d);
```

De donde se ejecuta la siguiente sección que se encarga de comparar las mediciones de los registros y decidir qué dirección debe tomar el robot:

``` c
//d[0] derecha - d[1] centro - d[2] izquierda 
	if(d[0] >= 35 && d[1] >= 35 && d[2] >= 35){
			mp3(1);
			wheels_cntrl_state_write(2);
			delay_ms(400);
			orientation = orientation + 1;
```