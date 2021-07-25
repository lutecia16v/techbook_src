#include <Arduino.h>
#include <ICS.h>
#include <ros.h>
#include <sensor_msgs/JointState.h>

#define KRS_FREE 0
#define SERVO_NUM 3

ros::NodeHandle  nh;
sensor_msgs::JointState jointstate_msg;
char frame_id[] = "/base_link";
char *joint_name[] = {"J1", "J2", "J3"};
float pos[SERVO_NUM] = {0.0, 0.0, 0.0};
float vel[SERVO_NUM] = {0.0, 0.0, 0.0};
float eff[SERVO_NUM] = {0.0, 0.0, 0.0};

ros::Publisher jsp("joint_state", &jointstate_msg);

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
  initializeServo();
  
  nh.initNode();
  nh.advertise(jsp);
  
  jointstate_msg.header.frame_id = frame_id;
  jointstate_msg.name_length = SERVO_NUM;
  jointstate_msg.position_length = SERVO_NUM;
  jointstate_msg.velocity_length = SERVO_NUM;
  jointstate_msg.effort_length = SERVO_NUM;
  jointstate_msg.name = joint_name;
}

void getPositionAll(){
  for(int i = 0; i < SERVO_NUM; i++){
    servo_pos[i] = servo[i].getPosition();
    delay(1);
  }
}

void loop() {
  getPositionAll();

  for(int i = 0; i < SERVO_NUM; i++){
    pos[i] = servo_pos[i];
  }
  jointstate_msg.position = pos;
  jointstate_msg.velocity = vel;
  jointstate_msg.effort = eff;
    
  jsp.publish( &jointstate_msg );
  nh.spinOnce();
  delay(100);
}
