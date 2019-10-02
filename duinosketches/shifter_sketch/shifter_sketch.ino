#include <SevSeg.h>

SevSeg disp;                                 //display library


  

void setup() {

  int upPin = 2;                         //pin for pos inc
  int downPin = 3;                       //pin for neg inc
  int upState = 0;                             //pos state
  int downState = 0;                           //neg state
  int gear = 0;                                //number shown
  
  byte numDigits = 1;                        //digits on display
  byte digitPins[] = {8};                    //pin for cathode
  byte segmentPins[] = {0, 1, 2, 3, 4, 5, 6, 7};
  disp.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);
  pinMode(upPin, INPUT);                     //setup pin to input
  pinMode(downPin,INPUT);                    //setup pin to input
  disp.setBrightness(10);                    //set screen brightness
}

void loop() {
  int flag;                                  //initialize flag
  upState = digitalRead(upPin);              //read state of pos
  downState = digitalRead(downPin);          //read state of neg

    if (gear < 4 && gear > 0) {
      if (upState == HIGH) {                 //pos inc
        gear+=1;
        flag = 0;
        Serial.println(gear + " " + flag);   //test
      }
      else if (downState == HIGH) {          //neg inc
        gear-=1;
        flag = 0;
        Serial.println(gear + " " + flag);   //test
      }
    }
    else if (gear == 4 && downState == HIGH) {
      gear-=1;                               //neg inc
      flag = 0;
      Serial.println(gear + " " + flag);     //test
    }
    else if (gear == 0 && upState == HIGH) {
      gear+=1;                               //pos inc
      flag = 0;
      Serial.println(gear + " " + flag);     //test
    }
  
  if (gear >= 0 && gear <= 4) {
    disp.setNumber(gear, 0);                 //display gear number
    if (flag == 0) {
      Serial.print("Number on display: ");   
      Serial.println(gear);                  //test
      flag = 1;
    }
  }
  disp.refreshDisplay();                     //refresh
}
