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
