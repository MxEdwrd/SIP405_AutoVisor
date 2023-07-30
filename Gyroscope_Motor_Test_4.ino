//Libraries
#include <Wire.h>

//Variables
float rateX, rateY, rateZ;
float rateCalX, rateCalY, rateCalZ; //Declare Calibration Values
int rateCalNum; //Store Calibration Number

const int stepPin = 5;
const int dirPin = 4;
const int stepsPerf = 65; //Number of steps to perform per cycle
const int stepDelay = 4000; //Delay in milliseconds between steps (acts as speed)

//const int stopTime = 2000; //Time in milliseconds to stop between cycles, for testing

const int tiltVal = 300; //Degree to tilt head, with a combination of acceleration and degrees
int visorPos; //Current visor position

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
  int16_t gyroX=Wire.read()<<8 | Wire.read();
  int16_t gyroY=Wire.read()<<8 | Wire.read();
  int16_t gyroZ=Wire.read()<<8 | Wire.read();
  
  rateX=(float)gyroX/65.5;
  rateY=(float)gyroY/65.5;
  rateZ=(float)gyroZ/65.5;
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

  for(rateCalNum=0; rateCalNum < 2000; rateCalNum++){ //Perform the calibration measurements
    gyro_signals();
    rateCalX+=rateX;
    rateCalY+=rateY;
    rateCalZ+=rateZ;
    delay(1);
  }
  rateCalX/=2000; //Calculate the calibration values
  rateCalY/=2000;
  rateCalZ/=2000;

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  visorPos = 0; //Visor position assumed to start closed
}

void loop() {
  gyro_signals();
  rateX-=rateCalX;   //Correct measured values before displaying
  rateY-=rateCalY;
  rateZ-=rateCalZ;
  Serial.print("Roll rate (X) [degree/s] =");
  Serial.print(rateX);
  Serial.print("\n");
  Serial.print("Pitch rate (Y) [degree/s] =");
  Serial.print(rateY);
  Serial.print("\n");
  Serial.print("Yaw rate (Z) [degree/s] =");
  Serial.print(rateZ);
  Serial.print("\n");
  if (visorPos == 0){
    Serial.print("Visor is closed");
    //Serial.print(visorPos);
    Serial.print("\n");
  }
  if (visorPos ==1){
    Serial.print("Visor is open");
    //Serial.print(visorPos);
    Serial.print("\n");
  }
  Serial.print("\n \n");
  delay(100);
  if (rateX > (tiltVal*1) && visorPos == 0) {
      visorPos = 1;
      digitalWrite(dirPin, HIGH);
      for(int x = 0; x < stepsPerf; x++) {
          digitalWrite(stepPin,HIGH); 
          delayMicroseconds(stepDelay); 
          digitalWrite(stepPin,LOW); 
          delayMicroseconds(stepDelay); 
      }
  }
  if (rateX < (tiltVal*-1) && visorPos == 1) {
      visorPos = 0;
      digitalWrite(dirPin, LOW);
      for(int x = 0; x < stepsPerf; x++) {
          digitalWrite(stepPin,LOW); 
          delayMicroseconds(stepDelay); 
          digitalWrite(stepPin,HIGH); 
          delayMicroseconds(stepDelay); 
      }
  }
}
