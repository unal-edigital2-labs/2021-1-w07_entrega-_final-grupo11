1. [ Ultra Sonido. ](#us)
2. [ Servomotor del ultra sonido. ](#pwmus)
3. [ Ruedas. ](#ruedas)
4. [ Bluetooth. ](#blue)
5. [ mp3. ](#mp3)
6. [ InfraRojo ](#InfraRojo)

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

![Screenshot](/images/ImplementacioIR.jpg)

