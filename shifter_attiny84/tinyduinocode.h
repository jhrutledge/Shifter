/*
 * a() b() c() d() e() f() g() DP()
 * set brightness with resistor value
 * 1st = b,c    [001]
 * 2nd = a,b,d,e,g  [010]
 * 3rd = a,b,c,d,g  [011]
 * 4th = b,c,f,g  [100]
 * TC = DP
 */

// PINOUT
const int bcdPins[] = {2,3,4};  // (4,2,1)
const int solPins[] = {5,6};   // (sol1,sol2)
const int TCPins[] = {8,7}; // (DP,sol3)
const int upPin = A1;         //
const int dnPin = A2;       //
const int brkPin = A3;      //
const int dPin = A5;

// gear datastructure
struct gearstruc{
  int bcd[3]; // states for BCD display
  int sol[2]; // states for solenoids
};
typedef struct gearstruc gear;

// button datastructure
struct buttstruc{
  int pin;
  int currState;
  int butState;
  int lastState;
  unsigned long lastDebounce;
};
typedef struct buttstruc button;

// VARS 
 int currGear = 1;
 unsigned long debounceDelay = 20;
 int dState = LOW;
 int TCState = LOW;
 int delTime = 100;
 unsigned long TCtime = 0;
 unsigned long TCdelay = 3000;
 int PWMduty = 175;
 
 const gear g0 = {{LOW,LOW,LOW},{LOW,LOW}};     // 0 no gear
 const gear g1 = {{LOW,LOW,HIGH},{HIGH,LOW}};   // 1st
 const gear g2 = {{LOW,HIGH,LOW},{HIGH,HIGH}};    // 2nd
 const gear g3 = {{LOW,HIGH,HIGH},{LOW,HIGH}};    // 3rd
 const gear g4 = {{HIGH,LOW,LOW},{LOW,LOW}};    // 4th
 const gear gears[] = { g0, g1, g2, g3, g4 };

 button upButton {upPin,LOW,LOW,LOW,0};
 button dnButton {dnPin,LOW,LOW,LOW,0};
 button brkButton {brkPin,LOW,LOW,LOW,0};

// process button noise to define rising edge change
void readButton(button &curr){
  // read the state of the switch into a local variable:
  int reading = digitalRead(curr.pin);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:
  // If the switch changed, due to noise or pressing:
  if (reading != curr.lastState) {
    // reset the debouncing timer
    curr.lastDebounce = millis();
  }
  if ((millis() - curr.lastDebounce) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    // if the button state has changed:
    if (reading != curr.butState) {
      // only toggle the LED if the new button state is HIGH
      curr.butState = reading;
      if (curr.butState == HIGH) {
        curr.currState = reading;
      }
    }
  }
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  curr.lastState = reading;
}
// set solenoid control pins to specified states
void setSolen(int states[]) {
  for (int i=0 ; i<2 ; i++) {
    digitalWrite(solPins[i],states[i]);
  }
}
// set specified pins to PWM
void setPWM(int states[]) {
  for (int i=0 ; i<2 ; i++) {
  if(states[i] == HIGH) {
    analogWrite(solPins[i],PWMduty);
  } else {
    digitalWrite(solPins[i],LOW);
  }
  }
}
// set bcd control pins to specified states
void setDisp(int states[]) {
  for (int i=0 ; i<3 ; i++) {
    digitalWrite(bcdPins[i],states[i]);
  }
}
// set TC solenoid and display to specified state
void TCLock(int state) {
  for (int i=0 ; i<2 ; i++) {
    digitalWrite(TCPins[i],state);
  }
  TCState = state;
}
// valid upshift request
void upGear(int curr) {
  if (curr > 3 || curr < 1) {
    return;
  }
  // starts at state X
  // state Xu
  TCLock(LOW);
  setPWM(gears[curr].sol);
  delay(delTime);
  
  // increment gear
  curr++;
  // state X+1d
  setPWM(gears[curr].sol);
  setDisp(gears[curr].bcd);
  delay(delTime);
  
  // state X+1
  setSolen(gears[curr].sol);
  TCLock(HIGH);
  // replace current gear
  currGear = curr;
}
// valid downshift request
void dnGear(int curr) {
  if (curr > 4 || curr < 2) {
    return;
  }
  // starts at state X
  // state Xd
  TCLock(LOW);
  setPWM(gears[curr].sol);
  delay(delTime);
  
  // decrement gear
  curr--;
  // state X-1u
  setPWM(gears[curr].sol);
  setDisp(gears[curr].bcd);
  delay(delTime);
  
  // state X-1
  setSolen(gears[curr].sol);
  TCLock(HIGH);
  // replace current gear
  currGear = curr;
}

// define pins and initialize if in Drive (D high)
void setup() {
  //inputs
  pinMode(dPin,INPUT);
  pinMode(upPin,INPUT);
  pinMode(dnPin,INPUT);
  pinMode(brkPin,INPUT);
  //outputs
  for (int i=0 ; i<3 ; i++) {
    pinMode(bcdPins[i],OUTPUT);
  }
  for (int i=0 ; i<2 ; i++) {
    pinMode(solPins[i],OUTPUT);
    pinMode(TCPins[i],OUTPUT);
  }
  //initialize first gear if in Drive
  dState = digitalRead(dPin);
  if (dState == HIGH){
    setSolen(gears[1].sol);
    setDisp(gears[1].bcd);
    currGear = 1;
  } else {
    setSolen(gears[0].sol);
    setDisp(gears[0].bcd);
    currGear = 0;
  }
}

void loop() {
  // collect switch states
  dState = digitalRead(dPin);
  readButton(upButton);
  readButton(dnButton);
  readButton(brkButton);

  // test enable (D)
  if (dState == HIGH) {
    if (currGear <= 0) {
      setSolen(gears[1].sol);
      setDisp(gears[1].bcd);
      currGear = 1;
    }
    // test up button
    if(upButton.currState != upButton.lastState) {
      if(upButton.currState == HIGH) {
        if(TCState == HIGH) {
          upGear(currGear);
        } else {
          TCLock(HIGH);
        }
      }
    }
    upButton.lastState = upButton.currState;
    // test down button
    if(dnButton.currState != dnButton.lastState) {
      if(dnButton.currState == HIGH) {
        dnGear(currGear);
      }
    }
    dnButton.lastState = dnButton.currState;
    // test brake switch
    if(brkButton.currState != brkButton.lastState) {
      if(brkButton.currState == HIGH) {
        TCLock(LOW);
      }
    }
    brkButton.lastState = brkButton.currState;
  }
}