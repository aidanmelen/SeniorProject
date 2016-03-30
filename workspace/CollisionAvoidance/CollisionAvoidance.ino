/*
 * Title: Collision Avoidance Algorithm
 *
 * Description:
 * This program has three major roles:
 * first, reading RC signals from the receiver and forwarding them to the flight controller.
 * second, get distance measurements from all four ultrasonic sensors and determine the closest measurement if any.
 * third, if the closest measurement is within the acceptable threshold, instruct flight controller to perform movement in opposite direction of trigged sensor.
 */

#include <NewPing.h>


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Configuration Variables
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define OUTER_DISTANCE_THRESHOLD_FOR_SENSORS_IN_CM 150
#define INNER_DISTANCE_THRESHOLD_FOR_SENSORS_IN_CM 10
#define MAX_PING_DISTANCE_IN_CM 200
#define NUMBER_OF_SONAR_BURSTS 10
#define MAX_AUTOPILOT_MOVEMENT 100

boolean chooseRandomSafetyDirection = false;
boolean serialMonitorIsOpen = false;

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
// Varibles used to store and display the values of each channel
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int throttle;
int roll;
int pitch;
int yaw;
int aux;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Input Pins for Ultrasonic Sensors
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define s1 40 // sensor left
#define s2 42 // sensor front
#define s3 44 // sensor right
#define s4 46 // sensor rear

// Create NewPing objects for each sensor
NewPing p1(s1, s1, 200); // pins share the same for both trig and echo
NewPing p2(s2, s2, 200);
NewPing p3(s3, s3, 200);
NewPing p4(s4, s4, 200);
NewPing sensorArray[] = {p1, p2, p3, p4};

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Setup
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void setup() {
  //  if (serialMonitorIsOpen == true) {
  Serial.begin(115200);
  //  }
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

  //  randomSeed(analogRead(0)); // for random safety direction
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Print RC Input Signals to Console
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
// Print Sonar Measurements to Console
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void printSonarMeasurementsToConsole(int sensorIndex, int distanceMeasurement) {
  Serial.print("sensor: ");
  Serial.print(sensorIndex);
  Serial.print(" ");
  Serial.print("distance: ");
  Serial.print(distanceMeasurement);
  Serial.print("  --  ");
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Print Final Avoidance Directon
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void printFinalAvoidanceDirecton(int avoidanceDirection) {
  if (avoidanceDirection == 0) {
    Serial.println();
    Serial.println("Avoidance Movement Left");
    Serial.println();
  }
  if (avoidanceDirection == 0) {
    Serial.println();
    Serial.println("Avoidance Movement Front");
    Serial.println();  
  }
  if (avoidanceDirection == 0) {
    Serial.println();
    Serial.println("Avoidance Movement To Right");
    Serial.println();    
  }
  if (avoidanceDirection == 0) {
    Serial.println();
    Serial.println("Avoidance Movement To Backwards");
    Serial.println();    
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Check Aux For AutoPilot Signal
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
boolean isAutoPilotMode() {
  aux = pulseIn(auxIn, HIGH);
  if (aux > 1500) return true;
  else return false;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Forward Aux Signal To The Flight Controller
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void forwardAuxSignal() {
  analogWrite(auxOut, map(aux, 0, 2000, 0, 255));
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Forward RC Signals To Flight Controller
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

  if (serialMonitorIsOpen) printReceiverInputToConsole();
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Run Auto Pilot Loop: Determine Sensor With Closest Measurement And Preform Collision Avoidance Manuever
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int runAutoPilot() {
  unsigned long finalDistanceMeasurement;
  int potentialAvoidanceDirecton;
  while (true) {
    potentialAvoidanceDirecton = getClosestDirection();
    if (potentialAvoidanceDirecton != -1) {
      finalDistanceMeasurement = sensorArray[potentialAvoidanceDirecton].ping_median(NUMBER_OF_SONAR_BURSTS) / US_ROUNDTRIP_CM;
      if (distanceMeasurementIsWithinRange(finalDistanceMeasurement)) forwardCollisionAvoidanceManeuverToFlightController((potentialAvoidanceDirecton + 2) % 4);
    }
    if (!isAutoPilotMode()) break;
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Get The Direction With The First Sensor That Is Closer Than OUTER_DISTANCE_THRESHOLD_FOR_SENSORS_IN_CM
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int getClosestDirection() {
  unsigned long initialDistanceMeasurement;
  int potentialAvoidanceDirecton = -1;
  for (int sensorIndex = 0; sensorIndex < 4; sensorIndex++)  {
    forwardRCSignalsToFlightController();
    initialDistanceMeasurement = sensorArray[sensorIndex].ping() / US_ROUNDTRIP_CM;
    forwardRCSignalsToFlightController();

    if (distanceMeasurementIsWithinRange(initialDistanceMeasurement)) potentialAvoidanceDirecton = sensorIndex;
    if (!isAutoPilotMode()) break;
  }
  return potentialAvoidanceDirecton;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Check If Distance Measurement is Within The Target Range
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
boolean distanceMeasurementIsWithinRange(int distanceMeasurement) {
  if (distanceMeasurement < OUTER_DISTANCE_THRESHOLD_FOR_SENSORS_IN_CM && distanceMeasurement > INNER_DISTANCE_THRESHOLD_FOR_SENSORS_IN_CM && distanceMeasurement != 0) return true;
  else return false;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Forward Collision Avoidance Movement To The Flight Controler
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void forwardCollisionAvoidanceManeuverToFlightController(int avoidanceDirection) {
  switch (avoidanceDirection) {
    case 0:
      if (serialMonitorIsOpen) printFinalAvoidanceDirecton(avoidanceDirection);
      smoothAcceleration(rollOut, roll, roll + MAX_AUTOPILOT_MOVEMENT); // move left
      break;
    case 1:
      if (serialMonitorIsOpen) printFinalAvoidanceDirecton(avoidanceDirection);
      smoothAcceleration(pitchOut, pitch, pitch + MAX_AUTOPILOT_MOVEMENT); // move forward
      break;
    case 2:
      if (serialMonitorIsOpen) printFinalAvoidanceDirecton(avoidanceDirection);
      smoothDeceleration(rollOut, roll, roll - MAX_AUTOPILOT_MOVEMENT); // move right
      break;
    case 3:
      if (serialMonitorIsOpen) printFinalAvoidanceDirecton(avoidanceDirection);
      smoothDeceleration(pitchOut, pitch, roll - MAX_AUTOPILOT_MOVEMENT); // move backward
      break;
    default:
      break;
  }
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

  delay(500);

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

  delay(500);

  for (int i = finalSpeed; i < currentSpeed; i++) {
    analogWrite(outputPin, map(i, 0, 2000, 0, 255));
    delayMicroseconds(500);
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Main Loop
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void loop() {
  if (isAutoPilotMode()) {
    runAutoPilot();
  } else {
    forwardRCSignalsToFlightController();
  }
}

