#include <NewPing.h>

#define s1 40 // sensor left
#define s2 42 // sensor front
#define s3 44 // sensor right
#define s4 46 // sensor rear

NewPing p1(s1, s1); // pin are the same for both trig and echo
NewPing p2(s2, s2);
NewPing p3(s3, s3);
NewPing p4(s4, s4);

unsigned int p1Out; // store distance measurements
unsigned int p2Out;
unsigned int p3Out;
unsigned int p4Out;

void setup() {
Serial.begin(115200);
}

void loop() {

  delay(20); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay

  p1Out = p1.ping()/ US_ROUNDTRIP_IN; // Send ping, get ping time in microseconds
  Serial.print("Left :");  
  Serial.print(p1Out);
  Serial.print("  |  ");
  
  delay(20);
  
  p2Out = p2.ping()/ US_ROUNDTRIP_IN; // and convert time into distance(Inches) using the newPing library
  Serial.print("Front :");  
  Serial.print(p2Out);
  Serial.print("  |  ");
  
  delay(20);
  
  p3Out = p3.ping()/ US_ROUNDTRIP_IN;
  Serial.print("Right :");  
  Serial.print(p3Out);
  Serial.print("  |  ");
  
  delay(20);
  
  p4Out = p4.ping()/ US_ROUNDTRIP_IN;
  Serial.print("Rear :");  
  Serial.print(p4Out);
  Serial.print("  |  ");
  
  Serial.println();
}

