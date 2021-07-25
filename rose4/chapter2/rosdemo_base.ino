#include <Arduino.h>
#include <ICS.h>

#define KRS_ORG 7500
#define KRS_FREE 0
#define SERVO_NUM 3

IcsController ICS(Serial1);
IcsServo servo[SERVO_NUM];

uint16_t servo_pos[SERVO_NUM];

void initializeServo(){
  ICS.begin();
  for(int i = 0; i < SERVO_NUM; i++){
    servo[i].attach(ICS, i);
    servo[i].setPosition(KRS_FREE);
    delay(1);
  }
}

void setup() {
  Serial.begin(9600);
  
  initializeServo();
}

void getPositionAll(){
  for(int i = 0; i < SERVO_NUM; i++){
    servo_pos[i] = servo[i].getPosition();
    delay(1);
  }
}

void loop() {
  getPositionAll();
  
  Serial.print("POS:"); Serial.print("\t");
  Serial.print(servo_pos[0]); Serial.print("\t");
  Serial.print(servo_pos[1]); Serial.print("\t");  
  Serial.print(servo_pos[2]); Serial.print("\n");  
 
  delay(100);
}
