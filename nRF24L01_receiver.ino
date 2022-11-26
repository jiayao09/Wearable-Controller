#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <Servo.h>

#define CE_pin 9
#define CSN_pin 8

int checkStatus(int servoData, int maxInput, int minInput);
void armIniPos();
void servoCmd(char serialName, int servoData, int Delay);

RF24 radio(CE_pin, CSN_pin);
const int baseMin = 0;
const int baseMax = 180;
const int rArmMin = 45;
const int rArmMax = 130;
const int fArmMin = 60;
const int fArmMax = 130;
const int clawMin = 5;
const int clawMax = 50;
int DSD = 15;
Servo base, fArm, rArm, claw; 
const byte address[6] = "00001";
int value[2] = {0};
int target = 0;
boolean i = 0;
int basePos = 120;
int rArmPos = 70;
int fArmPos = 75;
int clawPos = 50;

void setup() {
  // Setup nRF24L01
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.startListening();
  base.attach(3);  // b
  
  //Setup Servo and move to the initial position
  delay(200);
  rArm.attach(6);  // r
  delay(200);
  fArm.attach(5);   // f  
  delay(200);
  claw.attach(10);   // c  
  base.write(basePos);
  delay(10);
  fArm.write(fArmPos);
  delay(10);
  rArm.write(rArmPos);
  delay(10);
  claw.write(clawPos);
  delay(10);
  delay(200);
}

void loop() {
  if(radio.available()){
    Servo servo2go;
    int fromPos = 0;
    int toPos = 0;
    int servoData = 0;
    radio.read(&value, sizeof(value)); 
    Serial.print(char(value[0]));
    Serial.println(value[1]);
    switch(char(value[0])){
        case 'u':
          servo2go = fArm;
          fromPos = fArm.read();
          servoData = fromPos + value[1];
          toPos = checkStatus(servoData, fArmMax, fArmMin);
          break;
        case 'd':
          servo2go = fArm;
          fromPos = fArm.read();
          servoData = fromPos - value[1];
          toPos = checkStatus(servoData, fArmMax, fArmMin);
          break;
        case 'l':
          servo2go = base;
          fromPos = base.read();
          servoData = fromPos + value[1];
          toPos = checkStatus(servoData, baseMax, baseMin);
          break;
        case 'r':
          servo2go = base;
          fromPos = base.read();
          servoData = fromPos - value[1];
          toPos = checkStatus(servoData, baseMax, baseMin);         
          break;
        case 'c':
          servo2go = claw;
          fromPos = claw.read();
          toPos = clawMin;  
          break; 
        case 'o':
          servo2go = claw;
          fromPos = claw.read();
          toPos = clawMax;
          break;
        case 'b':
          if (claw.read() == clawMax){
              armIniPos();
              Serial.println("reset");
              servo2go = rArm;
              fromPos =rArm.read();
              toPos =rArm.read();
              delay(2000);
            }else{
              servo2go = rArm;
              fromPos = rArm.read();
              servoData = fromPos + value[1];
              toPos = checkStatus(servoData, rArmMax, rArmMin);  
            }
          break;  
        case 'f':
          servo2go = rArm;
          fromPos = rArm.read();
          servoData = fromPos - value[1];
          toPos = checkStatus(servoData, rArmMax, rArmMin);
          break;                                                               
      }
    if (fromPos <= toPos){
      for(int i = fromPos; i<= toPos; i++){
        servo2go.write(i);
        delay(DSD);
      } 
    } else {
      for(int i = fromPos; i >= toPos; i--){
        servo2go.write(i);
        delay(DSD);
      }
    }      
   } 
}


int checkStatus(int servoData, int maxInput, int minInput){
  int servo = 0;
  if (servoData >= minInput && servoData <= maxInput) {
    servo = servoData;
  } else if (servoData < minInput) {
    servo = minInput;
    Serial.println(" "); Serial.print("Warning: Servo Value is out of Limit!");
    Serial.print("Choose range between "); Serial.print(minInput);Serial.print(" and ");Serial.println(maxInput);
  }
  else {
    servo = maxInput;
    Serial.print("Warning: Servo Value is out of Limit! ");
    Serial.print("Choose range between "); Serial.print(minInput);Serial.print(" and ");Serial.println(maxInput);
  }
  Serial.print("Target angle = "); 
  Serial.println(servo); 
  return(servo);
}

void armIniPos(){
  servoCmd('b', basePos, DSD);
  servoCmd('r', fArmPos, DSD);
  servoCmd('f', rArmPos, DSD);
  servoCmd('c', clawPos, DSD);
}

void servoCmd(char serialName, int servoData, int Delay){
  Servo servo2go;
  int fromPos;
  int toPos;
  switch(serialName){
    case 'b':
      servo2go = base;
      fromPos = base.read();
      toPos = checkStatus(servoData, baseMax, baseMin);
      break;
    case 'r':
      servo2go = rArm;
      fromPos = rArm.read();
      toPos = checkStatus(servoData, rArmMax, rArmMin);
      break;
    case 'f':
      servo2go = fArm;
      fromPos = fArm.read();
      toPos = checkStatus(servoData, fArmMax, fArmMin);
      break;
    case 'c':
      servo2go = claw;
      fromPos = claw.read();
      toPos = checkStatus(servoData, clawMax, clawMin);
      Delay = 1;
      break;
  }
  
  if (fromPos <= toPos){
    for(int i = fromPos; i<= toPos; i++){
      servo2go.write(i);
      delay(Delay);
    } 
  }  else {
      for(int i = fromPos; i >= toPos; i--){
      servo2go.write(i);
      delay(Delay);
      }
  }
}
