//Libraries
#include <Wire.h>

//Variables
float RateX, RateY, RateZ;
float RateCalX, RateCalY, RateCalZ;  //Declare Calibration Values
int RateCalNum;                               //Store Calibration Number

const int stepPin = 5;
const int dirPin = 4;
const int stepsPerRev = 1024;
const int stepDelay = 3000;

const int tiltVal = 150;  //Degree to tilt head, is some combination of degree and acceleration

//Functions
void gyro_signals(void){
  Wire.beginTransmission(0x68);
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();

  Wire.beginTransmission(0x68);
  Wire.write(0x1B);
  Wire.write(0x08);
  Wire.endTransmission();

  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission();

  Wire.requestFrom(0x68,6);
  int16_t GyroX=Wire.read()<<8 | Wire.read();
  int16_t GyroY=Wire.read()<<8 | Wire.read();
  int16_t GyroZ=Wire.read()<<8 | Wire.read();
  
  RateX=(float)GyroX/65.5;
  RateY=(float)GyroY/65.5;
  RateZ=(float)GyroZ/65.5;
}

void setup(){
  Serial.begin(57600);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  Wire.setClock(400000);
  Wire.begin();
  delay(250);
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  for(RateCalNum=0; RateCalNum < 2000; RateCalNum++){ //Perform the calibration measurements
    gyro_signals();
    RateCalX+=RateX;
    RateCalY+=RateY;
    RateCalZ+=RateZ;
    delay(1);
  }
  RateCalX/=2000;  //Calculate the calibration values
  RateCalY/=2000;
  RateCalZ/=2000;

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void loop() {
  gyro_signals();
  RateX-=RateCalX;   //Correct measured values before displaying
  RateY-=RateCalY;
  RateZ-=RateCalZ;
  Serial.print("Roll rate (X) [degree/s] =");
  Serial.print(RateX);
  Serial.print(" Pitch rate (Y) [degree/s] =");
  Serial.print(RateY);
  Serial.print(" Yaw rate (Z) [degree/s] =");
  Serial.print(RateZ);
  Serial.print("\n \n");
  delay(100);
  if (RateX > (tiltVal*1)) {
      digitalWrite(dirPin, HIGH);
      for(int x = 0; x < stepsPerRev; x++) {
          digitalWrite(stepPin,HIGH); 
          delayMicroseconds(stepDelay); 
          digitalWrite(stepPin,LOW); 
          delayMicroseconds(stepDelay); 
      }
  }
  if (RateX < (tiltVal*-1)) {
      digitalWrite(dirPin, LOW);
      for(int x = 0; x < stepsPerRev; x++) {
          digitalWrite(stepPin,LOW); 
          delayMicroseconds(stepDelay); 
          digitalWrite(stepPin,HIGH); 
          delayMicroseconds(stepDelay); 
      }
  }
}
