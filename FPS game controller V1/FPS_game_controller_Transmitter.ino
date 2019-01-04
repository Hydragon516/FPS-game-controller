#include <SPI.h>
#include "RF24.h"
#include <Wire.h>
#include "Kalman.h"
  
int msg[6]={0,0,0,0,0,0};
int msg1 = 0;
int msg2 = 0;
int msg3 = 0;
int msg4 = 0;
int msg5 = 0;
  
int trig = 5;
int reload = 4;
  
const u8 X = 1;
const u8 Y = 2; 
const u8 S = 0;  
  
byte address[6] = "1Node";
RF24 radio(7,8);  // CE, CSN
  
int16_t gyroX, gyroZ;
  
int Sensitivity;
  
int delayi = 3;
  
uint32_t timer;
  
uint8_t i2cData[14]; // Buffer for I2C data
  
const uint8_t IMUAddress = 0x68;
const uint16_t I2C_TIMEOUT = 1000;
  
uint8_t i2cWrite(uint8_t registerAddress, uint8_t data, bool sendStop) {
  return i2cWrite(registerAddress,&data,1,sendStop);
  }
  
uint8_t i2cWrite(uint8_t registerAddress, uint8_t* data, uint8_t length, bool sendStop) {
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  Wire.write(data, length);
  return Wire.endTransmission(sendStop);
  }
  
uint8_t i2cRead(uint8_t registerAddress, uint8_t* data, uint8_t nbytes) {
  uint32_t timeOutTimer;
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  if(Wire.endTransmission(false))
     return 1; // Error in communication
   Wire.requestFrom(IMUAddress, nbytes,(uint8_t)true);
   for(uint8_t i = 0; i < nbytes; i++) {
     if(Wire.available())
       data[i] = Wire.read();
     else {
       timeOutTimer = micros();
       while(((micros() - timeOutTimer) < I2C_TIMEOUT) && !Wire.available());
       if(Wire.available())
         data[i] = Wire.read();
       else
         return 2;
     }
   }
   return 0;
 }
  
 void setup() {
   Serial.begin(9600);
  
   pinMode(5, INPUT);
   pinMode(4, INPUT);
  
   radio.begin();
   radio.openWritingPipe(address);
   radio.stopListening();
  
   Wire.begin();
  
   i2cData[0] = 7; // Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
   i2cData[1] = 0x00; // Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
   i2cData[3] = 0x00; // Set Accelerometer Full Scale Range to ±2g
  
   while(i2cWrite(0x19,i2cData,4,false)); // Write to all four registers at once
   while(i2cWrite(0x6B,0x01,true)); // PLL with X axis gyroscope reference and disable sleep mode
   while(i2cRead(0x75,i2cData,1));
  
   if(i2cData[0] != 0x68) { // Read "WHO_AM_I" register
     Serial.print(F("Error reading sensor"));
     while(1);
   }
  
   delay(100); // Wait for sensor to stabilize
   while(i2cRead(0x3B,i2cData,6));
   timer = micros();
 }
  
 void loop() {
   Sensitivity = map(analogRead(S), 0, 1023, 800, 200);
   int trig = digitalRead(5);
   int reload = digitalRead(4);
  
   if(trig==0){
     msg3 = 0;
     }
   else{
     msg3 = 1;
     }
  
   if(reload==0){
     msg4 = 1;
     }
   else{
     msg4 = 0;
     }
  
   while(i2cRead(0x3B,i2cData,14));
   gyroX = ((i2cData[8] << 8) | i2cData[9]);
   gyroZ = ((i2cData[12] << 8) | i2cData[13]);
 
   gyroX = gyroX / Sensitivity / 1.1  * -1;
   gyroZ = gyroZ / Sensitivity  * -1;
  
  
   Serial.print("\t");
   Serial.print(gyroX);
   Serial.print(gyroZ);
  
   msg1=gyroX;
   msg[0]=msg1;
   
   msg2=gyroZ;
   msg[1]=msg2;
  
   msg[2]=msg3;
  
   msg[3]=analogRead(X);
   msg[4]=analogRead(Y);
  
   msg[5]=msg4;
   
   radio.write(&msg, sizeof(msg));
  
   Serial.print(msg3);
  
   Serial.print(analogRead(X));
   Serial.print(analogRead(Y));
   Serial.print(Sensitivity);
  
   Serial.print(msg4);
  
   Serial.print("\r\n");
  
   delay(delayi);
  
 }
