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

#define sensorDetectionDistanceInFeet 4
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
// Input Pins for Ultrasonic Sensors
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define s1 40 // sensor left
#define s2 42 // sensor front
#define s3 44 // sensor right
#define s4 46 // sensor rear

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
  
  randomSeed(analogRead(0)); // for random safety direction
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
// Return ping sensor with closest result
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int runAutoPilotAndReturnClosest() {
  int minimumDistanceThresthold = sensorDetectionDistanceInFeet * 12; // convert to ft
  unsigned long distanceMeasurement;
  int localMinimum = 10000000;
  int randomSafeDirectionNumber;
  int result = -1;

  int sensorIndex = 0;
  while (true) {
    
    /*  Get the median measurement from a bursts of five pings */
    distanceMeasurement = sensors[sensorIndex].ping_median(5) / US_ROUNDTRIP_IN;

    /* Check if distanceMeasurement is out of range and therefore throwaway */
    if (distanceMeasurement == 0) {
      if (serialMonitorIsOpen) Serial.println(sensorIndex + " - Out Of Range");
      continue;
    }

    /* Failsafe mechanism for exiting autopilot */
    if (!checkAuxForAutoPilotSignal()) {
      if (serialMonitorIsOpen) Serial.println("Break out of closest");
      break;
    }
    
    /* Update minimumDistanceThresthold */
    if (distanceMeasurement < localMinimum) {
      localMinimum = distanceMeasurement;
      result = sensorIndex;
    }
    
    /* Increment sensor index or recycle index back to 0 and reset localMinimum to a big number*/
    if (sensorIndex == 4) {
      if (localMinimum < minimumDistanceThresthold) break;
      localMinimum = 10000000;
      sensorIndex = 0;
      result = -1;
    } else {
      sensorIndex++;
    }
    
    forwardRCSignalsToFlightController();
  }
  
  if (chooseRandomSafetyDirection) {
    while (true) {
      randomSafeDirectionNumber = random(0, 4);
        if (randomSafeDirectionNumber != result) {
          if (serialMonitorIsOpen) Serial.println(randomSafeDirectionNumber);
          return randomSafeDirectionNumber;
        }
    }
    return result;
  } else {
    return (result+2) % 4; // get opposite direction from the sensor detected i.e. shift two sensors
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
  if (checkAuxForAutoPilotSignal()) {
    int closest = runAutoPilotAndReturnClosest();
    switch (closest) {
      case 0:
        smoothAcceleration(rollOut, roll, 1650); // move left
        break;
      case 1:
        smoothAcceleration(pitchOut, pitch, 1650); // move forward
        break;
      case 2:
        smoothDeceleration(rollOut, roll, 1350); // move right
        break;
      case 3:
        smoothDeceleration(pitchOut, pitch, 1350); // move backward
        break;
      default:
        break;
    }
    forwardAuxSignal();
  } else {
    forwardRCSignalsToFlightController();
  }
}
