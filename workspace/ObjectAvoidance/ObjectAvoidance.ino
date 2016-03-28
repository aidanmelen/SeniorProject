/*
 * avoidance_quad.ino
 * 
 * Title: Quadcopter with Object Avoidance
 *
 * Description: 
 * In this project, a remote controlled quadcopter capable of detecting and reacting to nearby objects is explored. 
 * The purpose of this project is to reduce the responsibilities a pilot will have in regards to crash avoidance. 
 * The quadcopter is expected to contain the following attributes: stability will be determined by the flight controller,
 * an array of onboard ultrasonic sonar sensors will provide object detection, and an Arduino Mega ADK microcontroller will execute the avoidance protocol. 
 * As a result, the quad will remain fixed in a relative point in space until an object triggers the aircraft to perform an avoidance maneuver, 
 * thereby directing the craft to safety.
 *
 * setup()
 * void printReceiverInputToConsole()
 * boolean isAutoPilot()
 * void forwardRCSignals()
 * int getClosestDirection()
 * void smoothAcceleration(outputPin, currentSpeed, finalSpeed)
 * void smoothDeceleration(outputPin, currentSpeed, finalSpeed)
 * void loop()
 */

#include <NewPing.h>

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Input Pins for Receiver
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define throttleIn 2
#define rollIn 3
#define pitchIn 4
#define yawIn 5
#define auxIn 6

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Output Pins for Naza-M V2
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define throttleOut 8
#define rollOut 9
#define pitchOut 10
#define yawOut 11
#define auxOut 12

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Input Pins for Ultrasonic Sensors
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define s1 40 // sensor left
#define s2 42 // sensor front
#define s3 44 // sensor right
#define s4 46 // sensor rear

#define sensorDetectionDistanceInFeet 5

boolean PRINT_RECEIVER_INPUT = false;

// Varibles used to store and display the values of each channel
int throttle;
int roll;
int pitch;
int yaw;
int aux;

// Create NewPing objects for each sensor
NewPing p1(s1, s1); // pin are the same for both trig and echo
NewPing p2(s2, s2);
NewPing p3(s3, s3);
NewPing p4(s4, s4);
NewPing sensors[] = {p1,p2,p3,p4};

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Setup
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void setup() {
  if(PRINT_RECEIVER_INPUT == true) {
    Serial.begin(115200);
  }
  pinMode(throttleIn,INPUT);
  pinMode(rollIn,INPUT);
  pinMode(pitchIn,INPUT);
  pinMode(yawIn,INPUT);
  pinMode(auxIn,INPUT);
  
  pinMode(throttleOut,OUTPUT);
  pinMode(rollOut,OUTPUT);
  pinMode(pitchOut,OUTPUT);
  pinMode(yawOut,OUTPUT);
  pinMode(auxOut,OUTPUT);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Print RC Input Signals to Consol
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void printReceiverInputToConsole() {
  Serial.print("Throttle 1: ");
  Serial.print(throttle);
  Serial.print(" | ");

  Serial.print("Channel 2:");
  Serial.print(roll);
  Serial.print(" | ");
  
  Serial.print("Channel 3:");
  Serial.print(pitch);
  Serial.print(" | ");
  
  Serial.print("Channel 4:");
  Serial.print(yaw);
  Serial.print(" | ");
  
  Serial.print("Aux 1: ");
  Serial.print(aux);
  Serial.println();
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Check Aux Signal For AutoPilot
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void forwardAuxSignal() {
  aux = pulseIn(auxIn,HIGH);
  analogWrite(auxOut,map(aux,0,2000,0,255));
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Forward RC Signals
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void forwardRCSignals() {
  throttle = pulseIn(throttleIn,HIGH);
  roll = pulseIn(rollIn,HIGH);
  pitch = pulseIn(pitchIn,HIGH);
  yaw = pulseIn(yawIn,HIGH);
  
  analogWrite(throttleOut,map(throttle,0,2000,0,255));
  analogWrite(rollOut,map(roll,0,2000,0,255));
  analogWrite(pitchOut,map(pitch,0,2000,0,255));
  analogWrite(yawOut,map(yaw,0,2000,0,255));
  
  if (PRINT_RECEIVER_INPUT == true) {
    printReceiverInputToConsole(); 
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Return ping sensor with closest result
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int getClosestDirection() {
  unsigned long distance;
  int minimum = sensorDetectionDistanceInFeet * 12; // convert to ft
  int result = -1;
  
  for(int i = 0; i<4; i++) {
    distance = sensors[i].ping() / US_ROUNDTRIP_IN; // calculate distance from sensor
    
    if(distance < minimum && distance != 0) {
      minimum = distance; 
      result = i;
    }
  }
  return result;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Smooth Acceleration
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void smoothAcceleration(int outputPin, int currentSpeed, int finalSpeed) {
  boolean flag = true;
  for (int i = currentSpeed; i < finalSpeed; i++) {
    analogWrite(outputPin, map(i,0,2000,0,255));
    delayMicroseconds(500);
  }
  
  delay(1500);
  
  for (int i = finalSpeed; i > currentSpeed; i--) {
    analogWrite(outputPin, map(i,0,2000,0,255));
    delayMicroseconds(500);
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Smooth Deceleration
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void smoothDeceleration(int outputPin, int currentSpeed, int finalSpeed) {
  boolean flag = true;
  for (int i = currentSpeed; i > finalSpeed; i--) {
    analogWrite(outputPin, map(i,0,2000,0,255));
    delayMicroseconds(500);
  }
  
  delay(1500);
  
  for (int i = finalSpeed; i < currentSpeed; i++) {
    analogWrite(outputPin, map(i,0,2000,0,255));
    delayMicroseconds(500);
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Main Loop
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void loop() {
  forwardAuxSignal();
  if(aux < 1500) {
    forwardRCSignals();
  } else {
    int closest = getClosestDirection();
    switch (closest) {
      case 0:
        smoothDeceleration(rollOut,1500,1350); // move right
        break;
      case 1:
        smoothDeceleration(pitchOut,1500,1350); // move backward
        break;
      case 2:
        smoothAcceleration(rollOut,1500,1650); // move left
        break;
      case 3:
         smoothAcceleration(pitchOut,1500,1650);  // move forward
        break;
    }
  }
}

