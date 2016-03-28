/*
 * avoidance_quad.ino
 *
 * Title: Collision Avoidance Algorithm
 *
 * Description:
 * This program has three major roles: 
 * first, reading RC signals from the receiver and forwarding them to the flight controller.
 * second, get distance measurements from all four ultrasonic sensors and determine the closest measurement if any.
 * third, if the closest measurement is within the acceptable threshold, instruct flight controller to perform movement in opposite direction of trigged sensor. 
 *
 * setup()
 * void printReceiverInputToConsole()
 * void printArduinoOutputToConsole()
 * void auxIsInAutoPilotMode()
 * void forwardRCSignalsToFlightController()
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

#define sensorDetectionDistanceInFeet 2

boolean PRINT_INPUT_FROM_RECEIVER = false;
boolean PRINT_OUTPUT_FROM_ARDUINO = false;

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
NewPing sensors[] = {p1, p2, p3, p4};

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Setup
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void setup() {
  if (PRINT_INPUT_FROM_RECEIVER == true || PRINT_OUTPUT_FROM_ARDUINO == true) {
    Serial.begin(115200);
  }
  pinMode(throttleIn, INPUT);
  pinMode(rollIn, INPUT);
  pinMode(pitchIn, INPUT);
  pinMode(yawIn, INPUT);
  pinMode(auxIn, INPUT);

  pinMode(throttleOut, OUTPUT);
  pinMode(rollOut, OUTPUT);
  pinMode(pitchOut, OUTPUT);
  pinMode(yawOut, OUTPUT);
  pinMode(auxOut, OUTPUT);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Print RC Input Signals to Console
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void printReceiverInputToConsole() {
  Serial.print("Throttle 1: ");
  Serial.print(throttleOut);
  Serial.print(" | ");

  Serial.print("Channel 2:");
  Serial.print(rollOut);
  Serial.print(" | ");

  Serial.print("Channel 3:");
  Serial.print(pitchOut);
  Serial.print(" | ");

  Serial.print("Channel 4:");
  Serial.print(yawOut);
  Serial.print(" | ");

  Serial.print("Aux 1: ");
  Serial.print(auxOut);
  Serial.println();
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Print Arudino Output Signals to Consol
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void printArduinoOutputToConsole() {
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
// Check Aux Channel For AutoPilot Signal
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void auxIsInAutoPilotMode() {
  aux = pulseIn(auxIn, HIGH);
  analogWrite(auxOut, map(aux, 0, 2000, 0, 255));
  if (aux < 1500) {
    return true;
  else {
    return false; 
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Forward RC Signals to Flight Controller
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void forwardRCSignalsToFlightController() {
  throttle = pulseIn(throttleIn, HIGH);
  roll = pulseIn(rollIn, HIGH);
  pitch = pulseIn(pitchIn, HIGH);
  yaw = pulseIn(yawIn, HIGH);

  analogWrite(throttleOut, map(throttle, 0, 2000, 0, 255));
  analogWrite(rollOut, map(roll, 0, 2000, 0, 255));
  analogWrite(pitchOut, map(pitch, 0, 2000, 0, 255));
  analogWrite(yawOut, map(yaw, 0, 2000, 0, 255));

  if (PRINT_INPUT_FROM_RECEIVER == true) {
    printReceiverInputToConsole();
  }
  if (PRINT_OUTPUT_FROM_ARDUINO == true) {
    printArduinoOutputToConsole();
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Return ping sensor with closest result
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int getClosestDirection() {
  unsigned long distance;
  int minimum = sensorDetectionDistanceInFeet * 12; // convert to ft
  int result = -1;

  // it takes one seconds to get complete a measurement cycle
  for (int i = 0; i < 4; i++) {
    distance = sensors[i].ping_median(7) / US_ROUNDTRIP_IN; // calculate median distance a burst of 7 sonar pings

    if (distance < minimum && distance != 0) {
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
    analogWrite(outputPin, map(i, 0, 2000, 0, 255));
    delayMicroseconds(500);
  }

  delay(1500);

  for (int i = finalSpeed; i > currentSpeed; i--) {
    analogWrite(outputPin, map(i, 0, 2000, 0, 255));
    delayMicroseconds(500);
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Smooth Deceleration
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void smoothDeceleration(int outputPin, int currentSpeed, int finalSpeed) {
  boolean flag = true;
  for (int i = currentSpeed; i > finalSpeed; i--) {
    analogWrite(outputPin, map(i, 0, 2000, 0, 255));
    delayMicroseconds(500);
  }

  delay(1500);

  for (int i = finalSpeed; i < currentSpeed; i++) {
    analogWrite(outputPin, map(i, 0, 2000, 0, 255));
    delayMicroseconds(500);
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Main Loop
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void loop() {
  if (auxIsInAutoPilotMode()) {
    int closest = getClosestDirection();
    switch (closest) {
      case 0:
        smoothDeceleration(rollOut, roll, 1350); // move right
        break;
      case 1:
        smoothDeceleration(pitchOut, pitch, 1350); // move backward
        break;
      case 2:
        smoothAcceleration(rollOut, roll, 1650); // move left
        break;
      case 3:
        smoothAcceleration(pitchOut, pitch, 1650); // move forward
        break;
    }
  } else {
    forwardRCSignalsToFlightController();
  }
}

