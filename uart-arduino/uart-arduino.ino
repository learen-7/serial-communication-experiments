#include "Arduino_LED_Matrix.h"

#include "frames.h"

ArduinoLEDMatrix matrix;
unsigned long displayTime;

int number = -1;
int numberQueue[32];
int numberHead = 0;
int numberTail = 0;
int numberCount = 0;

void setup() {
  Serial.begin(115200);
  matrix.begin();
  matrix.renderBitmap(frameHappy, 8, 12);
  displayTime = millis();
}

void loop() {

  while (Serial.available()){
    char ascii_code = Serial.read();
    if (ascii_code == '\n' && number != -1){
      if (numberCount < 32){
        numberQueue[numberTail] = number; 
        numberTail = (numberTail+1)%32; 
        numberCount++;
        number = -1;
      } else {
        Serial.println  ("Queue full, wait a little !");
        number = -1;
      }
    } else if (ascii_code>='0' && ascii_code <= '9' && number == -1){
      number = ascii_code - '0';
    } else {
      matrix.renderBitmap(frameSad, 8, 12);
      displayTime = millis();
      number = -1;
      Serial.println  ("Send a number between 0 and 9 !");
    }
  }

  if (millis() - displayTime >= 1000){
    if (numberCount > 0){
      int n = numberQueue[numberHead];
      numberHead = (numberHead + 1)%32;
      numberCount--;
      matrix.renderBitmap(digitFrames[n], 8, 12); // display digit n on the LED matrix
      displayTime = millis();
    } else {
      matrix.renderBitmap(frameHappy, 8, 12); // waiting display
      displayTime = millis();
    }
  }
  
}
