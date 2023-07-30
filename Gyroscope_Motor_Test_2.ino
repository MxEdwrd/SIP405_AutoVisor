//Libraries
#include <Wire.h>

//Variables
float RateX, RateY, RateZ;
float RateCalX, RateCalY, RateCalZ;  //Declare Calibration Values
int RateCalNum;                               //Store Calibration Number

#define STEPPER_PIN_1 4
#define STEPPER_PIN_2 5
#define STEPPER_PIN_3 6
#define STEPPER_PIN_4 7
int steps;
int stepDelay = 3;  //Delay between step phases

int tiltDeg = 300;  //Degree to tilt head, is some combination of degree and acceleration

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
  
  RateRoll=(float)GyroX/65.5;
  RatePitch=(float)GyroY/65.5;
  RateYaw=(float)GyroZ/65.5;
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
    RateCalRoll+=RateRoll;
    RateCalPitch+=RatePitch;
    RateCalYaw+=RateYaw;
    delay(1);
  }
  RateCalRoll/=2000;  //Calculate the calibration values
  RateCalPitch/=2000;
  RateCalYaw/=2000;

  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);
}

void loop() {
  gyro_signals();
  RateRoll-=RateCalRoll;   //Correct measured values before displaying
  RatePitch-=RateCalPitch;
  RateYaw-=RateCalYaw;
  Serial.print("Roll rate (X) [degree/s] =");
  Serial.print(RateRoll);
  Serial.print(" Pitch rate (Y) [degree/s] =");
  Serial.print(RatePitch);
  Serial.print(" Yaw rate (Z) [degree/s] =");
  Serial.print(RateYaw);
  Serial.print("\n \n");
  delay(100);
  if (RatePitch > liftNum){
    for (; steps < 1024; steps++){
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        delay(stepDelay);
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        delay(stepDelay);
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        delay(stepDelay);
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        delay(stepDelay);
        }
        steps = 0;
      }
}
