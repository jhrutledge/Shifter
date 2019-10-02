/*
 * a() b() c() d() e() f() g() DP()
 * set brightness with resistor value
 * 1st = b,c
 * 2nd = a,b,d,e,g
 * 3rd = a,b,c,d,g
 * 4th = b,c,f,g
 * TC = DP
 */

// PINOUT
 const int sevSeg[] = {0,1,2,3,4,5,6,9}; //{a,b,c,d,e,f,g,DP}; CC to groud
 const int solen[] = {7,10,11}; //{sol1,sol2,sol3}; (inversed by relay board)
 const int upPin = A1;       //
 const int dnPin = A2;       //
 const int brkPin = A3;      //
 const int tpsPin = A4;      // analog
 //const in spdPin = A2;      // analog

// VARS
 int upState = 0;
 int lastUp = 0;
 int dnState = 0;
 int lastDn = 0;
 int brkState = 0;
 int lastBrk = 0;
 int tpsValue = 0;
 int lastTC = 0;
 int curGear = 1; 
 int delTime = 100;
 int tpsThresh = 300;
 unsigned long TCtime = 0;
 unsigned long TCdelay = 3000;

typedef struct {
  int disp[8];   // high and low for full display
  int sol[3];    // high and low for 2 solenoids
} gear;
gear gearArr[] {
  {{0,0,0,0,0,0,0,0},{1,1,1}},
  {{LOW,HIGH,HIGH,LOW,LOW,LOW,LOW,LOW},{LOW,HIGH,HIGH}},
  {{HIGH,HIGH,LOW,HIGH,HIGH,LOW,HIGH,LOW},{LOW,LOW,HIGH}},
  {{HIGH,HIGH,HIGH,HIGH,LOW,LOW,HIGH,LOW},{HIGH,LOW,HIGH}},
  {{LOW,HIGH,HIGH,LOW,LOW,HIGH,HIGH,LOW},{HIGH,HIGH,HIGH}}
};

void setup() {
  //inputs
  pinMode(upPin,INPUT);
  pinMode(dnPin,INPUT);
  pinMode(brkPin,INPUT);
  //outputs
  for (int i=0 ; i<8 ; i++) {
    pinMode(sevSeg[i],OUTPUT);
  }
  for (int i=0 ; i<3 ; i++) {
    pinMode(solen[i],OUTPUT);
  }
  //initialize first gear
  setSolen(gearArr[1]);
  setDisp(gearArr[1]);
  curGear = 1;
  digitalWrite(solen[2],HIGH);
  digitalWrite(sevSeg[7],LOW);
  lastTC = LOW;
  delay(delTime);
}

void loop() {
  // collect switch states
  upState = digitalRead(upPin);
  dnState = digitalRead(dnPin);
  brkState = digitalRead(brkPin);
  tpsValue = analogRead(tpsPin);
  // test up switch
  if(upState != lastUp) {
    if(upState == HIGH) {
      upGear(curGear);
    }
  }
  lastUp = upState;
  // test down switch
  if(dnState != lastDn) {
    if(dnState == HIGH) {
      dnGear(curGear);
    }
  }
  lastDn = dnState;
  // test brake switch
  if(brkState != lastBrk) {
    if(brkState == HIGH) {
      if(lastTC == HIGH) {
        digitalWrite(solen[2],HIGH);
        digitalWrite(sevSeg[7],LOW);
        lastTC = LOW;
        delay(delTime);
      }
    }
  }
  // use tps for TC lock in first
  if(curGear == 1) {
    if(tpsValue < tpsThresh) {
      TCtime = millis();
    }
    if(millis() - TCtime > TCdelay) {
      digitalWrite(solen[2],LOW);
      digitalWrite(sevSeg[7],HIGH);
      lastTC = HIGH;
    }
  }
}

void upGear(int curr) {
  if (curr > 3 || curr < 1) {
    return;
  }
  //step1 - TC delay
  if(lastTC != LOW) {
    digitalWrite(solen[2],HIGH);
    digitalWrite(sevSeg[7],LOW);
    lastTC = LOW;
    delay(delTime);
  }
  //step2 - set gear & display
  curr++;
  setSolen(gearArr[curr]);
  setDisp(gearArr[curr]);
  delay(delTime);
  //step3 - set TC
  curGear = curr;
  digitalWrite(solen[2],LOW);
  digitalWrite(sevSeg[7],HIGH);
  lastTC = HIGH;
}
void dnGear(int curr) {
  if (curr > 4 || curr < 2) {
    return;
  }
  //step1 - TC delay
  if(lastTC != LOW) {
    digitalWrite(solen[2],HIGH);
    digitalWrite(sevSeg[7],LOW);
    lastTC = LOW;
    delay(delTime);
  }
  //step2 - set gear & display
  curr--;
  setSolen(gearArr[curr]);
  setDisp(gearArr[curr]);
  delay(delTime);
  //step3 - set TC
  curGear = curr;
  digitalWrite(solen[2],LOW);
  digitalWrite(sevSeg[7],HIGH);
  lastTC = HIGH;
}

void setSolen(gear nt) {
  for (int i=0; i<2 ; i++) {
    digitalWrite(solen[i],nt.sol[i]);
  }
}

void setDisp(gear nt) {
  for (int i=0 ; i<7 ; i++) {
    digitalWrite(sevSeg[i],nt.disp[i]);
  }
}


