import processing.serial.*;

Serial port;
String string;
int line = 0;

void setup(){
  background(0, 0, 0);
  port = new Serial(this,Serial.list()[2],9600);
  size(500, 500);
}

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
      println(inBuffer);
      line = line + 1;
    }
  }
}
