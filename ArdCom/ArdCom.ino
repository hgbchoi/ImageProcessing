//byte index
//Y = PaintBot On
//N = PaintBot Off
//R = PaintBot Ready to receive array
//H = Hall Effect Triggered
//D = Data Ready
int a = 0; 
int active = 0;
int cycle = 0;
int data[500];
int i = 0;

char byteReceived = '0';

void setup() {
Serial.begin(9600);              //Starting serial communication
}

  

void loop() {
  onOffPB(); 
  
   while (active == 1){
     i = 0;
     getDatafromPi();       
     cycle++; 
     for (int i = 0; i < 300; i++){
     Serial.print(data[i]);
     
     }
     } 
}
  

void onOffPB (){
  
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
         break;
    }
}

}

void getDatafromPi () {

  
  Serial.print("R");
  delay(1000);
  int j = 0;
  while (Serial.available() > 0){
   byteReceived = Serial.read();
   
   
   switch (byteReceived){
     case 'Y' :
       active = 1;
       break;
     case 'N':
        active = 0;
         i = 0;
        cycle = 0;
        break;
     default :
        data[i] = byteReceived + 0 ;
        i++;              
   }
  }
}





