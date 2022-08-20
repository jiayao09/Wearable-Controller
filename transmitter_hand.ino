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

int x = 0;
int y = 0;
int z = 0;
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
  
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
      delay(10);
  }

  Serial.println("Found a MPU-6050 sensor");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  delay(100);
  
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

}
void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  display.clearDisplay();
  display.setCursor(0, 0);

  display.println("Accelerometer - m/s^2");
  display.print(a.acceleration.x, 1);
  display.print(", ");
  display.print(a.acceleration.y, 1);
  display.print(", ");
  display.print(a.acceleration.z, 1);
  display.println("");

  display.println("Gyroscope - rps");
  display.print(g.gyro.x, 1);
  display.print(", ");
  display.print(g.gyro.y, 1);
  display.print(", ");
  display.print(g.gyro.z, 1);
  display.println("");

  display.display();
  delay(100);
  
  x = a.acceleration.x;
  y = a.acceleration.y;
  z = a.acceleration.z;

if (x < 10 && x > 0 && y < 4 && y > -4){
  value = map(x, 0, 10, 0, 180);
  control = 'u';
  sendDatas = control;
  radio.write(&sendDatas, sizeof(sendDatas));
  radio.write(&value, sizeof(value));
  Serial.print("up");
  Serial.println(value);
  delay(1000);
}else if (x > -10 && x < 0 && y < 4 && y > -4){
  value = map(x, -10, 0, 180, 0);
  control = 'd';
  
  sendDatas = control;
  radio.write(&sendDatas, sizeof(sendDatas));
  radio.write(&value, sizeof(value));
  Serial.println("down");
  Serial.print(value);
  delay(1000);
}


if (y < 10 && y > 0 && x < 4 && x > -4){
  value = map(y, 0, 10, 0, 180);
  control = 'r';
  
  sendDatas = control;
  radio.write(&sendDatas, sizeof(sendDatas));
  radio.write(&value, sizeof(value));
  Serial.println("Right");
  Serial.print(value);
  delay(1000);
}else if (y > -10 && y < 0 && x < 4 && x > -4){
  value = map(y, -10, 0, 180, 0);
  control = 'l';
  
  sendDatas = control;
  radio.write(&sendDatas, sizeof(sendDatas));
  radio.write(&value, sizeof(value));
  Serial.println("left");
  Serial.print(value);
  delay(1000);
}
}
