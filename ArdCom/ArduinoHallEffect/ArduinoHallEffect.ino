/*
Arduino Hall Effect Sensor Project
by Arvind Sanjeev
Please check out  http://diyhacking.com for the tutorial of this project.
DIY Hacking
*/
#include <Servo.h>  

 Servo triggerServo;
 volatile int change;
 int data[10] = {0, 0, 0, 0, 1, 1, 0, 1,0 ,1};
 int j = 0;
 
 void setup()
 {
   
   Serial.begin(115200);
   triggerServo.attach(13);
   triggerServo.write(180);
  // attachInterrupt(0, magnet_detect, CHANGE);//Initialize the intterrupt pin (Arduino digital pin 2)

 }
 void loop()//Measure RPM
 {
   triggerServo.write(0);

  delay(2000);
  
  triggerServo.write(180);
  
   
 }
 

 
