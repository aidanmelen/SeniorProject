/*
 * Title: Collision Avoidance Algorithm
 *
 * Description:
 * This program has three major roles:
 * first, reading RC signals from the receiver and forwarding them to the flight controller.
 * second, get distance measurements from all four ultrasonic sensors and determine the closest measurement if any.
 * third, if the closest measurement is within the target range, instruct the flight controller to preform a maneuver in the opposite direction of trigged sensor.
 */

#include <NewPing.h>

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Configuration variables
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define OUTER_DISTANCE_THRESHOLD_FOR_SENSORS_IN_CM 50
#define INNER_DISTANCE_THRESHOLD_FOR_SENSORS_IN_CM 150
#define NUMBER_OF_SONAR_BURSTS 10
#define MAX_AUTOPILOT_MOVEMENT 200
#define AUTOPILOT_MOVEMENT_DURATION_IN_MILLI 500

boolean const preformMarkovAvoidance = false;
boolean const serialMonitorIsOpen = false;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Input pins for receiver
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define throttleIn 2
#define rollIn 3
#define pitchIn 4
#define yawIn 5
#define auxIn 6

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Output pins for Naza-M V2
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

// Create NewPing objects for each sensor where sensors share the same pin for both trig and echo. The third argument is timeout.
NewPing p1(s1, s1, 200);
NewPing p2(s2, s2, 200);
NewPing p3(s3, s3, 200);
NewPing p4(s4, s4, 200);
NewPing sensorArray[] = {p1, p2, p3, p4};

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Setup Pins for RC i/o and utrasonic sensor inputs
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

  randomSeed(analogRead(0)); // for random safety direction
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Print RC input signals to console
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
// Print sonar measurements to console
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void printSonarMeasurementsToConsole(int sensorIndex, int distanceMeasurement) {
  if (sensorIndex == 0) {
    Serial.print("Left: ");
    Serial.print(sensorIndex);
  }
  if (sensorIndex == 1) {
    Serial.print("Front: ");
    Serial.print(sensorIndex);
  }
  if (sensorIndex == 2) {
    Serial.print("Right: ");
    Serial.print(sensorIndex);
  }
  if (sensorIndex == 3) {
    Serial.print("Rear: ");
    Serial.print(sensorIndex);
  }
  Serial.print(" Distance: ");
  Serial.print(distanceMeasurement);
  Serial.print("  |  ");
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Print final avoidance directon
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void printFinalAvoidanceDirecton(int avoidanceDirection) {
  if (avoidanceDirection == 0) {
    Serial.println();
    Serial.println("Collision Avoidance Maneuver Left");
    Serial.println();
  }
  if (avoidanceDirection == 1) {
    Serial.println();
    Serial.println("Collision Avoidance Maneuver Front");
    Serial.println();
  }
  if (avoidanceDirection == 2) {
    Serial.println();
    Serial.println("Collision Avoidance Maneuver To Right");
    Serial.println();
  }
  if (avoidanceDirection == 3) {
    Serial.println();
    Serial.println("Collision Avoidance Maneuver To Backwards");
    Serial.println();
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Check the Aux for AutoPilot signal
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
boolean isAutoPilotMode() {
  aux = pulseIn(auxIn, HIGH);
  if (aux > 1500) return true;
  else return false;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Forward the aux signal to the Flight Controller
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void forwardAuxSignal() {
  analogWrite(auxOut, map(aux, 0, 2000, 0, 255));
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Forward the RC signals to the Flight Controller
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
// Run auto pilot loop: determine the sensor with the closest measurement 
// and forward collision avoidance manuever to the flight controller to preform.
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int runAutoPilot() {
  int incomingCollisionDirecton = -1;
  while (true) {

    if (!serialMonitorIsOpen) forwardRCSignalsToFlightController();
    // check sensors for triggering distances
    incomingCollisionDirecton = getClosestDirection();
    if (!serialMonitorIsOpen) forwardRCSignalsToFlightController();

    // a sensor was triggered by closest()
    if (incomingCollisionDirecton != -1) {

      if (preformMarkovAvoidance) {
        int markovDirection = getMarkovCollisionAvoidance(incomingCollisionDirecton);
        forwardCollisionAvoidanceManeuverToFlightController(markovDirection);  
      } else {
        forwardCollisionAvoidanceManeuverToFlightController((incomingCollisionDirecton + 2) % 4); // avoid collision with movement in opposite direction
      }
    }

    if (!isAutoPilotMode()) break;

  } // end of while

}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Get the direction with the first sensor that is closer than OUTER_DISTANCE_THRESHOLD_FOR_SENSORS_IN_CM.
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int getClosestDirection() {
  int initialDistanceMeasurement;
  int finalDistanceMeasurement;
  int incomingCollisionDirecton = -1;

  for (int sensorIndex = 0; sensorIndex <= 3; sensorIndex++)  {

    // get distance measurement quickly with one ping
    initialDistanceMeasurement = sensorArray[sensorIndex].ping() / US_ROUNDTRIP_CM;

    // print out distance measurement
    if (serialMonitorIsOpen) printSonarMeasurementsToConsole(sensorIndex, initialDistanceMeasurement);

    // check and exit if triggered
    if (distanceMeasurementIsWithinRange(initialDistanceMeasurement)) {

      if (!serialMonitorIsOpen) forwardRCSignalsToFlightController();
      if (!isAutoPilotMode()) break;

      if (!serialMonitorIsOpen)forwardRCSignalsToFlightController();
      // double check with with multiple pings
      finalDistanceMeasurement = sensorArray[sensorIndex].ping_median(NUMBER_OF_SONAR_BURSTS) / US_ROUNDTRIP_CM;
      if (!isAutoPilotMode()) break;
      if (!serialMonitorIsOpen)forwardRCSignalsToFlightController();

      // if it passes, it was not a false-positive measurement.
      // proceed to mark the direction as triggered.
      if (distanceMeasurementIsWithinRange(finalDistanceMeasurement)) {
        incomingCollisionDirecton = sensorIndex;
        break;

      }  // end final check
    } // end initial check

    if (!isAutoPilotMode()) break;

  } // end of for

  if (serialMonitorIsOpen) Serial.println();

  return incomingCollisionDirecton;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Check if distance measurement is within the target range.
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
boolean distanceMeasurementIsWithinRange(int distanceMeasurement) {
  if (distanceMeasurement < OUTER_DISTANCE_THRESHOLD_FOR_SENSORS_IN_CM && distanceMeasurement > INNER_DISTANCE_THRESHOLD_FOR_SENSORS_IN_CM && distanceMeasurement != 0) return true;
  else return false;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Get distance measurement from a giving direction
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int getDistanceMeasurement(int incomingDirection) {
  return sensorArray[incomingDirection].ping() / US_ROUNDTRIP_CM;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Markov collision avoidance algorithm.
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int getMarkovCollisionAvoidance(int incomingCollisionDirecton) {
  int randomSafeDirectionNumber;
  int oppositeDirection = (incomingCollisionDirecton + 2) % 4;
  int relativeLeftDirection = (incomingCollisionDirecton + 1) % 4;
  int relativateRightDirection = (incomingCollisionDirecton + 3) % 4;

  randomSafeDirectionNumber = random(0, 10); // 10 is exclusive

  if (serialMonitorIsOpen) Serial.println(randomSafeDirectionNumber);
  
  /* Give 60% chance of avoiding in the opposite direction */
  if (randomSafeDirectionNumber <= 5) {
    if (!distanceMeasurementIsWithinRange(getDistanceMeasurement(oppositeDirection))) return oppositeDirection;
  }
  
  /* Give 20% chance of avoiding in the relative left direction */
  if (randomSafeDirectionNumber == 6 || randomSafeDirectionNumber == 7) {
    if (!distanceMeasurementIsWithinRange(getDistanceMeasurement(relativeLeftDirection))) return relativeLeftDirection;
  }

  /* Give 20% chance of avoiding in the relative right direction */
  if (randomSafeDirectionNumber == 8 || randomSafeDirectionNumber == 9) {
    if (!distanceMeasurementIsWithinRange(getDistanceMeasurement(relativateRightDirection))) return relativateRightDirection;
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Forward collision avoidance manuever to the Flight Controler
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void forwardCollisionAvoidanceManeuverToFlightController(int avoidanceDirection) {
  switch (avoidanceDirection) {
    case 0:
      if (serialMonitorIsOpen) printFinalAvoidanceDirecton(avoidanceDirection);
      smoothAcceleration(rollOut, roll, 1490 + MAX_AUTOPILOT_MOVEMENT); // move left
      break;
    case 1:
      if (serialMonitorIsOpen) printFinalAvoidanceDirecton(avoidanceDirection);
      smoothAcceleration(pitchOut, pitch, 1490 + MAX_AUTOPILOT_MOVEMENT); // move forward
      break;
    case 2:
      if (serialMonitorIsOpen) printFinalAvoidanceDirecton(avoidanceDirection);
      smoothDeceleration(rollOut, roll, 1490 - MAX_AUTOPILOT_MOVEMENT); // move right
      break;
    case 3:
      if (serialMonitorIsOpen) printFinalAvoidanceDirecton(avoidanceDirection);
      smoothDeceleration(pitchOut, pitch, 1490 - MAX_AUTOPILOT_MOVEMENT); // move backward
      break;
    default:
      break;
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Smooth acceleration
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void smoothAcceleration(int outputPin, int currentSpeed, int finalSpeed) {
  boolean flag = true;
  for (int i = currentSpeed; i < finalSpeed; i++) {
    analogWrite(outputPin, map(i, 0, 2000, 0, 255));
    delayMicroseconds(500);
  }

  if (isAutoPilotMode()) delay(AUTOPILOT_MOVEMENT_DURATION_IN_MILLI);

  for (int i = finalSpeed; i > currentSpeed; i--) {
    analogWrite(outputPin, map(i, 0, 2000, 0, 255));
    delayMicroseconds(500);
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Smooth deceleration
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void smoothDeceleration(int outputPin, int currentSpeed, int finalSpeed) {
  boolean flag = true;
  for (int i = currentSpeed; i > finalSpeed; i--) {
    analogWrite(outputPin, map(i, 0, 2000, 0, 255));
    delayMicroseconds(500);
  }

  if (isAutoPilotMode()) delay(AUTOPILOT_MOVEMENT_DURATION_IN_MILLI);

  for (int i = finalSpeed; i < currentSpeed; i++) {
    analogWrite(outputPin, map(i, 0, 2000, 0, 255));
    delayMicroseconds(500);
  }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Main loop
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void loop() {
  if (isAutoPilotMode()) {
    forwardAuxSignal();
    runAutoPilot();
  } else {
    forwardAuxSignal();
    forwardRCSignalsToFlightController();
  }
}
