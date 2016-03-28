#include <NewPing.h>

int ledPin = 13;

#define s1 40 // sensor left
#define s2 42 // sensor front
#define s3 44 // sensor right
#define s4 46 // sensor rear

char *pingString[] = {"Left","Front","Right","Rear"}; 

NewPing p1(s1, s1); // pin are the same for both trig and echo
NewPing p2(s2, s2);
NewPing p3(s3, s3);
NewPing p4(s4, s4);
NewPing sensors[] = {p1,p2,p3,p4};

unsigned long pingResult;

void setup() {
Serial.begin(115200);
Serial.println("Closest Test");

pinMode(ledPin, OUTPUT);
}


void loop() {
  int minimum = 2; // 5ft
  int closestPingIdentifier = -1;
  
  for(int i = 0; i<4; i++) {
    delay(5);
    pingResult = sensors[i].ping() / US_ROUNDTRIP_IN; // get distance from sensor
    
    if(pingResult < minimum && pingResult != 0) { // ping sensors return 0 when out of range so we ignore it. 
      minimum = pingResult; 
      closestPingIdentifier = i;
    }
  }
  
  // print results
  if (minimum<2) {
    Serial.print(pingString[closestPingIdentifier]);
    Serial.print(": ");
    Serial.println(minimum); 
    
    digitalWrite(ledPin,HIGH);
  } else {
    Serial.println("None"); 
    digitalWrite(ledPin,LOW);
  }
}

