//Libraries
#include <Wire.h>

//Variables
float RateRoll, RatePitch, RateYaw;    //Roll = X, Pitch = Y, Yaw = Z
//float RateCalX, RateCalY, RateCalZ;  //Declare Calibration Values
//int RateCalNum;                      //Store Calibration Number
float AccX, AccY, AccZ;              //Define Accelerometer Values
float AngleRoll, AnglePitch;                //Define X and Y angles

float LoopTimer;
/*const int stepPin = 5;
const int dirPin = 4;
const int stepsPerRev = 2048;
const int stepDelay = 1000;

//const int tiltVal = 300;  //Degree to tilt head, is some combination of degree and acceleration
*/
//Functions
void gyro_signals(void){
  Wire.beginTransmission(0x68);
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();

  //Accelerometer
  Wire.beginTransmission(0x68);     //Configure Accelerometer Output
  Wire.write(0x1C);
  Wire.write(0x08);                 //(0x00 = +/-2g, 0x08 = +/-4g, 0x10 = +/-8g, 0x18 = +/-16g)
  Wire.endTransmission();
  Wire.beginTransmission(0x68);     //Pull Accelerometer Measurements
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(0x68,6);
  int16_t AccXLSB=Wire.read()<<8 | Wire.read();
  int16_t AccYLSB=Wire.read()<<8 | Wire.read();
  int16_t AccZLSB=Wire.read()<<8 | Wire.read();

  //Gyroscope
  Wire.beginTransmission(0x68);     //Configure Gyroscope Output and Pull Rotation Rate Measurements
  Wire.write(0x1B);
  Wire.write(0x8);
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

  AccX=(float)AccXLSB/8192-0.02;       //Convert Measurements to Physical Values (g)
  AccY=(float)AccYLSB/8192;       //(2g = 16384, 4g = 8192, 8g = 4096, 16g = 2048)
  AccZ=(float)AccZLSB/8192-0.06;

  AngleRoll=atan(AccY/sqrt(AccX*AccX+AccZ*AccZ))*1/(3.142/180);    //Calculate Absolute Angles
  AnglePitch=-atan(AccX/sqrt(AccY*AccY+AccZ*AccZ))*1/(3.142/180);
}

void setup(){
  Serial.begin(9600);            //Communication with Gyroscope and Calibration
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  Wire.setClock(400000);
  Wire.begin();
  delay(250);
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  /*for(RateCalNum=0; RateCalNum < 2000; RateCalNum++){ //Perform the calibration measurements
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
  */
}

void loop() {
  gyro_signals();
  /*RateX-=RateCalX;   //Correct measured values before displaying
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
  if (RateY > (tiltVal*1)) {
      digitalWrite(dirPin, HIGH);
      for(int x = 0; x < stepsPerRev; x++) {
          digitalWrite(stepPin,HIGH); 
          delayMicroseconds(stepDelay); 
          digitalWrite(stepPin,LOW); 
          delayMicroseconds(stepDelay); 
      }
  }
  if (RateY < (tiltVal*-1)) {
      digitalWrite(dirPin, LOW);
      for(int x = 0; x < stepsPerRev; x++) {
          digitalWrite(stepPin,LOW); 
          delayMicroseconds(stepDelay); 
          digitalWrite(stepPin,HIGH); 
          delayMicroseconds(stepDelay); 
      }
  }
  */
  Serial.print("Roll Angle [°]= ");
  Serial.print(AngleRoll);
  Serial.print(" Pitch Angle [°]= ");
  Serial.print(AnglePitch);
  delay(50);
  /*Serial.print("Acceleration X [g]= ");     //For Calibrating Acceleration Values on MPU6050
  Serial.print(AccX);
  Serial.print(" Acceleration Y [g]= ");
  Serial.print(AccY);
  Serial.print(" Acceleration Z [g]= ");
  Serial.print(AccZ);
  delay(50);
  */
}
