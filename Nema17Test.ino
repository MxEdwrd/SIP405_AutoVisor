#include <Wire.h>

const int stepPin = 5;
const int dirPin = 4;
const int stepsPerRev = 75;
const int stepDelay = 6000; 

const int stopTime = 3000;

int visorPos;

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void loop() {
  delay(stopTime);
  visorPos = 1;
  Serial.print("Visor Position: ");
  Serial.print(visorPos);
  Serial.print("\n");
  digitalWrite(dirPin, HIGH);
  for(int x = 0; x < stepsPerRev; x++){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
  delay(stopTime);

  visorPos = 0;
  Serial.print("Visor Position: ");
  Serial.print(visorPos);
  Serial.print("\n");
  digitalWrite(dirPin, LOW);
  for(int x = 0; x < stepsPerRev; x++){
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
  }
  delay(stopTime);
}
