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
  // initialize the LED pin as an output:
  pinMode(motorPin, OUTPUT);      
  // initialize the hall effect sensor pin as an input:
  pinMode(hallPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(hallPin), rpmRead, FALLING);
  Serial.begin(9600);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
}

void loop(){
  goToTrack(15, 0);
}

void rpmRead() {
  //This is executed every time the hall effect sensor goes from high to low
  gearTurns++;
}

void clockwise() {
  gearTurns = 0;
  while (gearTurns != gearRatioConst) {
    Serial.print("Rotating >>>... Gear Turns = ");
    Serial.println(gearTurns);
    digitalWrite(6, HIGH);
  }
  Serial.println("FINISHED ONE TRACK MOVE");
  digitalWrite(7, LOW);
  digitalWrite(6, LOW);
}

void counterclockwise() {
  gearTurns = 0;
  while (gearTurns != gearRatioConst) {
    Serial.print("Rotating <<<... Gear Turns = ");
    Serial.println(gearTurns);
    digitalWrite(7, HIGH);
  }
  Serial.println("FINISHED ONE TRACK MOVE");
  digitalWrite(7, LOW);
  digitalWrite(6, LOW);
}

void rotate(int directionToGo, int distance) {
  if (directionToGo == 0) {
    //clockwise rotation
    for (int rotations = 0; rotations < distance; rotations++) {
      clockwise();
    }
    Serial.println("DONE");
    
  }
  if (directionToGo == 1) {
    for (int rotations = 0; rotations < distance; rotations++) {
      counterclockwise();
    }
    Serial.println("DONE");
  }
}

//direct is a weird argument. 1 means clockwise, 2 is counterclockwise
void goToTrack(int trknumber, int direct) {
  if (direct == -1) {
    //auto choose direction
    int actualdirect = chooseDirection(currentTrack, trknumber, numTracks);
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
