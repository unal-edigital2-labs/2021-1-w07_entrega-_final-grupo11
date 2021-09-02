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

## SoC

![Screenshot](/images/SoCMem.png)

## [ Mapa de Memoria ](https://github.com/unal-edigital2/w07_entrega-_final-grupo11/tree/main/module)

En la presente sección se encuentran los diferentes perífericos que se usaron para la elaboración del robot cartógrafo junto con sus respectivos espacios en memoria que fueron utilizados y como cada uno de estos se creo en hardware y del mismo modo se implemento.

## [ Firmware ](https://github.com/unal-edigital2/w07_entrega-_final-grupo11/tree/main/firmware)

En esta seccion se encuentra como se realizo el procesamiento desde software, realizando el codigo para las respectivas pruebas de cada periférico y como se ejecuto para su funcionamiento completo.

## Construcción

Para la construcción del carrito se utilizaron los siguientes materiales:
1. Placa de MDF de 15cmx20cm
2. 2 motorreductores junto con sus llantas
3. 1 rueda loca
4. 1 servomoto
5. 1 ultrasonido HC-SR04
6. 5 sensores infrarrojos con su driver
7. 1 puente H L298N
8. 1 Bluetooth HC-06
9. 1 Protoboard
10. 1 MP3 TF16P

Bosquejo del carrito:

![Screenshot](/images/plano.PNG)


![Screenshot](/images/Carrito.jpeg)

Para el laberinto se estableció un patron con cinta aislante, en cada linea perpendicular al camino principal el carrito se detiene y mide las distancias a su alrededor. Las paredes del laberinto tienen una separación aproximada de 40cm, el grosor de la cinta aislante es de aproximadamente 4cm, y las lineas que cortan el camino son de aproximadamente 25cm de largo:

![Screenshot](/images/Laberinto.PNG)

![Screenshot](/images/lab.jpeg)

## Pruebas funcionales

<a href="http://www.youtube.com/watch?feature=player_embedded&v=XLlC-236xpk
" target="_blank"><img src="http://img.youtube.com/vi/XLlC-236xpk/0.jpg" 
alt="(http://img.youtube.com/vi/XLlC-236xpk/0.jpg)" width="400"/></a>

