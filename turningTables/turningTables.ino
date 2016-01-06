// constants won't change. They're used here to set pin numbers:
const int hallPin = 2;     // the number of the hall effect sensor pin
const int motorPin =  13;     // the number of the LED pin
const int gearRatioConst = 5;


// variables will change:
int hallState = 0;          // variable for reading the hall sensor status
int gearTurns = 0; //variable for how many times the gear with the magnet has spun

void setup() {
  // initialize the LED pin as an output:
  pinMode(motorPin, OUTPUT);      
  // initialize the hall effect sensor pin as an input:
  pinMode(hallPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(hallPin), rpmRead, FALLING);
  Serial.begin(9600);
}

void loop(){
  rotate(true, 3);
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
  }
  Serial.println("FINISHED ONE TRACK MOVE");
}

void counterclockwise() {
  gearTurns = 0;
  while (gearTurns != gearRatioConst) {
    Serial.print("Rotating <<<... Gear Turns = ");
    Serial.println(gearTurns);
  }
  Serial.println("FINISHED ONE TRACK MOVE");
}

void rotate(boolean clockwiseOrNot, int distance) {
  if (clockwiseOrNot == true) {
    //clockwise rotation
    for (int rotations = 0; rotations < distance; rotations++) {
      clockwise();
    }
    Serial.println("DONE");
    
  }
  if (clockwiseOrNot == false) {
    for (int rotations = 0; rotations < distance; rotations++) {
      counterclockwise();
    }
    Serial.println("DONE");
  }
}
