#define num_tracks 24
#define motor_cw_pin 10
#define motor_ccw_pin 11
#define hall_pin 2 //make sure this pin is interrupt-capable
const int turnsPerRotation = 5; //this is 5 on Atlas turntable
#define normal_motor_speed 255 //this is the PWM number the motor will turn at, if on a PWM-capable pin

unsigned total_gearTurns = 0;
int rotation_gearTurns = 0; //this will be between 1 and 5 always
int currentPosition = 0;

void setup() {
  //set all the pins to the correct mode
  pinMode(motor_cw_pin, OUTPUT);
  pinMode(motor_ccw_pin, OUTPUT);
  pinMode(hall_pin, INPUT);

  //making hall effect sensor interrupt
  attachInterrupt(digitalPinToInterrupt(hall_pin), hallTrigger, FALLING);

  //start Serial connection
  Serial.begin(9600);

  
}

void loop() {
  // put your main code here, to run repeatedly:
  rotate(12,0);
  rotate(23,0);
  rotate(24,0);
}

//simple function to run on hall effect sensor interrupt
void hallTrigger() {
  Serial.println("TRIGGER");
  total_gearTurns++;
  Serial.print("Total Gear Turns:"); Serial.println(total_gearTurns);

  //code to increment rotation_gearTurns
  if (rotation_gearTurns < turnsPerRotation) {
    rotation_gearTurns++;
  }
  else {
    rotation_gearTurns = 1; //at first glance it seems this should be zero. It shouldn't.
  }

  Serial.print("Rotation Gear Turns:"); Serial.println(rotation_gearTurns);
}

void motor_cw() {
  analogWrite(motor_cw_pin, normal_motor_speed);
}

void motor_ccw() {
  analogWrite(motor_ccw_pin, normal_motor_speed);
}

void motor_stop() {
  analogWrite(motor_ccw_pin, 0);
  analogWrite(motor_cw_pin, 0);
}

//Turns the turntable clockwise by moves number of tracks
void turn_cw(int moves) {
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
    
  }
  if (mode == 2) {
    //forced counterclockwise
    
  }
  if (mode == 3) {
    //rotate 180
    
  }
  Serial.print("Moves needed: "); Serial.println(movesNeeded);
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

