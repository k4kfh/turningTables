#include <CMRI.h>

// constants won't change. They're used here to set pin numbers:
const int hallPin = 2;     // the number of the hall effect sensor pin
const int motorPin =  13;     // the number of the LED pin
const int gearRatioConst = 5;
const int numTracks = 20;


// variables will change:
int hallState = 0;          // variable for reading the hall sensor status
int gearTurns = 0; //variable for how many times the gear with the magnet has spun
int currentTrack = 0;

void setup() { 
  // initialize the hall effect sensor pin as an input:
  pinMode(hallPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(hallPin), rpmRead, FALLING);

//motor shield pins
  pinMode(3, OUTPUT);
  pinMode(12, OUTPUT);
  Serial.begin(9600, SERIAL_8N2);
}

void loop(){
  clockwise();
  counterclockwise();
}

//This is a function executed (with attachInterrupt) every time the hall effect sensor goes from high to low
void rpmRead() {
  //This is executed every time the hall effect sensor goes from high to low
  gearTurns++;
}

void clockwise() {
  gearTurns = 0;
  while (gearTurns != gearRatioConst) {
    digitalWrite(12, HIGH);
    digitalWrite(3, HIGH);
  }
  digitalWrite(3, LOW);
  digitalWrite(12, LOW);
}

//Called to turn the turntable 1 move counterclockwise.
void counterclockwise() {
  gearTurns = 0;
  while (gearTurns != gearRatioConst) {
    digitalWrite(3, HIGH);
  }
  digitalWrite(3, LOW);
  digitalWrite(12, LOW);
}

void rotate(int directionToGo, int distance) {
  if (directionToGo == 0) {
    //clockwise rotation
    for (int rotations = 0; rotations < distance; rotations++) {
      clockwise();
    }
    
  }
  if (directionToGo == 1) {
    for (int rotations = 0; rotations < distance; rotations++) {
      counterclockwise();
    }
  }
}

//direct is a weird argument. 1 means clockwise, 2 is counterclockwise
void goToTrack(int trknumber, int direct) {
  int actualdirect;
  if (direct == -1) {
    //auto choose direction
    actualdirect = chooseDirection(currentTrack, trknumber, numTracks);
  }
  else {
    actualdirect = direct;
  }
  int moves; //declared here for proper scope crap
  if (actualdirect == 0) {
    //clockwise
    moves = trknumber - currentTrack;
  }
  else if (actualdirect == 1) {
    //counterclockwise
    moves = (currentTrack + ( (numTracks + 1) - trknumber)); //this elegant math gives the number of counterclockwise track moves we have to make to get to a given track
  }

  rotate(actualdirect, moves);

  //set current track to new one
  currentTrack = trknumber;
}


/*
chooseDirection()

Using some borrowed code from StackExchange, I built this function that determines the quickest direction to get where we need to go.
*/
int chooseDirection(int currentPosition, int goingTo, int maxPosition) {
  int directionToGo; //this will be defined later, just go ahead and declare it
  int specialNum = maxPosition / 2;
  if(currentPosition < goingTo) {
    if(abs(currentPosition - goingTo) < specialNum) {
       //CLOCKWISE
       directionToGo = 0;
    }
    else {
        //COUNTERCLOCKWISE
        directionToGo = 1;
    }
  }

  else {
    if(abs(currentPosition - goingTo) < specialNum) {
       //COUNTERCLOCKWISE
       directionToGo = 1;
    }
    else {
        //CLOCKWISE
        directionToGo = 0;
    }
  }
  return directionToGo;
 }

