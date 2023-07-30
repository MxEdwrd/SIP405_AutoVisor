//Libraries
#include <Wire.h>

//Variables

//Accelerometer
float accX, accY, accZ;
float angleRoll, anglePitch;

//Gyroscope
float rateX, rateY, rateZ;
float rateCalX, rateCalY, rateCalZ; //Declare Calibration Values
int rateCalNum; //Store Calibration Number

const int stepPin = 5;
const int dirPin = 4;
const int stepsPerf = 80; //Number of steps to perform per cycle
const int stepDelay = 1500; //Delay in milliseconds between steps (acts as speed)

//const int stopTime = 2000; //Time in milliseconds to stop between cycles, for testing

const int tiltVal = 125; //Degree to tilt head, with a combination of acceleration and degrees
int visorPos; //Current visor position
int autoCloseVal; //AutoClose value, to detect if crash occurred

//Functions
void gyro_signals(void){
  Wire.beginTransmission(0x68);
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();

  //Accelerometer Setup
  Wire.beginTransmission(0x68); //Configure Accelerometer Output
  Wire.write(0x1C);
  Wire.write(0x08); //(0x00 = +/-2g, 0x08 = +/-4g, 0x10 = +/-8g, 0x18 = +/-16g)
  Wire.endTransmission();
  
  Wire.beginTransmission(0x68); //Pull Accelerometer Measurements
  Wire.write(0x3B);
  Wire.endTransmission();
  
  Wire.requestFrom(0x68,6);
  int16_t accXLSB=Wire.read()<<8 | Wire.read();
  int16_t accYLSB=Wire.read()<<8 | Wire.read();
  int16_t accZLSB=Wire.read()<<8 | Wire.read();

  accX=(float)accXLSB/8192-0.02; //Convert Measurements to Physical Values (g)
  accY=(float)accYLSB/8192; //(2g = 16384, 4g = 8192, 8g = 4096, 16g = 2048)
  accZ=(float)accZLSB/8192-0.06;

  angleRoll=atan(accY/sqrt(accX*accX+accZ*accZ))*1/(3.142/180);    //Calculate Absolute Angles
  anglePitch=-atan(accX/sqrt(accY*accY+accZ*accZ))*1/(3.142/180);

  //Gyroscope Setup
  Wire.beginTransmission(0x68); //Configure Gyroscope Output and Pull Rotation Rate Measurements
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

  autoCloseVal = 0; //Start with autoclose emergency setting off
}

void loop() {
  gyro_signals();
  rateX-=rateCalX;   //Correct measured values before displaying
  rateY-=rateCalY;
  rateZ-=rateCalZ;

  if (autoCloseVal == 0){
    Serial.print("Roll rate (X) [degree/s] =");
    Serial.print(rateX);
    Serial.print("\n");
    Serial.print("Pitch rate (Y) [degree/s] =");
    Serial.print(rateY);
    Serial.print("\n");
    Serial.print("Yaw rate (Z) [degree/s] =");
    Serial.print(rateZ);
    Serial.print("\n\n");
  
    Serial.print("Acceleration X [g]= ");     //For Calibrating Acceleration Values on MPU6050
    Serial.print(accX);
    Serial.print("\n");
    Serial.print("Acceleration Y [g]= ");
    Serial.print(accY);
    Serial.print("\n");
    Serial.print("Acceleration Z [g]= ");
    Serial.print(accZ);
    Serial.print("\n\n");
    
    Serial.print("Roll Angle [°]= ");
    Serial.print(angleRoll);
    Serial.print("\n");
    Serial.print("Pitch Angle [°]= ");
    Serial.print(anglePitch);
    Serial.print("\n\n");
    
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
    //Serial.print(autoCloseVal);
    Serial.print("\n\n");
    
    
    if (rateX < (tiltVal*-1) && visorPos == 0) {
        visorPos = 1;
        digitalWrite(dirPin, HIGH);
        for(int x = 0; x < stepsPerf; x++) {
            digitalWrite(stepPin,HIGH); 
            delayMicroseconds(stepDelay); 
            digitalWrite(stepPin,LOW); 
            delayMicroseconds(stepDelay); 
        }
    }
    if (rateX > (tiltVal*1) && visorPos == 1) {
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
  /*if (accX >= 0.90 || accX <=-0.90 || autoCloseVal == 1){
    autoCloseVal = 1;
    if (visorPos == 1) {
      visorPos = 0;
      digitalWrite(dirPin, LOW);
        for(int x = 0; x < stepsPerf; x++) {
            digitalWrite(stepPin,LOW); 
            delayMicroseconds(stepDelay); 
            digitalWrite(stepPin,HIGH); 
            delayMicroseconds(stepDelay); 
        }
    }
    Serial.print("Emergency AutoClose Activated");
    Serial.print("\n\n");
  }*/
  delay(100);
}
