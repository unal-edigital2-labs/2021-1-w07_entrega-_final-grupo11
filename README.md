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