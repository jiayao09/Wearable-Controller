#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Servo.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
Adafruit_MPU6050 mpu;


#define CE_pin 9
#define CSN_pin 10

RF24 radio(CE_pin, CSN_pin);

const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int finger;
int fingerinital;
float x, y, z;
float pre_x, pre_y, pre_z;
int c = 0;
float* arr;
int value = 0;
char control;
const byte address[6] = "00001";
char sendDatas;

void setup() {
  Serial.begin(9600);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  
  display.display();
  delay(5000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  //display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Starting calibration...")); 
  display.display();
  
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission

  Serial.println("Found a MPU-6050 sensor");
  arr = calculate_IMU_error();
  // Print the error values on the Serial Monitor
  AccErrorX = arr[0];
  AccErrorY = arr[1];
  GyroErrorX = arr[2];
  GyroErrorY = arr[3];
  GyroErrorZ = arr[4];
   
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  delay(100);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Calibration complete!");
  delay(5000); 
  display.display();
  delay(500);
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);
  
  fingerinital = analogRead(A0);
}
void loop() {
 // === Read acceleromter data === //
  display.clearDisplay();
  display.setCursor(0,0);
  delay(100);
  finger = analogRead(A0);
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 8192.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 8192.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 8192.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorX; // AccErrorX See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorY; // AccErrorY
  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  GyroX = GyroX - GyroErrorX; // GyroErrorX
  GyroY = GyroY - GyroErrorY; // GyroErrorY
  GyroZ = GyroZ - GyroErrorZ; // GyroErrorZ 
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;
  // Complementary filter - combine acceleromter and gyro angle values
  gyroAngleX = 0.96 * gyroAngleX + 0.04 * accAngleX;
  gyroAngleY = 0.96 * gyroAngleY + 0.04 * accAngleY;
  roll = gyroAngleX;
  pitch = gyroAngleY;  

  display.println("Accelerometer - m/s^2");
  display.print(GyroX, 1);
  display.print(", ");
  display.print(GyroY, 1);
  display.print(", ");
  display.print(GyroZ, 1);
  display.println("");

  display.println("Gyroscope - rps");
  display.print(yaw, 1);
  display.print(", ");
  display.print(roll, 1);
  display.print(", ");
  display.print(pitch, 1);
  display.println("");

  display.display();
  delay(100);
  x = roll-pre_x;
  y = pitch-pre_y;
  z = yaw-pre_z;
  
  if(abs(x) > 10 && abs(z) < 20){
    Serial.print("x change = ");
    Serial.println(x);
    value = map(abs(x), 10, 80, 0, 180);   // C10
    if(x > 0){
      control = 'u';
      Serial.print("up");
      Serial.println(value);
    }else{
      control = 'd';
      Serial.print("down");
      Serial.println(value);
    }
    sendDatas = control;
    radio.write(&sendDatas, sizeof(sendDatas));
    radio.write(&value, sizeof(value));
    delay(100);
  }
  if(abs(y) > 10 && abs(z) < 20){  //f9
    Serial.print("y change = ");
    Serial.println(y);
    value = map(abs(y), 10, 80, 0, 180);
    if(y > 0){
      control = 'f';
      Serial.print("front");
      Serial.println(value);
    }else{
      control = 'b';
      Serial.print("back");
      Serial.println(value);
    }
    sendDatas = control;
    radio.write(&sendDatas, sizeof(sendDatas));
    radio.write(&value, sizeof(value));
    delay(100);    
  }
  
  if(abs(z)>20){
    Serial.print("z change = ");
    Serial.println(yaw-pre_z);
    value = map(abs(z), 20, 70, 0, 180);
    if(z > 0){
      control = 'l';
      Serial.print("Left");
      Serial.println(value);
    }else{
      control = 'r';
      Serial.print("right");
      Serial.println(value);
    }
    sendDatas = control;
    radio.write(&sendDatas, sizeof(sendDatas));
    radio.write(&value, sizeof(value));
    delay(100);    
  }
  pre_x = roll;
  pre_y = pitch;
  pre_z = yaw;
  
  if (abs(fingerinital - finger) > 10){
   value = map(abs(fingerinital - finger), 5, 100, 0, 180);
   if(fingerinital > finger){
     control = 'c';
     Serial.print("close");
     Serial.println(value);
    }else if (fingerinital < finger){
      control = 'o';
      Serial.print("open");
      Serial.println(value);
    }   
   sendDatas = control;
   radio.write(&sendDatas, sizeof(sendDatas));
   radio.write(&value, sizeof(value));
   delay(100);  
  }
  fingerinital = finger;
  delay(100);  
}



float* calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  arr = new float[5];
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    float cal1 = ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  // Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all reading
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;

  
  arr[0] = AccErrorX;
  arr[1] = AccErrorY;
  arr[2] = GyroErrorX;
  arr[3] = GyroErrorY;
  arr[4] = GyroErrorZ;

    // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
  return arr;
}
