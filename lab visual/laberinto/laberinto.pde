import processing.serial.*;

//Se inicia la comunicacion serial
Serial port;
String string;
int line = 0;

//Se inicializa la ventana
void setup(){
  background(0, 0, 0);
  port = new Serial(this,Serial.list()[2],9600);
  size(500, 500);
}

//Se lee cada linea recibida por bluetooth
//Cuando un valor en la matriz es 1 se pinta un cuadrado rojo
//De lo contrario se pinta negro
void draw(){
  if(line >= 10)
    line = 0;
  while (port.available() > 0) {
    String inBuffer = port.readStringUntil('\n');   
    if (inBuffer != null) {
      if(line > 1){
        for(int i=0;i<inBuffer.length();i++){
          if(inBuffer.charAt(i) == '1'){
            fill(255, 0, 0);
          }else{
            fill(0, 0, 0);
          }
          square(50*i, 50*(line-2), 50);
          
        }
      }
      line = line + 1;
    }
  }
}
