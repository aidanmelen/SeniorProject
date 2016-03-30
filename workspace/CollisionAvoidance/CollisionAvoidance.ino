/*
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
 * boolean checkAuxForAutoPilotSignal()
 * void forwardAuxSignal()
 * void forwardRCSignalsToFlightControllerToFlightController()
 * int runAutoPilotAndReturnClosest()
 * void smoothAcceleration(outputPin, currentSpeed, finalSpeed)
 * void smoothDeceleration(outputPin, currentSpeed, finalSpeed)
 * void loop()
 */

#include <NewPing.h>

#define OUTER_DISTANCE_THRESHOLD_FOR_SENSORS_IN_CM 80 // 1.5 ft
#define MAX_PING_DISTANCE_IN_CM 200
#define NUMBER_OF_SONAR_BURSTS 5
#define CAPACITY_OF_AUTOPILOT_MOVEMENTS 100

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
NewPing sensors[] = {p1, p2, p3, p4};

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Setup
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void setup() {
  if (serialMonitorIsOpen == true) {
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
// Check Aux For AutoPilot Signal
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
boolean checkAuxForAutoPilotSignal() {
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
// Run Auto Pilot Loop: determine sensor with closest measurement
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int runAutoPilot() {
  unsigned long distanceMeasurement;
  unsigned long doubleCheckDistanceMeaseurement;
  int temporaryMinimum;
  int temporaryAvoidanceDirection;
  boolean avoidanceDirectionWasSet;
  
  while (true) {
    temporaryMinimum = OUTER_DISTANCE_THRESHOLD_FOR_SENSORS_IN_CM;
    temporaryAvoidanceDirection = -1;
    avoidanceDirectionWasSet = false;
    for (int sensorIndex = 0; sensorIndex < 4; sensorIndex++) {
      
      forwardRCSignalsToFlightController();
      
      /*  Do a burst of 5 ping measurements, discard out of range pings and return median in microseconds */
      distanceMeasurement = sensors[sensorIndex].ping_median(NUMBER_OF_SONAR_BURSTS) / US_ROUNDTRIP_IN;

      forwardRCSignalsToFlightController();
      
      /* Failsafe mechanism for exiting autopilot */
      if (!checkAuxForAutoPilotSignal()) break;
      
      /* Check if distanceMeasurement is out of range and therefore throwaway */
      if (distanceMeasurement == 0) continue;
      
      /* Update temporaryAvoidanceDirection and resulting direction */
      if (distanceMeasurement < temporaryMinimum) {
        temporaryMinimum = distanceMeasurement;
        temporaryAvoidanceDirection = sensorIndex;
        avoidanceDirectionWasSet = true;
      }     
    } // end for
    
    /* Failsafe mechanism for exiting autopilot */
    if (!checkAuxForAutoPilotSignal()) {
       if (serialMonitorIsOpen) Serial.println("Break Out");
      break;
    }
    
    /* Avoid a collision by moving the quadcopter is a safe direction */
    /* shift two sensor i.e the opposite direction of the triggering sensor */
    if (avoidanceDirectionWasSet) {
      if (serialMonitorIsOpen) Serial.println("Avoidance Was Called");
      
      doubleCheckDistanceMeaseurement = sensors[temporaryAvoidanceDirection].ping_median(NUMBER_OF_SONAR_BURSTS) / US_ROUNDTRIP_IN;
      if (doubleCheckDistanceMeaseurement < temporaryMinimum) {
        forwardCollisionAvoidanceMovementToFlightControler((temporaryAvoidanceDirection + 2) % 4);
      }
    }
  } // end while
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Forward Collision Avoidance Movement To The Flight Controler
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void forwardCollisionAvoidanceMovementToFlightControler(int avoidanceDirection) {
  switch (avoidanceDirection) {
      case 0:
        smoothAcceleration(rollOut, roll, roll + CAPACITY_OF_AUTOPILOT_MOVEMENTS); // move left
        break;
      case 1:
        smoothAcceleration(pitchOut, pitch, pitch + CAPACITY_OF_AUTOPILOT_MOVEMENTS); // move forward
        break;
      case 2:
        smoothDeceleration(rollOut, roll, roll - CAPACITY_OF_AUTOPILOT_MOVEMENTS); // move right
        break;
      case 3:
        smoothDeceleration(pitchOut, pitch, roll - CAPACITY_OF_AUTOPILOT_MOVEMENTS); // move backward
        break;
      default:
        break;
    }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Main Loop
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void loop() {
  if (checkAuxForAutoPilotSignal()) {
    runAutoPilot();
  } else {
    forwardRCSignalsToFlightController();
  }
}
