//byte index
//Y = PaintBot On
//N = PaintBot Off
//S = Start Signal
//R = PaintBot Ready to receive array
//H = Hall Effect Triggered
//C = Capture Image (PI)
int a = 0; 
int active = 0;
int i = 0;
int cycle = 0;
int EXPECTED_PIXEL_COUNT; //Number of pixels the Arduino will receive
int CAPTURES_PER_STRIP; //Number of images to Capture in one cycle
int data[500]; //Buffer for pixel data
char byteReceived = '0'; //Byte received from PI

void setup() {
  
Serial.begin(9600);              //Starting serial communication

CAPTURES_PER_STRIP = 5; 
EXPECTED_PIXEL_COUNT = 140;

}

  

void loop() {
  
  
     activeStatePB();
     memset(data,0,sizeof(data));
     
     while (active == 1){
       Serial.print('S');
       
       byteReceived = Serial.read();
       
       while (byteReceived != 'S'){
       
          byteReceived = Serial.read(); 
          delay(100);
         
       }
       
       
       //PaintBot Control Code Goes Here
       
       
       //commandPItoCapture() will have to be integrated with the movement of tower
       commandPItoCapture();
       
       
       delay(2000);
       
       //Receives data for next cycle
       getNextStripData();
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
         active = 0;
         i = 0;
         cycle = 0;
         return;
    }
}

}

void commandPItoCapture() {
  int numCaptures = 0;
  
  while (numCaptures < CAPTURES_PER_STRIP){
    
    if (Serial.available() > 0) {
    byteReceived = Serial.read();
    if (byteReceived == 'N'){
      active = 0;
      i = 0;
      cycle = 0;
      return;
      }
    }
    delay(3000);
    Serial.print("C");
    numCaptures++;
  }
}


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
            active = 0;
            i = 0;
            cycle = 0;
          return;
          default :
            data[i] = byteReceived + 0 ;
            bytes_read++;
            i++;                  
     }
     }      
  }
  
  }
}





