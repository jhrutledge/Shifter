#include <SevSeg.h>

SevSeg disp;

void setup() {
  byte numDigits = 1;
  byte digitPins[] = {4};
  byte segmentPins[] = {5, 6, 7, 8, 9, 10, 11, 12};
  disp.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);

  //
  //const int numofDigits = 1;
  //int digitPins [numofDigits] = {4};
  //(5, 6, 7, 8, 9, 10, 11, 12)
  //
  
  Serial.begin(9600);
  int number = 0;
  int flag;
  disp.setBrightness(5);

}

void loop() {
  int number = 0;
  int flag;
  disp.setBrightness(5);

  if (Serial.available() > 0) {
    number = Serial.parseInt();
    flag = 0;
  }
  if (number >= 1 && number <= 9) {
    disp.setNumber(number, 0);
    if (flag == 0) {
      Serial.print("Number on 7 segmet display:");
      Serial.println(number);
      flag = 1;
    }
  }
  disp.refreshDisplay();
}
