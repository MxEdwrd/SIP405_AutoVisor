//Libraries
#include <Wire.h> //Wire library for I2C

//Variables
float AccX, AccY, AccZ; //Define accelerometer variables
float intVel = 0;
unsigned long startTime = 0; // Store the start time
unsigned long elapsedTime = 0; // Declare elapsedTime variable

//Functions
void signals(void) {
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

  AccX=(float)AccXLSB/8192-0.02;       //Convert Measurements to Physical Values (g)
  AccY=(float)AccYLSB/8192;       //(2g = 16384, 4g = 8192, 8g = 4096, 16g = 2048)
  AccZ=(float)AccZLSB/8192-0.07;
}

float VelCalc(float intVel, float AccX, float elapsedTime) {
  float velocity = intVel + (AccX * elapsedTime);
  return velocity * 2.23694;
}

float trimToDec(float value) {
  float trimmedVal = floor(value*10)/10.0;
  return trimmedVal;
}

void timeCount() {
  /*unsigned long currentTime = millis(); // Get the current time

  // Calculate the hours, minutes, and seconds
  unsigned long hours = (currentTime / 3600000) % 12;
  unsigned long minutes = (currentTime / 60000) % 60;
  unsigned long seconds = (currentTime / 1000) % 60;
  // Reset the clock if it reaches 11:59:59
  if (hours == 11 && minutes == 59 && seconds == 59) {
    currentTime = 0;
  }
  
  // Print the time to the serial monitor with leading zeros
  if (hours < 10) {
    Serial.print("0");
  }
  Serial.print(hours);
  Serial.print(":");
  
  if (minutes < 10) {
    Serial.print("0");
  }
  Serial.print(minutes);
  Serial.print(":");
  
  if (seconds < 10) {
    Serial.print("0");
  }
  Serial.println(seconds);
  */
  elapsedTime = (millis() - startTime) / 1000;

  Serial.print("Seconds Passed: ");
  Serial.println(elapsedTime);
}

void setup() {
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
}

void loop() {
  
  timeCount();
  signals();
  float trimmedVal = trimToDec(AccX);

  AccX = trimmedVal;

  float velocity = VelCalc(intVel, AccX, elapsedTime);

  Serial.print("Acceleration X [g]= ");     //For Calibrating Acceleration Values on MPU6050
  Serial.println(AccX);
  /*Serial.print(" Acceleration Y [g]= ");
  Serial.print(AccY);
  Serial.print(" Acceleration Z [g]= ");
  Serial.print(AccZ);
  */
  Serial.print("Velocity: ");
  Serial.print(velocity);
  Serial.println(" mph");

  Serial.print("\n \n");

  delay(1000); // Delay for 1 second before printing the next time
}
