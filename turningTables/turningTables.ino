/*
turningTables | Model Railway Turntable Indexing System
- Compatible with Atlas and other turntable brands
- Interfaces with JMRI by posing as a C/MRI system
- C/MRI Interface courtesy of ArduinoCMRI library
- Controlled as a series of turnouts, like a Walthers or other comparable turntable
Author: k4kfh
Project Website: https://github.com/k4kfh/turningTables
*/
#include <CMRI.h>
#include <EEPROM.h>

#define num_tracks 24
#define motor_cw_pin 11
#define motor_ccw_pin 10
#define piezo_pin 9
#define service_btn_cw_pin 3
#define service_btn_ccw_pin 4
#define hall_pin 2 //make sure this pin is interrupt-capable
const int turnsPerRotation = 5; //this is 5 on Atlas turntable
#define normal_motor_speed 255 //this is the PWM number the motor will turn at, if on a PWM-capable pin

unsigned total_gearTurns = 0;
int rotation_gearTurns = 0; //this will be between 1 and 5 always
int currentPosition = 0;
int buttonState_cw = 0;
int lastButtonState_cw = 0;
int buttonState_ccw = 0;
int lastButtonState_ccw = 0;
int mode = 0;
int motorStartMillis = 0; //used in the time-based reliability enhancements
int lastBitStates[48];

//setting up CMRI class
CMRI cmri;

void setup() {
  //set all the pins to the correct mode
  pinMode(motor_cw_pin, OUTPUT);
  pinMode(motor_ccw_pin, OUTPUT);
  pinMode(hall_pin, INPUT);
  pinMode(service_btn_cw_pin, INPUT_PULLUP);
  pinMode(service_btn_ccw_pin, INPUT_PULLUP);

  //read EEPROM for last position
  EEPROM.get(0, currentPosition);

  //making hall effect sensor interrupt
  attachInterrupt(digitalPinToInterrupt(hall_pin), hallTrigger, FALLING);

  //start Serial connection
  Serial.begin(9600, SERIAL_8N2);

  //set last bit states initially
  for (int i=0;i < 47;i++) {
    lastBitStates[i] = cmri.get_bit(i);
  }

  //play startup noise
  startup_beep(); startup_beep();
}

void loop() {
  //This code handles reading the alignment buttons. The buttons move the turntable not one track position, but one rotation of the measured gear, which is useful if the mechanism gets a few turns out of alignment.
  buttonState_cw = digitalRead(service_btn_cw_pin);
  if (buttonState_cw != lastButtonState_cw) {
    if (buttonState_cw == LOW) {
      click_cw();
    }
  }
  lastButtonState_cw = buttonState_cw;

  buttonState_ccw = digitalRead(service_btn_ccw_pin);
  if (buttonState_ccw != lastButtonState_ccw) {
    if (buttonState_ccw == LOW) {
      click_ccw();
    }
  }
  lastButtonState_ccw = buttonState_ccw;
  //end button handling code

  //CMRI interface code
  cmri.process();

  //all the bits for the stalls
  for (int i=0; i < num_tracks; i++) {
    int currentBit = cmri.get_bit(i);
    if (currentBit == 1) {
      if (currentBit != lastBitStates[i]) {
        rotate(i,0);
      }
    }
  }

  if (cmri.get_bit(25) == 1) {
    if (cmri.get_bit(25) != lastBitStates[25]) {
      turn_ccw(12);
    }
  }

  //set last bit states at the end of the "cycle"
  for (int i=0;i < 47;i++) {
    lastBitStates[i] = cmri.get_bit(i);
  }
}

//simple function to run on hall effect sensor interrupt
void hallTrigger() {
  int currentMillis = millis();
  int timeSinceStart = currentMillis - motorStartMillis;
  if (timeSinceStart > 400) {
    total_gearTurns++;
    //code to increment rotation_gearTurns
    if (rotation_gearTurns < turnsPerRotation) {
      rotation_gearTurns++;
    }
    else {
      rotation_gearTurns = 1; //at first glance it seems this should be zero. It shouldn't.
    }
  }
}

void motor_cw() {
  //Swap the lines here if your motor pin is not PWM-capable.
  analogWrite(motor_cw_pin, normal_motor_speed);
  //digitalWrite(motor_cw_pin, HIGH);
}

void motor_ccw() {
  //Swap these lines if your motor pin is not PWM-capable.
  analogWrite(motor_ccw_pin, normal_motor_speed);
  //digitalWrite(motor_ccw_pin, HIGH);
}

void motor_stop() {
  digitalWrite(motor_ccw_pin, LOW);
  digitalWrite(motor_cw_pin, LOW);
}

//Turns the turntable clockwise by moves number of tracks
void turn_cw(int moves) {
  motorStartMillis = millis();
  for (int i=0; i < moves; i++) {
    //Turns the motor until the gear has rotated turnsPerRotation
    while (rotation_gearTurns < turnsPerRotation) {
      motor_cw();
    }
    rotation_gearTurns = 0; //this seems out of place, but it must be set otherwise the above WHILE loop can only run once
  }
  motor_stop();
}

//Turns the turntable counterclockwise by moves number of tracks
void turn_ccw(int moves) {
  motorStartMillis = millis();
  for (int i=0; i < moves; i++) {
    //Turns the motor until the gear has rotated turnsPerRotation
    while (rotation_gearTurns < turnsPerRotation) {
      motor_ccw();
    }
    rotation_gearTurns = 0; //this seems out of place, but it must be set otherwise the above WHILE loop can only run once
  }
  motor_stop();
}


//Rotates the turntable to TRACK in MODE (mode can be 0, 1, 2, or 3, for auto-direction, clockwise, counterclockwise, or forced 180)
void rotate(int track, int mode) {
  int movesNeeded;
  if (mode == 0) {
    //automatic direction choice
    int actualDirection = chooseDirection(currentPosition, track, num_tracks);
    if (actualDirection == 1) {
      if ((track - currentPosition) < 0) {
        //if we cross over zero in order to get that
        movesNeeded = (track + num_tracks) - currentPosition;
        turn_cw(movesNeeded);
      }
      else {
        //if we don't have to cross zero
        movesNeeded = track - currentPosition;
        turn_cw(movesNeeded);
      }
    }
    else if (actualDirection == 2) {
      //counterclockwise
      if ((currentPosition - track) < 0) {
        //if we cross over zero in order to get that
        movesNeeded = (currentPosition + num_tracks) - track;
        turn_ccw(movesNeeded);
      }
      else {
        //if we don't have to cross zero
        movesNeeded = currentPosition - track;
        turn_ccw(movesNeeded);
      }
    }
  }
  if (mode == 1) {
    //forced clockwise
    if ((track - currentPosition) < 0) {
       //if we cross over zero in order to get that
       movesNeeded = (track + num_tracks) - currentPosition;
       turn_cw(movesNeeded);
    }
    else {
        //if we don't have to cross zero
        movesNeeded = track - currentPosition;
        turn_cw(movesNeeded);
      }
  }
  if (mode == 2) {
    //forced counterclockwise
    if ((currentPosition - track) < 0) {
        //if we cross over zero in order to get that
        movesNeeded = (currentPosition + num_tracks) - track;
        turn_ccw(movesNeeded);
      }
      else {
        //if we don't have to cross zero
        movesNeeded = currentPosition - track;
        turn_ccw(movesNeeded);
      }
  }
  if (mode == 3) {
    //rotate 180 degrees
    turn_cw((num_tracks/2));
  }

  EEPROM.put(0, track); //writes the current position to the EEPROM. This means that the Arduino will know where it is even if the power is cut ungracefully
  currentPosition = track;
}


//Used by rotate()'s inner workings to figure out the most efficient direction to move in order to get to a certain track from our current position
int chooseDirection(int currentPosition, int goingTo, int maxPosition) {
  int directionToGo; //this will be defined later, just go ahead and declare it
  int specialNum = maxPosition / 2;
  if(currentPosition < goingTo) {
    if(abs(currentPosition - goingTo) < specialNum) {
       //CLOCKWISE
       directionToGo = 1;
    }
    else {
        //COUNTERCLOCKWISE
        directionToGo = 2;
    }
  }

  else {
    if(abs(currentPosition - goingTo) < specialNum) {
       //COUNTERCLOCKWISE
       directionToGo = 2;
    }
    else {
        //CLOCKWISE
        directionToGo = 1;
    }
  }
  return directionToGo;
 }

 void prepToShutdown() {
  tone(piezo_pin, 500);
  delay(200);
  noTone(piezo_pin);
  delay(200);
  tone(piezo_pin, 500);
  delay(200);
  noTone(piezo_pin);
  delay(200);
  tone(piezo_pin, 500);
  delay(200);
  noTone(piezo_pin);
  delay(200);
  rotate(0,0);
  delay(2000);
  tone(piezo_pin, 800);
  delay(75);
  tone(piezo_pin, 700);
  delay(75);
  tone(piezo_pin, 600);
  delay(75);
  tone(piezo_pin, 500);
  delay(75);
  tone(piezo_pin, 400);
  delay(75);
  tone(piezo_pin, 300);
  delay(75);
  tone(piezo_pin, 200);
  delay(75);
  tone(piezo_pin, 100);
  delay(100);
  noTone(piezo_pin);
 }

//calibration and debug functions
void click_cw() {
  motorStartMillis = millis();
  for (int i=0; i < 1; i++) {
    //Turns the motor until the gear has rotated turnsPerRotation
    while (rotation_gearTurns < 1) {
      motor_cw();
    }
    rotation_gearTurns = 0; //this seems out of place, but it must be set otherwise the above WHILE loop can only run once
  }
  motor_stop();
}

void click_ccw() {
  motorStartMillis = millis();
  for (int i=0; i < 1; i++) {
    //Turns the motor until the gear has rotated turnsPerRotation
    while (rotation_gearTurns < 1) {
      motor_ccw();
    }
    rotation_gearTurns = 0; //this seems out of place, but it must be set otherwise the above WHILE loop can only run once
  }
  motor_stop();
}

void startup_beep() {
  tone(piezo_pin, 100);
  delay(75);
  tone(piezo_pin, 200);
  delay(75);
  tone(piezo_pin, 300);
  delay(75);
  tone(piezo_pin, 400);
  delay(75);
  tone(piezo_pin, 500);
  delay(75);
  tone(piezo_pin, 600);
  delay(75);
  tone(piezo_pin, 700);
  delay(75);
  tone(piezo_pin, 800);
  delay(100);
  noTone(piezo_pin);
}

