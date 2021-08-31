1. [ Ultra Sonido. ](#us)
2. [ Servomotor del ultra sonido. ](#pwmus)
3. [ Ruedas. ](#ruedas)
4. [ Bluetooth. ](#blue)
5. [ mp3. ](#mp3)
6. [ InfraRojo ](#InfraRojo)
7. [ Camara ](#Camara)

<a name="us"></a>
# Ultrasonido

Para este periferico se utilizo el ultrasonido HC-sr04, a partir del [documento](../datasheets/HCSR04.pdf) proporcionado por el fabricante, el ultrasonido debe recibir una señal de 10 us por el pin trig, de esta manera, se emiten 8 rafagas de sonido a 40 kHz, posteriormente, se cuenta el tiempo que transcurre hasta que el sonido vuelva. A partir de esto, se establece que la distancia es igual al tiempo en microsegundos dividido entre 58. Con estas indicaciones se implementa la siguiente maquina de estados:


En esta sección se establece que cuando la señal init esta en 1, los registros done, counter, y distance se colocan en 0 y se pasa al siguiente estado.

``` verilog
parameter Start = 0, Pulse = 'd1, Echo = 'd2;
always @ (posedge newCLK)begin

    case(status)
        Start:  begin
                    if(init) begin
                        done = 0;
                        counter = 0;
                        distance = 0;
                        status = Pulse;
                    end
                end

```

Para el segundo estado, se envía un 1 en trig durante 11 us, en este caso, se aumenta la duración recomendada por el fabricante ya que al usar 10 us se observaban fallos en la medición.

``` verilog 
Pulse:  begin
                    trig = 1'b1;
                    counter = counter + 1'b1;
                        if(counter == 'd11)begin
                            trig = 0;
                            counter = 0;
                            status = Echo;
                        end
                end

```

En el ultimo estado, se espera a que echo sea 1, se cuenta cuanto tiempo transcurre hasta que sea 0, y se divide el contador entre 58 para obtener la distancia en centimetros. En caso de que el ultrasonido falle y el contador sea 0, se devuelve al estado anterior hasta obtener un valor de distancia.

``` verilog
Echo:   begin
                      if(echo)begin
                        echoStart = 1;
                        counter = counter + 1'b1;
                      end
                      if(echo == 0 && echoStart == 1)begin
                        
                        if(counter == 0) status = Pulse;
                        else begin
                            distance = counter/'d58;
                            status = Start;
                            done = 1;
                        end
                      end
                end
```


Para la implementación en python, se establece que init es un registro tipo Storage con el fin de poderlo editar por software, la distancia y donde se colocan como status, y las señales echo y trig se establecen como salidas de la FPGA.
``` python
class US(Module, AutoCSR):
    def __init__(self, echo, trig):
        self.clk = ClockSignal()
        self.init = CSRStorage(1)
        self.echo = echo
        self.trig = trig
        self.distance = CSRStatus(9)
        self.done = CSRStatus(1)
```


En el archivo buildSoCproject se crea el submodulo y se importan los pines de entrada y salida del modulo:

``` python
		#ultraSound
		SoCCore.add_csr(self,"ultraSound_cntrl")
		self.submodules.ultraSound_cntrl = ultraSound.US(platform.request("echo"), 	platform.request("trig"))
```

<a name="pwmus"></a>
# Servomotor del ultra sonido

Para este modulo se utiliza el servomotor sg90. Segun el [datasheet](../datasheets/sg90_datasheet.pdf) la señal pwm debe ser de 50 Hz, a partir de esto, la posicion a 0° debe tener un ancho de pulso de 1 ms, mientras que para 180° el ancho debe ser de 2 ms. Con esto en mente, se implementa el siguiente codigo en verilog:


En el caso de el servomotor que se utilizo, los anchos de pulso varian, por lo que los tiempos utilizados a 0° y 180° varian por 5 ms. Debido a esto, se establece un contador hasta un millon, cuando se llega a 50.000 se coloca el servo en 0°, cuando el contador llega a 150.000 se coloca en 90°, y cuando se llega a 250.000 se coloca en 180°.

``` verilog
always@(posedge clk)begin
	contador = contador + 1;
	if(contador =='d1_000_000) begin
	   contador = 0;
	end
	
	case(pos)
        2'b00:  servo = (contador < 'd50_000) ? 1:0;
        
        2'b01:  servo = (contador < 'd150_000) ? 1:0;
        
        2'b10:  servo = (contador < 'd250_000) ? 1:0;
        
        default:servo = (contador < 'd50_000) ? 1:0;
    endcase

end
```

En python se importa el modulo donde se maneja el registro pos como Storage para que se pueda editar desde C, y la señal servo como una conexion externa en la FPGA.

``` python
class servoUS(Module, AutoCSR):
    def __init__(self, servo):
            self.clk = ClockSignal()
            self.pos = CSRStorage(2)
            self.servo = servo

```

Luego se añade el submodulo y se importan los pines.

``` python
#PWMUS
		SoCCore.add_csr(self,"PWMUS_cntrl")
		self.submodules.PWMUS_cntrl = PWMUS.servoUS(platform.request("servo"))
```



<a name="ruedas"></a>
# Ruedas

Para las ruedas se utilizó un puente H (modelo L298N) conectado a los motores en vez de un PWM. 

![Screenshot](/images/ruedas1.png)

La conexión se realiza partiendo de los 4 pines de la fpga que se conectan a 4 puertos del puente H, este asigna 2 pines a la rueda izquierda y 2 pines a la rueda derecha; en cada una de las ruedas se asigna un pin para mover la rueda en una dirección (hacia adelante/sentido horario) y otro para moverlo en la dirección contraria (hacia atrás/sentido antihorario), estos pines son activados con señales en 1 y 0, por lo que no pueden estar ambos pines en 1 a la misma vez.

![Screenshot](/images/puenteh.png)

El módulo en verilog se conecta al puente H de la siguiente forma:

``` verilog

 // Recto
        3'b000:  begin
                    right[0] = 1;
                    right[1] = 0;
                    left[0]  = 1;
                    left[1]  = 0;
                end
```

Donde se indica a cuál dirección se moverá cada una de las ruedas (siendo right[0] hacia adelante y right[1] hacia atrás). En este caso se activa el movimiento de las mismas de acuerdo a como se quiera mover el robot.

Como segundo ejemplo se puede observar el código:

``` verilog
        // Giro 180
        3'b100:  begin
                    right[0] = 0;
                    right[1] = 1;
                    left[0]  = 1;
                    left[1]  = 0;
                end
```

Como se muestra, para girar 180 grados se activa la rueda derecha girando hacia atrás y la rueda izquierda girando hacia delante. Para escoger estos movimientos, se ingresa un estado que puede tomar valores del 0 al 5, con esto se escoge cuál rueda se activa y qué tipo de movimiento tiene. Los movimientos considerados son: Recto, derecha, izquierda, quieto, giro 180 y retroceder.

El código elaborado en python es el siguiente:


``` python

class wheels(Module, AutoCSR):
    def __init__(self, right, left):
            self.clk = ClockSignal()
            self.state = CSRStorage(3)
            self.right = right
            self.left = left

            self.specials += Instance("wheels",
                i_clk = self.clk,
                i_state = self.state.storage,
                o_right = self.right,
                o_left = self.left)

```

Se agrega la señal del reloj y la variable state, esta variable se define tipo storage para poder editarlo desde el código en C. Las señales right y left son las que se usan en los pines del puente H para controlar las ruedas. 

Para la implementación en el BuildSoCproject se elaboró el siguiente código:

``` python

SoCCore.add_csr(self,"wheels_cntrl")
right = Cat(*[platform.request("right", i) for i in range(2)])
left = Cat(*[platform.request("left", i) for i in range(2)])
self.submodules.wheels_cntrl = wheels.wheels(right, left)
```

Con esto se realiza un barrido de 1 a 2 en left y right debido a que cada uno tiene 2 pines, esto evita que se haga manualmente. En wheels_cntrl se definen los pines a usar, en este caso los elegidos anteriormente: right y left

En otras secciones de la documentación se puede conseguir la explicación del funcionamiento de las ruedas junto a los infrarrojos, mp3 y el ultrasonido.

<a name="blue"></a>
# Bluetooth

El SoC usado incluía el modulo UART por lo que solo debía integrarse el modulo al buildSoCproject con sus pines, de esta manera:

``` python
from litex.soc.cores import uart
		self.submodules.uart1_phy = uart.UARTPHY(
			pads     = platform.request("uart1"),
			clk_freq = self.sys_clk_freq,
			baudrate = 9600)
		self.submodules.uart1 = ResetInserter()(uart.UART(self.uart1_phy,
			tx_fifo_depth = 16,
			rx_fifo_depth = 16))
		self.csr.add("uart1_phy", use_loc_if_exists=True)
		self.csr.add("uart1", use_loc_if_exists=True)
		if hasattr(self.cpu, "interrupt"):
			self.irq.add("uart1", use_loc_if_exists=True)
		else:
			self.add_constant("UART_POLLING")

```

El modulo bluetooth utilizado fue el HC-06:

![Screenshot](/images/blue.jpg)

<a name="mp3"></a>
# mp3

De igual forma que el Bluetooth, para el mp3 se hizo uso de una uart. Solo se integra al buildSoCproject, así:

``` python
self.submodules.uart2_phy = uart.UARTPHY(
			pads     = platform.request("uart2"),
			clk_freq = self.sys_clk_freq,
			baudrate = 9600)
		self.submodules.uart2 = ResetInserter()(uart.UART(self.uart2_phy,
			tx_fifo_depth = 16,
			rx_fifo_depth = 16))
		self.csr.add("uart2_phy", use_loc_if_exists=True)
		self.csr.add("uart2", use_loc_if_exists=True)
		if hasattr(self.cpu, "interrupt"):
			self.irq.add("uart2", use_loc_if_exists=True)
		else:
			self.add_constant("UART_POLLING")

```

El módulo mp3 utilizado fue el DFPlayer mini:

![Screenshot](/images/mp3.jpg)


<a name="InfraRojo"></a>
# InfraRojo
Este dispositivo se basa en un fotodiodo emisor de luz y un fototransistor. Su funcionamiento consiste en la emision de luz por el fotodiodo por el cual al recibir esta energia en el fototransistor a traves de la base para permitir un flujo de corriente a traves del colector y el emisor. La luz no se puede reflejar a traves de una superficie negra, por lo que al incidir sobre esta no es posiblle generar la corriente en el colector, de modo que para obtener una señal activa es necesario invertir el funcionamiento de los infrarojos para que estos operenc bajo una superficie negra. 

![Screenshot](/images/facil.jpg)

Desde el hardwarelos infrarojos consta de cinco registros, en donde se encuentran dos infrarojos en los laterales del robot (iR y iL) y tres infrarojos en el centro que representan los bordes del centro para indicar que tan centrado se está desplazando el robot (iRC y ILC), y el infrarojo central que nos asegura un movimiento recto (iC).
Estos se encargan de actualizar estos registros y enviarlos al software siempre que reciban una señal proveniente de los infrarojos.

![Screenshot](/images/IR_V.JPG)

Para su implementación desde python se llamo a cada uno de estos registros para leer cada uno de estos a través del status y conocer la posicion en la que se encontraba el robot

![Screenshot](/images/ImplementacionIR.JPG)

<a name="Camara"></a>
#Cámara
La cámara utilizada fue la OV7670, esta cámara no tiene FIFO y fue necesario hacer uso de un arduino en modo maestro para para realizar la solicitud de los registros por medio de comunicación I2C esto se describe en el [documento](../datasheets/OV7670.pdf) del dispositivo con todos sus pines de entrada y salida, por motivo de la dificultad del modulo la descripción de hardware en su mayoría fue entregada en el paquete de trabajo 02 asignado por el profesor de la asignatura. Dicha descripción cuenta con los módulos buffer_ram_dp, VGA_driver, cam_read y test_cam siendo necesaria la modificación de los dos últimos para el funcionamiento de la cámara, adicionalmente se utiliza un modulo divisor de frecuencia de 24MHz para que pueda operar la cámara y 25MHz para la VGA.

![Screenshot](/images/OV7670.png)

Para que la cámara funcione es necesario las señales SDL y SDA las cuales no son mas que dos señales de reloj sincronizadas para hacer posible que el arduino que en este caso actúa como maestro le solicite los registros a la cámara, por medio de estos registros es que se realiza la configuración de la imagen. También es necesario la conexión de dos resistencias de 10KΩ en estos puertos para que funcione la correctamente, de lo contrario la cámara no dará ninguna imagen.

![Screenshot](/images/SCL_SDA.png)


![Screenshot](/images/Conexion.png)

Inicialmente se propone que el formato de la adquisición de datos sea RGB565 y por medio de la descripción de hardware se cambie a GB332, sin embargo se hace necesario cambiar a formato RGB444 para mayor facilidad a la hora de la salida de la imagen a la VGA.

![Screenshot](/images/RGB444.png)

El tamaño de la imagen propuesto fue de 160 * 120, aunque en el momento de realizar la integración en el SoC fue necesario disminuirlo a 100 * 75 por efectos de tamaño de memoria, esto quiere decir que la imagen que queremos obtener sera de 7500 pixeles o 90000 bits, para eso utilizamos la siguiente formula:

2^n = 7500

al resolverla tenemos:

log_2(7500) = 12.873

aproximando a 13, remplazando n por ese valor tenemos 2^13 = 8192, lo cual es suficiente para lo que necesitamos.

A continuación se muestra la descripción de hardware de los cuatro módulos:

### cam_read

Este modulo se encarga de recolectar los datos y enviarlos a la memoria.

``` verilog

module cam_read #(
		parameter AW = 13 // Cantidad de bits  de la dirección 
		)(
		input CAM_pclk,
		input rst,
		input CAM_vsync,
		input CAM_href,
		input [7:0] CAM_px_data,

		output reg[AW-1:0] DP_RAM_addr_in  ,
		output reg[11:0]  DP_RAM_data_in ,
		output reg DP_RAM_regW  
   );
	
	

/********************************************************************************
Por favor colocar en este archivo el desarrollo realizado por el grupo para la 
captura de datos de la camara 
debe tener en cuenta el nombre de las entradas  y salidad propuestas 
********************************************************************************/
reg [1:0]cs=0;// Actúa como el contador de case (para establecer los casos)
reg ovsync;// Utilizado para guardar el valor pasado de Vsync
reg bp=1'b0;
always @ (posedge CAM_pclk) begin// sentencias que se llevan a cabo siempre y cuando pclk se encuentre en un flanco de subida
	case (cs)//inicio de la máquina de estados
	0: begin// estado 0 de la máquina de estados cs=00
		if(ovsync && CAM_vsync)begin//rápidamente ovsync ha tomado el primer valor de vsync y procedemos a compararlos, con && garantizamos una comparación de tipo AND
		cs=1;// si ovsync y !vsync =1 entonces procedemos a pasar al case 1
		DP_RAM_addr_in=0;//iniciamos en la posición de memoria 0
		end
	end
	1: begin// primer estado, cs=01, en este caso hacemos la captura de los datos y procedemos a convertirlos a RGB 332
		DP_RAM_regW=0;// indicamos que aún no escribimos en la memoria
		if (CAM_href) begin//debemos asegurar que href se encuentre en flanco de subida para hacer el proceso
/****************************************************************
 En esta parte tomamos los datos más significativo de R(rojo) y V (Verde)
 del primer byte que vienen en formato 565(RGB) y lo guardamos en formato   
 332(RGB)         
******************************************************************/
				DP_RAM_data_in[11]=CAM_px_data[3];          
                		DP_RAM_data_in[10]=CAM_px_data[2]; 
                		DP_RAM_data_in[9]=CAM_px_data[1];          
                		DP_RAM_data_in[8]=CAM_px_data[0];
				        

				cs=2;// Después de tomar los datos más significativos pasamos al estado 2 
		end
	end
	2: begin// Estado 2, en este estado procedemos a tomar los datos del color azul(B) que vienen en formato 565 RGB y se pasa a 332 RGB
				DP_RAM_data_in[3]=CAM_px_data[3]; 
                		DP_RAM_data_in[2]=CAM_px_data[2];          
                		DP_RAM_data_in[1]=CAM_px_data[1]; 
				DP_RAM_data_in[0]=CAM_px_data[0];
				
				DP_RAM_data_in[7]=CAM_px_data[7];
				DP_RAM_data_in[6]=CAM_px_data[6];          
                		DP_RAM_data_in[5]=CAM_px_data[5]; 
                		DP_RAM_data_in[4]=CAM_px_data[4];
                
         

			 	DP_RAM_regW=1;//Procedemos a escribir en memoria
				DP_RAM_addr_in=DP_RAM_addr_in+1;//Nos desplazamos a la siguiente dirección de memoria
				cs=1;//Posteriormente volvemos al estado 1 de la máquina de estados 
		if(CAM_vsync) begin// Con este condicional analizamos si vsync está en un flanco de subida volvemos al estado 0
		cs=0;//volvemos al estado 0 de nuestra máquina de estados
		end		
		if (DP_RAM_addr_in==7500) begin//Limitador de memoria
			DP_RAM_addr_in=0;// Si la memoria  llega  a la posición de  19200 píxeles, debe volver a la posición 0 nuevamente
			cs=0;//Nos devolvemos al estado 0 a evaluar vsync
		end
		end
endcase
	ovsync<=CAM_vsync;// Se usa para que recurrentemente ovsync tome el valor pasado de vsync
end
endmodule

```

### test_cam

Este es el modulo top entregado al cual se le agrego el modulo cam_read y se realizaron algunas modificaciones para el funcionamiento.


``` verilog

module test_cam(
    input wire clk,           // board clock: 32 MHz quacho 100 MHz nexys4 
    input wire rst,         	// reset button

	// VGA input/output  
    output wire VGA_Hsync_n,  // horizontal sync output
    output wire VGA_Vsync_n,  // vertical sync output
    output wire [3:0] VGA_R,	// 4-bit VGA red output
    output wire [3:0] VGA_G,  // 4-bit VGA green output
    output wire [3:0] VGA_B,  // 4-bit VGA blue output

	//CAMARA input/output
	
	output wire CAM_xclk,		// System  clock imput
	output wire CAM_pwdn,		// power down mode 
	output wire CAM_reset,		// clear all registers of cam
	
	// colocar aqui las entras  y salidas de la camara  que hace falta

	input wire CAM_pclk,
	input wire CAM_vsync,
	input wire CAM_href,
	input wire [7:0] CAM_px_data


		
);

// TAMAÑO DE ADQUISICIÓN DE LA CAMARA 
parameter CAM_SCREEN_X = 100;
parameter CAM_SCREEN_Y = 75;

localparam AW = 14; // LOG2(CAM_SCREEN_X*CAM_SCREEN_Y)
localparam DW = 12;

// El color es RGB 332
localparam RED_VGA =   12'b111100000000;
localparam GREEN_VGA = 12'b000011110000;
localparam BLUE_VGA =  12'b000000001111;


// Clk 
//wire clk32M;
wire clk25M;
wire clk24M;

// Conexión dual por ram

wire  [AW-1: 0] DP_RAM_addr_in;  
wire  [DW-1: 0] DP_RAM_data_in;
wire DP_RAM_regW;

 
	
// Conexión VGA Driver
wire [DW-1:0]data_mem;	   // Salida de dp_ram al driver VGA
wire [DW-1:0]data_RGB332;  // salida del driver VGA al puerto
wire [9:0]VGA_posX;		   // Determinar la pos de memoria que viene del VGA
wire [8:0]VGA_posY;		   // Determinar la pos de memoria que viene del VGA

reg [AW-1: 0] DP_RAM_addr_out;
/* ****************************************************************************
la pantalla VGA es RGB 444, pero el almacenamiento en memoria se hace 332
por lo tanto, los bits menos significactivos deben ser cero
**************************************************************************** */
assign VGA_R = data_RGB332[11:8];
assign VGA_G = data_RGB332[7:4];
assign VGA_B = data_RGB332[3:0];



/* ****************************************************************************
Asignación de las señales de control xclk pwdn y reset de la camara 
**************************************************************************** */

assign CAM_xclk=  clk24M;
assign CAM_pwdn=  0;			// power down mode 
assign CAM_reset= 1;



/* ****************************************************************************
  Este bloque se debe modificar según sea le caso. El ejemplo esta dado para
  fpga Spartan6 lx9 a 32MHz.
  usar "tools -> Core Generator ..."  y general el ip con Clocking Wizard
  el bloque genera un reloj de 25Mhz usado para el VGA  y un relo de 24 MHz
  utilizado para la camara , a partir de una frecuencia de 32 Mhz
**************************************************************************** */
//assign clk32M =clk;
clk24_25_nexys4
  clk25_24(
  .CLK_IN1(clk),
  .CLK_OUT1(clk25M),
  .CLK_OUT2(clk24M),
  .RESET(rst)
 );


/* ****************************************************************************
buffer_ram_dp buffer memoria dual port y reloj de lectura y escritura separados
Se debe configurar AW  según los calculos realizados en el Wp01
se recomiendia dejar DW a 8, con el fin de optimizar recursos  y hacer RGB 332
**************************************************************************** */
buffer_ram_dp #( AW,DW)
	DP_RAM(  
	.clk_w(CAM_pclk), 
	.addr_in(DP_RAM_addr_in), 
	.data_in(DP_RAM_data_in),
	.regwrite(DP_RAM_regW), 
	
	.clk_r(clk25M), 
	.addr_out(DP_RAM_addr_out),
	.data_out(data_mem)
	);
	

/* ****************************************************************************
VGA_Driver640x480
**************************************************************************** */

VGA_Driver640x480 VGA640x480
(
	.rst(rst),
	.clk(clk25M), 				// 25MHz  para 60 hz de 640x480
	.pixelIn(data_mem), 		// entrada del valor de color  pixel RGB 332 
	.pixelOut(data_RGB332), // salida del valor pixel a la VGA 
	.Hsync_n(VGA_Hsync_n),	// señal de sincronizaciÓn en horizontal negada
	.Vsync_n(VGA_Vsync_n),	// señal de sincronizaciÓn en vertical negada 
	.posX(VGA_posX), 			// posición en horizontal del pixel siguiente
	.posY(VGA_posY) 			// posición en vertical  del pixel siguiente

);


/* ****************************************************************************
LÓgica para actualizar el pixel acorde con la buffer de memoria y el pixel de 
VGA si la imagen de la camara es menor que el display  VGA, los pixeles 
adicionales seran iguales al color del último pixel de memoria 
**************************************************************************** */

always @ (VGA_posX, VGA_posY) begin
		if ((VGA_posX>CAM_SCREEN_X-1) || (VGA_posY>CAM_SCREEN_Y-1))
			DP_RAM_addr_out=15'b111111111111111;
		else
	                DP_RAM_addr_out = VGA_posX + VGA_posY * CAM_SCREEN_X;
end


/*****************************************************************************


**************************************************************************** */
 cam_read #(AW)cam_read(
		.CAM_pclk(CAM_pclk),
		.rst(rst),
		.CAM_vsync(CAM_vsync),
		.CAM_href(CAM_href),
		.CAM_px_data(CAM_px_data),

		.DP_RAM_addr_in(DP_RAM_addr_in),
		.DP_RAM_data_in(DP_RAM_data_in),
		.DP_RAM_regW(DP_RAM_regW)
   );
endmodule

```

### buffer_ram_dp

Este modulo describe el dual port de la memoria ram la cual recibe los datos de la cámara.

``` verilog

module buffer_ram_dp#( 
	parameter AW = 14, // Cantidad de bits  de la direcci�n 
	parameter DW = 12, // cantidad de Bits de los datos 
	parameter   imageFILE= "/home/juan/Documentos/Digital_II/Nexys4/Proyectos/Camara/wp02_drivercamara-grupo11/src/image.men")
	(  
	input  clk_w, 
	input  [AW-1: 0] addr_in, 
	input  [DW-1: 0] data_in,
	input  regwrite, 
	
	input  clk_r, 
	input [AW-1: 0] addr_out,
	output reg [DW-1: 0] data_out
	);

// Calcular el número de posiciones totales de memoria 
localparam NPOS = 2 ** AW; // Memoria

 reg [DW-1: 0] ram [0: NPOS-1]; 


//	 escritura  de la memoria port 1 
always @(posedge clk_w) begin 
       if (regwrite == 1) 
             ram[addr_in] <= data_in;
end

//	 Lectura  de la memoria port 2 
always @(posedge clk_r) begin 
		data_out <= ram[addr_out]; 
end


initial begin
	$readmemh(imageFILE, ram);
	ram[14'b11111111111111]=12'b000000000000;	
end

endmodule

```

### VGA_driver

Esta diseñado para que la transmisión de datos no sea superior a RGB444 por este motivo se elige este formato.

``` verilog

module VGA_Driver640x480 (
	input rst,
	input clk, 				// 25MHz  para 60 hz de 640x480
	input  [7:0] pixelIn, 	// entrada del valor de color  pixel 
	
	output  [7:0] pixelOut, // salida del valor pixel a la VGA 
	output  Hsync_n,		// señal de sincronización en horizontal negada
	output  Vsync_n,		// señal de sincronización en vertical negada 
	output  [9:0] posX, 	// posicion en horizontal del pixel siguiente
	output  [8:0] posY 		// posicion en vertical  del pixel siguiente
);

localparam SCREEN_X = 640; 	// tama�o de la pantalla visible en horizontal 
localparam FRONT_PORCH_X =16;  
localparam SYNC_PULSE_X = 96;
localparam BACK_PORCH_X = 28;
localparam TOTAL_SCREEN_X = SCREEN_X+FRONT_PORCH_X+SYNC_PULSE_X+BACK_PORCH_X; 	// total pixel pantalla en horizontal 


localparam SCREEN_Y = 480; 	// tama�o de la pantalla visible en Vertical 
localparam FRONT_PORCH_Y =10;  
localparam SYNC_PULSE_Y = 2;
localparam BACK_PORCH_Y = 33;
localparam TOTAL_SCREEN_Y = SCREEN_Y+FRONT_PORCH_Y+SYNC_PULSE_Y+BACK_PORCH_Y; 	// total pixel pantalla en Vertical 


reg  [9:0] countX=SCREEN_X; // no se deja en cero para hacer mas rapida la simulaci�n
reg  [8:0] countY=SCREEN_Y; // no se deja en cero para hacer mas rapida la simulaci�n

assign posX    = countX;
assign posY    = countY;

assign pixelOut = (countX<SCREEN_X) ? (pixelIn ) : (12'b000000000000) ;

assign Hsync_n = ~((countX>=SCREEN_X+FRONT_PORCH_X) && (countX<SCREEN_X+SYNC_PULSE_X+FRONT_PORCH_X)); 
assign Vsync_n = ~((countY>=SCREEN_Y+FRONT_PORCH_Y) && (countY<SCREEN_Y+FRONT_PORCH_Y+SYNC_PULSE_Y));


always @(posedge clk) begin
	if (rst) begin
		countX <= SCREEN_X; // no se deja en cero para hacer mas rapida la simulaci�n
		countY <= SCREEN_Y; // no se deja en cero para hacer mas rapida la simulaci�n
	end
	else begin 
		if (countX >= (TOTAL_SCREEN_X-1)) begin
			countX <= 0;
			if (countY >= (TOTAL_SCREEN_Y-1)) begin
				countY <= 0;
			end 
			else begin
				countY <= countY + 1;
			end
		end 
		else begin
			countX <= countX + 1;
			countY <= countY;
		end
	end
end

endmodule

```
se crea el archivo camara.py la cual describe las entradas y salidas utilizadas por la cámara para realizar la correcta integración del modulo en el SoC, a continuación el código:


``` python

from migen import *
from migen.genlib.cdc import MultiReg
from litex.soc.interconnect.csr import *
from litex.soc.interconnect.csr_eventmanager import *

# Modulo Principal
class Camara(Module,AutoCSR):
    def __init__(self, CAM_xclk,CAM_pclk,cam_data_in, CAM_vsync, CAM_href):
        self.clk = ClockSignal()   
        #self.rst = ResetSignal()  
        self.CAM_xclk = CAM_xclk
        
        self.CAM_pclk = CAM_pclk
        self.CAM_vsync= CAM_vsync
        self.CAM_href= CAM_href
        self.CAM_px_data = cam_data_in

        self.data_mem = CSRStatus(12)
        self.DP_RAM_addr_out = CSRStorage(14)

        self.specials +=Instance("test_cam",
            i_clk = self.clk,
            #i_rst = self.rst,
            o_CAM_xclk = self.CAM_xclk,
            i_CAM_pclk = self.CAM_pclk,
            i_CAM_px_data=self.CAM_px_data,
            o_data_mem=self.data_mem.status,
            o_CAM_vsync= self.CAM_vsync,
            o_CAM_href= self.CAM_href,
            i_DP_RAM_addr_out= self.DP_RAM_addr_out.storage)

```

