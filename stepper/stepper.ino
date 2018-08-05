

    //really basic code for the ‘MakeBlock 2H Microstep Driver’

    //step controller
    int dirPin = 5;
    int pulPin = 8;
    int enblPin = 2;

    void setup(){
  
      pinMode(dirPin, OUTPUT);
      pinMode(pulPin, OUTPUT);
      pinMode(enblPin, OUTPUT);
      digitalWrite(enblPin, HIGH);

    }

    void loop(){


	for (int i = 0; i < 10; i ++){
      slide('L', 30000, 25); //go Left for 500 steps at speed 500, (the lower the number is the faster the motor will go)
	}    
      delay(500);
  	
     slide('R', 300000, 25); //go Right for 500 steps at speed 500
  	
  	
      delay(500);
      
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

