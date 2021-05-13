#include <ArduinoBLE.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  Serial.println("Hello world.");
  pinMode(LED_BUILTIN, OUTPUT);
}

int ix = 0;

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, ix % 2 ? LOW: HIGH);
  ix++;

  // wait for a second
  delay(100);

  if((ix % 100) == 0) Serial.println("Still here");

}
