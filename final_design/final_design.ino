#include <AccelStepper.h>
#include <SoftwareSerial.h>

//pins for stepper motors
#define motorInterfaceType 1
#define dirPinR 1
#define stepPinR 0
#define dirPinL 3
#define stepPinL 2

test changes 
test again

AccelStepper stepperR = AccelStepper(motorInterfaceType, stepPinR, dirPinR);
AccelStepper stepperL = AccelStepper(motorInterfaceType, stepPinL, dirPinL);

//drivemode variables
int runmode = 1;
//variables needed for adjustment in drivemode
const int rotate90 = 600;//steps needed for 90 degree turn
const int rotate180 = 1200;//steps needed for 180 degree turn
const int rotate360 = 2400;//steps needed for 360 degree turn

//variables and pins for ultrasonic
const int trigPin = 5;
const int echoPin = 6;
long duration;
long distance;
long mindistance = 10;
long beacondistance = 5;

//variables and pins for ir reciever
SoftwareSerial irrec(A0, A0);
char sig1 = '0';
char sig2 = '5';
char letter;
boolean firstbeacon = false;
boolean irsignal = false;

//variables and pins for button
const int button1 = 4;
int buttonState = 0;
boolean toggle = false;

void setup() {
  Serial.begin(9600);
  
  stepperR.setMaxSpeed(1000);
  stepperL.setMaxSpeed(1000);
  stepperR.setAcceleration(30);
  stepperL.setAcceleration(30);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  irrec.begin(2400);

  pinMode(button1, INPUT);
}

void loop() {
  if (toggle == true){
    irlight();
    Ping();
    if (distance <= mindistance && irsignal == false){//checks for collision distance if under rotate
      runmode = 3;
    }
    else if(irsignal == false) {
      runmode =7;
    }
    else{//otherwise drive and beaconfind
      runmode = 1;
    }
    
    if (irsignal == true){//ir reciever sees beacon
      if (distance <= beacondistance){//goes dead slow if it reaches the close beacon distance
        runmode = 6;
      }else{//otherwise run at normal speed
        runmode = 1;////////////FIIIIIIIXXXXXXXXX
      }
    }
    Drive();
  }
  if (toggle == false){
    buttonState = digitalRead(button1);
    if (buttonState == 1){
      toggle == true;
    }
  }
}

void Ping(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;//distance in cm
}

void irlight(){
  if (irrec.available()){
    letter = irrec.read();
  }
}

void Drive(){
      switch(runmode){
      case 1://fowards
      stepperL.setSpeed(400);
      stepperR.setSpeed(400);
      break;
      case 2://reverse
      stepperL.setSpeed(-200);
      stepperR.setSpeed(-200);
      break;
      case 3://turn 90 uses while loop as a blocking function to rotate robot
      stepperR.setCurrentPosition(0);
      stepperL.setCurrentPosition(0);
      while(stepperL.currentPosition() != rotate90){//have to play with this
        stepperL.setSpeed(200);
        stepperR.setSpeed(-200);
      }
      delay(500);
      runmode = 5;//stops robot
      break;
      case 4://turn 180 uses while loop as a blocking function to rotate robot
      stepperR.setCurrentPosition(0);
      stepperL.setCurrentPosition(0);
      while(stepperL.currentPosition() != rotate180){
        stepperL.setSpeed(200);
        stepperR.setSpeed(-200);
      }
      delay(500);
      runmode = 5;//stops robot
      break;
      case 5://stop
      stepperL.setSpeed(0);
      stepperR.setSpeed(0);
      break;
      case 6://slow down fowards
      stepperL.setSpeed(200);
      stepperR.setSpeed(200);
      break;
      case 7://rotate 360 beacon find mode
      stepperR.setCurrentPosition(0);
      stepperL.setCurrentPosition(0);
      while(stepperL.currentPosition() != rotate360){
        stepperL.setSpeed(200);
        stepperR.setSpeed(-200);
        irlight();
        if (letter == sig1 && firstbeacon == false){
          runmode = 5;
          break;
        }
        if (letter == sig2 && firstbeacon == true){
          runmode = 5;
          break;
        }
      }
      delay(500);
      runmode = 5;//stops robot
    }
    stepperL.runSpeed();
    stepperR.runSpeed();
}
