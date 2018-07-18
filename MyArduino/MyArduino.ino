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
int data[100]; //Buffer for pixel data
char byteReceived = '0'; //Byte received from PI


//PaintBot States
enum states {BASE_MOVING, BASE_ALIGN, TOWER_MOVING, NONE};
states state = NONE;


void setup() {
  
Serial.begin(9600); //Starting serial communication


CAPTURES_PER_STRIP = 5; // Number of images that will be captured by PI (only needed if not using hall effect) 
EXPECTED_PIXEL_COUNT = 25; // Number of bytes expected to receive from the PI

triggerServo.attach(13);
triggerServo.write(90);

attachInterrupt(0, magnet_detect, CHANGE);//Initialize the intterrupt pin (Arduino digital pin 2)
pinMode (7, INPUT);

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
       state = TOWER_MOVING;       
       //commandPItoCapture();// will have to be integrated with the movement of tower
       // Receives data for next cycle
       
       while (Serial.available() == 0){
         delay(100);
       }
       byteReceived = Serial.read();
       if (byteReceived == 'D'){
       getNextStripData();
       } else if (byteReceived == 'N'){
         reset();
       }
       cycle++;         
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
  
  delay(2000);
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


void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
 {
   if (Serial.available() == 0){
     if (Serial.read() == 'N'){
         reset();
         
     }
   }
   if(state = TOWER_MOVING){  
   
   if (data[j] == 1){
     triggerServo.write(0);
   } else {
     triggerServo.write(180); 
   }
   j++;
   if (j%5 == 0)
     Serial.print("C"); //Stands for Capture
     else{
     Serial.print("T"); //Stands for Triggered
     }
   if (j == 25)
     j = 0;       
 
 }
} 
 
//Reset some globals
void reset() {
   active = 0;
   i = 0;
   j = 0;
   cycle = 0;
}






