int ch1; // Here's where we'll keep our channel values
int ch2;
int ch3;
int ch4;
int ch5;
//int ch6;

void setup() {

  pinMode(2, INPUT); // Set our input pins as such
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT); // aux
//  pinMode(7, INPUT);
  
  pinMode(8, OUTPUT); // Set our output pins as such
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
//  pinMode(13, OUTPUT);

  Serial.begin(115200); // Pour a bowl of Serial

}

void loop() {

  ch1 = pulseIn(2, HIGH, 25000); // Read the pulse width
  ch2 = pulseIn(3, HIGH, 25000); 
  ch3 = pulseIn(4, HIGH, 25000);
  ch4 = pulseIn(5, HIGH, 25000);
  ch5 = pulseIn(6, HIGH, 25000);
//  ch6 = pulseIn(7, HIGH, 25000);
   
//  analogWrite(8, map(ch1,0,2000,0,255));
//  analogWrite(9, map(ch2,0,2000,0,255));
//  analogWrite(10, map(ch3,0,2000,0,255));
//  analogWrite(11, map(ch4,0,2000,0,255));
//  analogWrite(12, map(ch5,0,2000,0,255));
//  analogWrite(13, map(ch6,0,2000,0,500));
//  
  Serial.print("Channel 1: ");
  Serial.print(ch1);
  Serial.print(" | ");

  Serial.print("Channel 2:");
  Serial.print(ch2);
  Serial.print(" | ");
  
  Serial.print("Channel 3:");
  Serial.print(ch3);
  Serial.print(" | ");
  
  Serial.print("Channel 4:");
  Serial.print(ch4);
  Serial.print(" | ");
  
  Serial.print("Aux 1: ");
  Serial.print(ch5);
  Serial.println();
//  
//  Serial.print("Aux 2: ");
//  Serial.println(ch6);
}
