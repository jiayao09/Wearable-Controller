#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <Servo.h>

#define CE_pin 8
#define CSN_pin 9

RF24 radio(CE_pin, CSN_pin);

Servo base, fArm, rArm, claw; 
const byte address[6] = "00001";
char text;
int value;
boolean i = 0;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  base.attach(3);  // b
  
  delay(200);
  rArm.attach(5);  // r
  
  delay(200);
  fArm.attach(6);   // f
  
  delay(200);
  claw.attach(10);   // c

  delay(200);
}

void loop() {
  if(radio.available()){
   if (i == 0){
     radio.read(&text, sizeof(text));
     i = 1;
   }else{
     radio.read(&value, sizeof(value));
     i = 0;
//     Serial.println(text);
//     Serial.println(value); 
      
      switch(text){
        case 'u':
          base.write(value);
          Serial.print("Set base servo value: ");
          Serial.println(value);
          break;
        case 'd':
          rArm.write(value);
          Serial.print("Set base servo value: ");
          Serial.println(value);
          break;
        case 'l':
          fArm.write(value);
          Serial.print("Set base servo value: ");
          Serial.println(value);
          break;
        case 'r':
          claw.write(value);
          Serial.print("Set base servo value: ");
          Serial.println(value);
          break;                      
      }      
   }    


//     switch(text){
//      case 'u':
//        // base.write(data);
//        Serial.print("Set base servo value: ");
//        Serial.println(data);
//        break;
//      case 'd':
//        rArm.write(data);
//        Serial.print("Set base servo value: ");
//        Serial.println(data);
//        break;
//      case 'l':
//        fArm.write(data);
//        Serial.print("Set base servo value: ");
//        Serial.println(data);
//        break;
//      case 'r':
//        claw.write(data);
//        Serial.print("Set base servo value: ");
//        Serial.println(data);
//        break;                      
//    }      
//      delay(1000);
//    base.write(0);
//    rArm.write(0);
//    fArm.write(0);
//    claw.write(0);
 }
//  delay(100);

}
