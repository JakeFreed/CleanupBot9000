// Hardware Pinouts:
//Air Pump
#define airPumpA 14
#define airPumpB 15
#define conveyorA 2
#define conveyorB 4
​
//Drive Train
#define rightDriveA 7
#define rightDriveB 6
#define rightDriveEnable 3
#define leftDriveA 9
#define leftDriveB 8
#define leftDriveEnable 5
​
//Intake
#define intakeA 11
#define intakeB 10
​
//Lift
#define doorMotorA 13
#define doorMotorB 12
​
//Solenoid
#define solenoidValve 22
​
//Radio
#define radioMISO 50
#define radioMOSI 51
#define radioSCK 52
#define radioCSN 49
#define radioCE 48
//#define radioIRQ 
​
//Libraries
#include "RF24.h" //Import RF24 library
#include <SPI.h>  //Import SPI library
​
byte addresses[][6] = {"0"};
​
RF24 radio (radioCE, radioCSN); //These can be any I/O pins. Connect to (CE, CSN) respectively 
​
struct radioInformation //data package that will be sent
{
  int speed;
  int xCord;
  int yCord;
  int varPot;
};
​
radioInformation data;
​
int speed;
int turn;
int Direction;
int varPot;
​
int power;
int Delta;
int Theta;
​
const int STALL_CORRECTION = 100;
​
void setup(){
    pinMode(airPumpA, OUTPUT);
    pinMode(airPumpB, OUTPUT);
    pinMode(conveyorA, OUTPUT);
    pinMode(conveyorB, OUTPUT);
    pinMode(rightDriveA, OUTPUT);
    pinMode(rightDriveB, OUTPUT);
    pinMode(leftDriveA, OUTPUT);
    pinMode(leftDriveB, OUTPUT);
    pinMode(intakeA, OUTPUT);
    pinMode(intakeB, OUTPUT);
    pinMode(doorMotorA, OUTPUT);
    pinMode(doorMotorB, OUTPUT);
    pinMode(solenoidValve, OUTPUT);
    pinMode(radioMOSI, OUTPUT);
    pinMode(radioSCK, OUTPUT);
    pinMode(radioCSN, OUTPUT);
    // pinMode(radioIRQ, OUTPUT);
    pinMode(radioCE, OUTPUT);
    Serial.begin(9600);
​
    //Setup radio
    radio.begin(); 
    radio.setChannel(115); 
    radio.setPALevel(RF24_PA_MAX); //Set NRF24L01+ to max current draw
    radio.setDataRate( RF24_250KBPS ) ; //Set data rate to 250KBPS (for longest range)
    radio.maskIRQ(1, 1, 0); //set NRF24L01+ to inturuppt only when data is recieved
    radio.openReadingPipe(1, addresses[0]);
    radio.startListening();
​
    Serial.println("Radio Setup Complete");
    Serial.println("Setup Complete");
    delay(2000);
}
​
void loop(){
    readRadio();
    // rightDrive(Direction);
    // leftDrive(Direction);
    drive(Direction, turn);
    intake(varPot);
    conveyor(speed);
    door(speed);
    Serial.println(Direction);
    // delay(1);
    // Serial.println("Looping");
    // delay(1000);
    // // drive(255, 100);
    // intake(255);
    // delay(1000);
    // intake(0);
}
​
void readRadio(){
  if ( radio.available()) //Check if radio is available 
  {
      radio.read(&data, sizeof(data));
      // For testing reasons:
        Serial.print("\nPackage:");
        Serial.print("x-Cord:");
        Serial.print(data.xCord);
        Serial.print("\n");
        Serial.print("y-Cord:");
        Serial.print(data.yCord);
        Serial.print("\n");
        Serial.print("speed:");
        Serial.print(data.speed);
        Serial.print("\n");
        Serial.print("Var Pot:");
        Serial.print(data.varPot);
        Serial.print("\n");
  }
  else {
    Serial.print("Radio was not avaliable");
    // stopDrive();
    return;
  }
  /*Serial.println(Direction);
  Serial.println(turn);
  Serial.println(speed);*/
  
  Direction = data.yCord - 508;
  turn = data.xCord - 499;
  speed = data.speed; //allows for very fine control on speed
  varPot = data.varPot;
}
​
void stopDrive(){
    // drive(0, 0);
}
​
// void drive(int speed, int turn) {
//     int leftSpeed = speed + turn;
//     int rightSpeed = speed - turn;
​
//     leftDrive(leftSpeed);
//     rightDrive(rightSpeed);
// }
​
void drive(int speed, int turn) {
    int leftSpeed = speed + turn;
    int rightSpeed = speed - turn;
​
    leftDrive(leftSpeed);
    rightDrive(rightSpeed);
}
​
void rightDrive(int speed){
    Serial.print("Right Drive: ");
    Serial.println(speed);
    if (speed < 10 && speed > -10){
        Serial.println("Stopping");
        digitalWrite(rightDriveA, LOW);
        digitalWrite(rightDriveB, LOW);
    }
    else if (speed > 10){
        Serial.println("Forward");
        digitalWrite(rightDriveA, HIGH);
        digitalWrite(rightDriveB, LOW);
        digitalWrite(rightDriveEnable, HIGH);
        // analogWrite(rightDriveEnable, speed + STALL_CORRECTION);
    }
    else if (speed < -10){
        Serial.println("Backward");
        digitalWrite(rightDriveA, LOW);
        digitalWrite(rightDriveB, HIGH);
        digitalWrite(rightDriveEnable, HIGH);
        // analogWrite(rightDriveEnable, -speed + STALL_CORRECTION);
    }
}
​
void leftDrive(int speed){
    Serial.print("Right Drive: ");
    Serial.println(speed);
    if (speed < 10 && speed > -10){
        Serial.println("Stopping");
        digitalWrite(leftDriveA, LOW);
        digitalWrite(leftDriveB, LOW);
    }
    else if (speed > 10){
        Serial.println("Forward");
        digitalWrite(leftDriveA, HIGH);
        digitalWrite(leftDriveB, LOW);
        digitalWrite(leftDriveEnable, HIGH);
        // analogWrite(leftDriveEnable, speed + STALL_CORRECTION);
    }
    else if (speed < -10){
        Serial.println("Backward");
        digitalWrite(leftDriveA, LOW);
        digitalWrite(leftDriveB, HIGH);
        digitalWrite(leftDriveEnable, HIGH);
        // analogWrite(leftDriveEnable, -speed + STALL_CORRECTION);
    }
}
​
void intake(int speed) {
    Serial.print("Intake: ");
    Serial.println(speed);
    if (speed < 30 ) {
        Serial.println("Stopping");
        digitalWrite(intakeA, LOW);
        digitalWrite(intakeB, LOW);
        airPump(0);
    }
    else if (speed > 30 && speed < 500) {
        Serial.println("Forward"); 
        digitalWrite(intakeA, HIGH);
        digitalWrite(intakeB, LOW);
        airPump(255);
    }
​
    else if (speed > 501 && speed < 1023) {
        Serial.println("Backward");
        airPump(255);
    }
}
​
void airPump(int s) {
    Serial.print("Air Pump: ");
    Serial.println(s);
    if (s < 30) {
        Serial.println("Off");
        digitalWrite(airPumpA, LOW);
    }
    else if (s > 30 && s < 500) {
        Serial.println("On");
        digitalWrite(airPumpA, HIGH);
    } else if (s > 501 && s < 1023) {
        Serial.println("On");
        digitalWrite(airPumpB, HIGH);
    }
}
​
void conveyor(int s) {
    Serial.print("Conveyor");
    Serial.println(s);
    if (s < 30 ) {
        Serial.println("Off");
        digitalWrite(conveyorA, LOW);
        digitalWrite(conveyorB, LOW);
    } else if (s > 31 && s < 500) {
        Serial.println("On");
        digitalWrite(conveyorA, HIGH);
        digitalWrite(conveyorB, LOW);
    }
}
​
void door(int s) {
    Serial.println("Door");
    if (s < 30) {
        Serial.println("Off");
        digitalWrite(doorMotorA, LOW);
    }
    else if (s > 501) {
        Serial.println("On");
        digitalWrite(doorMotorA, HIGH);
    }
}
​
void blower(int s) {
    Serial.println("Blower: ");
    Serial.println(s);
​
    if (s < 30 ) {
        Serial.println("Off");
        digitalWrite(solenoid, LOW);
        digitalWrite(airPumpB, LOW);
    } else {
        digitalWrite(solenoid, HIGH);
        digitalWrite(airPumpB, HIGH);
    }
}
​
void solenoid(int speed) {
    Serial.println("Solenoid: ");
    Serial.println(speed);
    if (speed < 30 ) {
        // Serial.println("")
        digitalWrite(solenoidValve, LOW);
    }
    else {
        // Serial.println("")
        digitalWrite(solenoidValve, HIGH);  
    }
        
}
​