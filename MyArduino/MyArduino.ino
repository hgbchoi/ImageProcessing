//byte index
//Y = PaintBot On
//N = PaintBot Off
//S = Start Signal
//R = PaintBot Ready to receive array
//C = Capture Image (PI)

#include <Servo.h>


//Trigger Servo
Servo triggerServo;

//Globals
int hallState = 0;
int cycle = 0;
int active = 0;
int i = 0;
int j = 0;
int EXPECTED_PIXEL_COUNT; //Number of pixels the Arduino will receive
int CAPTURES_PER_STRIP; //Number of images to Capture in one cycle
long TOWER_HEIGHT;
int data[150]; //Buffer for pixel data
char byteReceived = '0'; //Byte received from PI

int dirPin = 5;
int pulPin = 8;
int enblPin = 2;
int trigPin = 11;

//PaintBot States
enum states {BASE_MOVING, BASE_ALIGN, TOWER_MOVING, NONE};
states state = NONE;


void setup() {
  
Serial.begin(9600); //Starting serial communication

CAPTURES_PER_STRIP = 6; // Number of images that will be captured by PI (only needed if not using hall effect) 
EXPECTED_PIXEL_COUNT = 15*CAPTURES_PER_STRIP; //Number of bytes expected to receive from the PI
TOWER_HEIGHT = 290000;
pinMode(dirPin, OUTPUT);
pinMode(pulPin, OUTPUT);
pinMode(enblPin, OUTPUT);
digitalWrite(enblPin, HIGH);
triggerServo.attach(trigPin);
triggerServo.write(90);

}

  

void loop() {
  
    i = 0;
    j = 0;
    
    activeStatePB(); //Check for on/off bytes 
    memset(data,0,sizeof(data)); //set data array to be all 0
     
    while (active == 1){
       Serial.print('S'); //Send start byte to PI
       
       //Wait until PI sends start byte for confirmation
       while (byteReceived != 'S'){
       
         byteReceived = Serial.read(); 
         delay(100);
         
       }
       
       
       //PaintBot Control Code Goes Here
       
       
       //Set state to tower moving when base is aligned and ready for tower movement
       for (int i = 0; i < EXPECTED_PIXEL_COUNT; i ++){
         if (data[i] == 1){
           triggerServo.write(90);
         } else {
           triggerServo.write(150);
         }          
         slide('L', TOWER_HEIGHT/EXPECTED_PIXEL_COUNT, 25); //go Left for 500 steps at speed 500, (the lower the number is the faster the motor will go)
         if (i % 15 == 0 ) {
           Serial.print('C');
         }

       }
       
       delay(500);
      
        for (int i = EXPECTED_PIXEL_COUNT - 1; i != -1; i--){
         if (data[i] == 1){
           triggerServo.write(90);
         } else {
           triggerServo.write(150);
         }          
          slide('R', TOWER_HEIGHT/EXPECTED_PIXEL_COUNT, 25); //go Left for 500 steps at speed 500, (the lower the number is the faster the motor will go)

       }       
      
       delay(500);
      
      Serial.print('D');
      while (byteReceived != 'D'){
       
         byteReceived = Serial.read(); 
         delay(100);
         
       }
         getNextStripData();
       
       
      }
       cycle++;         
   
    
}

void slide(int dir, long steps, int sspeed){
  
      if (sspeed < 50) sspeed = 10; //keeps the speed above 10
  
      if (dir == 'L') digitalWrite(dirPin, HIGH);
      else if (dir == 'R') digitalWrite(dirPin, LOW);
  
      while (steps > 0){
        digitalWrite(pulPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(pulPin, LOW);
        delayMicroseconds(10);
        delayMicroseconds(sspeed);
        steps = steps - 1;
      }
}


void activeStatePB(){
  
  if (Serial.available() > 0){
    byteReceived = Serial.read();
    switch (byteReceived) {
       case 'Y' :
         active = 1;
         break; 
       case 'N':
         reset();
         return;
    }
  }
}

//Only needed when not using Hall Effect
void commandPItoCapture() {
  int numCaptures = 0;
  
  while (numCaptures < CAPTURES_PER_STRIP){
    delay(2000);
    Serial.print("C");    

    if (Serial.available() > 0) {
    byteReceived = Serial.read();
    if (byteReceived == 'N'){
      reset();
      return;
      
      }
    }
    numCaptures++;
  }
}

//Run at the end of the tower movement (reaching top or bottom) ask PI for the data to be used for the next strip
void getNextStripData() {
  
  i = 0;
  int bytes_read = 0;
  if (active == 1){
    Serial.print("R");
    while (bytes_read < EXPECTED_PIXEL_COUNT){
       while (Serial.available() > 0){
         byteReceived = Serial.read();
         switch (byteReceived){        
           case 'N':
           reset();
           default :
              data[i] = byteReceived + 0 ;
              bytes_read++;
              i++;                  
         }
       }      
     }
  }
}

 
//Reset some globals
void reset() {
   active = 0;
   i = 0;
   j = 0;
   cycle = 0;
}






