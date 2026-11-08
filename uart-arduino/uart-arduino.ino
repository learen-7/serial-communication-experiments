#include "Arduino_LED_Matrix.h"

#include "frames.h"

ArduinoLEDMatrix matrix;

void setup() {
  Serial.begin(115200);
  matrix.begin();
  matrix.renderBitmap(frameHappy, 8, 12);
}

void loop() {
  
  matrix.renderBitmap(frameHappy, 8, 12); // waiting display
  
  if(Serial.available()){
    int n=Serial.parseInt();
    if(n>=0 && n<=9){
      matrix.renderBitmap(digitFrames[n], 8, 12); // display digit n on the LED matrix
      delay(1000);
    } else {
      matrix.renderBitmap(frameSad, 8, 12);
      delay(1000);
    }
 }
}
