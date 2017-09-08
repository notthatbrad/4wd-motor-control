//Set up variables for readability
const int frontLeft = 0;
const int backLeft = 1;
const int frontRight = 2;
const int backRight = 3;

const int Forward = 0;
const int Backward = 1;
const int PivotRight = 2;
const int PivotLeft = 3;


//set the rising edge/falling edge bytes for edge detection on the encoder wheel.
const byte risingEdge=0b01111111;
const byte fallingEdge=0b10000000;




//Motor Class containing settings and functions for each motor.
class Motor{
  //Pin assignments for the motor
  int enablePin;        //enable pin for l293d
  int forwardPin;       //pin for forward movement
  int backwardPin;      //pin for backward movement
  int encoderPin;       //pin for reading encoder on current motor.
  
  //current state of the encoder wheel. This variable is used for edge detection on the wheel.
  //initialized to 0
  byte encoderState=0b00000000;

  //variables for encoder counting.
  //
  int count = 0;                  //the number of "spokes" counted. one rotation == 20
  unsigned long startTime = 0;    //the time at which the current rotation has started.

  public:
  //motor constructor
  Motor(){};

 
  
  //initialize Motor variables
  void initMotor(int fPin, int bPin, int enPin, int encPin){
    enablePin = enPin;
    forwardPin = fPin;
    backwardPin = bPin;
    encoderPin = encPin;

    //Set up pins for controlling the motor
    pinMode(enablePin, OUTPUT);
    pinMode(forwardPin, OUTPUT);
    pinMode(backwardPin, OUTPUT);
    pinMode(encoderPin, INPUT);
  
    //initialize pins
    digitalWrite(forwardPin, 0);
    digitalWrite(backwardPin, 0);
    digitalWrite(enablePin, 0);
  }


  //uses the enable pin to check if the motor is moving
  boolean isMoving(){
    return digitalRead(enablePin);
  }

  //clears the variables used for counting the encoder speed.
  //called when a wheel stops, or a new rotation begins
  void clearParameters(){
    encoderState=0b00000000;
    count = 0;
    startTime = 0;
  }
  
  void goForward(){
    digitalWrite(forwardPin, 1);
    digitalWrite(backwardPin, 0);
    digitalWrite(enablePin, 1);
  }
  
  void goBackward(){
    digitalWrite(forwardPin, 0);
    digitalWrite(backwardPin, 1);
    digitalWrite(enablePin, 1);
  }

  void Stop(){
    digitalWrite(forwardPin, 0);
    digitalWrite(backwardPin, 0);
    digitalWrite(enablePin, 0);
    clearParameters();
  }


//used to count the rotation of the wheels
//TODO:update to return significant data to be used for calibration.
  void checkRotation(){

    //if this is the start of a new rotation, initialize the startTime to the current time
    if(count == 0){
      startTime =  millis();
    }

    //update the encoderState byte by shifting in the encoder value
    encoderState = (encoderState << 1) | digitalRead(encoderPin);

    //id the encoderState variable is on a rising edge, increment the count variable. If not, count stays the same.
    count = encoderState == risingEdge ? count +1 : count;

   //20 clicks of the encoder wheel is a full rotation.
   //once that is reached, clear counter variables to begin a new rotation.
    if(count >= 20){
      clearParameters();
    }

  }

  //TODO: create function for getting/setting pwm values for motor speeds
  
 };


//create an array of motors
Motor motor[4];


//unsigned long startTime = 0;


void setup() {
  //start the serial ports
  Serial.begin(9600); 
  Serial.println("program started");
 
  //initialize motor variables in each motor
  //void initMotor(int forwardPin, int backwardPin, int enablePin, int encoderPin
  //TODO: redo pin assignments to make wiring cleaner.
  motor[frontLeft].initMotor(2,4,3,7);
  motor[backLeft].initMotor(12,13,5,8);
  motor[frontRight].initMotor(A0,A1,6,10);
  motor[backRight].initMotor(A2,A3,9,11);

}



void loop() {


  //loop through the motors and check if each is moving. If it is, get update te rotation data.
  //TODO: this is where I will eventually implement monitoring or calibration for speed control on each wheel
  for(int x = 0; x < 4; x++){
    if(motor[x].isMoving()){
      motor[x].checkRotation();
    }
  }


  //check the serial input, and read the first byte to control the motion
  //TODO: update this to allow me to tell the motors how many rotations to go in any direcion
  //TODO: update to allow rotation degree input e.g. pivotLeft(90) would be a 90 degree pivot (left turn)
  if (Serial.available() > 0) {
                // read the serial data for incoming directional data
                switch (Serial.read()){
                  case Forward:
                    Serial.print("Moving Forward");
                    motor[frontLeft].goForward();
                    motor[backLeft].goForward();
                    motor[frontRight].goForward();
                    motor[backRight].goForward();
                    break;
                    
                  case Backward:
                    Serial.print("Moving Backward");
                    motor[frontLeft].goBackward();
                    motor[backLeft].goBackward();
                    motor[frontRight].goBackward();
                    motor[backRight].goBackward();
                    break;
  
                  case PivotRight:
                    Serial.print("Rotate Left");
                    motor[frontLeft].goForward();
                    motor[backLeft].goForward();
                    motor[frontRight].goBackward();
                    motor[backRight].goBackward();
                    break;

                  case PivotLeft:
                    Serial.print("Rotate Right");
                    motor[frontLeft].goBackward();
                    motor[backLeft].goBackward();
                    motor[frontRight].goForward();
                    motor[backRight].goForward();
                    break;
                    
                  default:
                    Serial.print("Stop");
                    motor[frontLeft].Stop();
                    motor[backLeft].Stop();
                    motor[frontRight].Stop();
                    motor[backRight].Stop();
                    break;
 
                }
                
  }



}




