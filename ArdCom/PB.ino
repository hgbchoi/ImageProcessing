#include <Servo.h>

///////////
//Tower Motor Variables
Servo TowerMotor;
const int TowerMotorPin = 13;
const int inDirectionUpward = 0;
const int inDirectionDownward = 180;
const int TowerHeightTime = 3000;
///////////

Servo sprayGunActuator;
const int sprayGunActuatorPin = 30;
const int sprayGunON = 1;
const int sprayGunOFF = 0;

///////////
//Drive Motor Variables
Servo DriveMotorBL;
Servo DriveMotorBR;
Servo DriveMotorFL;
Servo DriveMotorFR;

const int DriveMotorPinBL = 12;
const int DriveMotorPinBR = 8;
const int DriveMotorPinFL = 10;
const int DriveMotorPinFR = 6;

const int PivotMotorPinBL = 13;
const int PivotMotorPinBR = 9;
const int PivotMotorPinFL = 11;
const int PivotMotorPinFR = 7;
///////////

///////////
//Pivot Motor Variables
Servo PivotMotorBL; 
Servo PivotMotorBR;
Servo PivotMotorFL;
Servo PivotMotorFR;

const int PivotMotorBLPar   = 0;
const int PivotMotorBLPiv   = 36;
const int PivotMotorBLPerp  = 80;

const int PivotMotorBRPar   = 78;
const int PivotMotorBRPiv   = 40;
const int PivotMotorBRPerp  = 4;

const int PivotMotorFLPerp  = 8;
const int PivotMotorFLPiv   = 40;
const int PivotMotorFLPar   = 80;

const int PivotMotorFRPar  = 4;
const int PivotMotorFRPiv  = 38;
const int PivotMotorFRPerp = 82;

const int stopMotorTower = 91;
const int stopMotor = 95;

const int turnTime180   = 4000;
const int stepTurnTime  = 200;

const int CWturn  = 180;
const int CCWturn = 0;

const int parallelDriveDistance = 2000;
const int perpDriveDistance     = 2000;

const int stepAlignPerp = 250;

const int inDirectionForward = 0;
const int inDirectionBackward = 180;

const int StripTime = 1000;
///////////
///////////
//US variables
const int trigPinFL           = 4;
const int echoPinFL           = 5;
const int trigPinFR           = 3;
const int echoPinFR           = 2;
const int definedWallDistance = 7; //cm 
const int wallDistanceError   = 1; // cm

int averageCounter 	= 0;
int errorTolerance 	= 2; // 5 seems perhaps too precise.
int firstSweep		= 0;
int Hall 			= 0;
int arbitraryCount 	= 0;

const int sizeArray = 1000;

long duration;
int distance;
int d1;
int d2;
bool parallelAlignment;
bool distanceAlignment;  

int nextColumnArray[sizeArray]; 

///////////
//Orientation State Variable
typedef enum {PIVOT, PARALLEL, PERP, UNSET} states; 
states baseState = UNSET;

void setup() 
{
	Serial.begin(9600); 

	DriveMotorBL.attach(DriveMotorPinBL);
	DriveMotorBR.attach(DriveMotorPinBR);
	DriveMotorFL.attach(DriveMotorPinFL);
	DriveMotorFR.attach(DriveMotorPinFR);

	PivotMotorBL.attach(PivotMotorPinBL);
	PivotMotorBR.attach(PivotMotorPinBR);
	PivotMotorFL.attach(PivotMotorPinFL);
	PivotMotorFR.attach(PivotMotorPinFR);

	TowerMotor.attach(TowerMotorPin);
	sprayGunActuator.attach(sprayGunActuatorPin);

	pinMode(trigPinFL, OUTPUT);
	pinMode(echoPinFL, INPUT); 

	pinMode(trigPinFR, OUTPUT); // Sets the trigPin2 as an Output
	pinMode(echoPinFR, INPUT); 
}

//*********************************
// LOOP
//*********************************

void loop()
{
  
  int pollActive();
  
  int active = Serial.Read();
  while(active == 1)
  {
	parallelAlignment = false;
	distanceAlignment = false;  

	StopTower();
	ParallelAlign(); 
	DistanceAlign();
	ObjectDetection();
	NextStrip();

  }
  
    //ParallelDrive(1000);
  
}

//*********************************
//*********************************

////////////////////
// Functions
int pollActive()
{
	Serial.begin(9600);
	while (Serial.available()>0) serIn=Serial.read();

	if (Serial.available()>0) {
	  serIn=Serial.read()
	  if (SerIn=='A') { 
		return 1;
	  }
	  while (Serial.available()>0) serIn=Serial.read();
	}  
}

/*
int pollActiveBT()
{
	Serial.begin(9600);
	while (Serial.available()>0) serIn=Serial.read();

	if (Serial.available()>0) {
	  serIn=Serial.read()
	  if (SerIn=='A') { //CHANGE TO WHAT WE EXPECT FROM THE BT 
		return 1;
	  }
	  while (Serial.available()>0) serIn=Serial.read();
	}  
}
*/

void paintColumn()
{
		startColorDetection();				// send 'G' to Pi
		int confirm = rxPiConfirmation(); 	// rx 'C' from Pi
		if(confirm == 1)
		{
			if(firstSweep == 0)
			{
				//TowerBegin(sprayGunOFF); // 0 means the spraygun is OFF for the first sweep
				TowerMotor.write(inDirectionUpward);
				sprayGunActuator.write(sprayGunOFF);
				
				Hall = PollHall(); // not realistic
				
				if(Hall == 1)
				{
					stopColorDetection();	//send to Pi - send a lot of 'S's
					nextColumnArray[] = obtainPiArray(); // need to do the serial processing
				}
				Hall = 0;
				firstSweep = 1;
			}
			else if(firstSweep == 1)
			{
				TowerMotor.write(inDirectionUpward);
				//sprayGunActuator.write(sprayGunOFF);
				while(Hall == 0)
				{
					for(i = 0; i < size(currentColumnArray); i++)
					{
						if(currentColumnArray[i] == 1)
						{
							sprayGunActuator.write(sprayGunOFF); //
						}
						delayFactor();
					}
					CheckHall();
				}
				if(Hall == 1)
				{
					stopColorDetection();
					nextColumnArray[] = obtainPiArray(); // need to do the serial processing
				}
				Hall = 0;
				firstSweep = 1;	
			}	
		}
}
int* obtainPiArray()
{
	// we do a serial read from the pi and form an array
	int piArray[sizeArray];
	
	int bytes_read = 0;
	
	while (bytes_read < sizeArray)
	{
		if (Serial.available() > 0)
		{
		   piArray[bytes_read] = Serial.read();
		   bytes_read ++;
		}
	}
	
	return &piArray;
	
}
void CheckHall()
{
	arbitraryCount = arbitraryCount + 1;
	if(arbitraryCount == 100)
	{
		Hall = 1;
		arbitraryCount = 0;
	}
	
}
void PollHall()
{
	delay(TowerHeightTime);
	Hall = 1;
	return; // THIS IS TO GET THINGS MOVING ALONG - WE WILL NEED THE HALL..
}
/*
void TowerBegin(int sprayGunState)
{
	if(sprayGunState == 1)
	{
		Serial.print("TOWER OF POWER1");
		TowerMotor.write(inDirectionUpward);
		sprayGunActuator.write(sprayGunON);
		// Now, we need to read the array for when to set the sprayGunActuator.write(sprayGunOFF)
		
	}
	else if(sprayGunState == 0)
	{
		Serial.print("TOWER OF POWER2");
		TowerMotor.write(inDirectionUpward);
		sprayGunActuator.write(sprayGunOFF);	
	}
}
*/
void startColorDetection()
{
	// this sends a serial signal to the polling Pi
	char goSignal = 'G';
	Serial.println(goSignal);
	
}
int rxPiConfirmation()
{
	Serial.begin(9600);
	while (Serial.available()>0) serIn=Serial.read();

		if (Serial.available()>0) {
		  serIn=Serial.read()
		  if (SerIn=='C') { // received confirmation from Pi 
			return 1;
		  }
		  while (Serial.available()>0) serIn=Serial.read();
		}
}
void stopColorDetection()
{
	char stopSignal = 'S';
	for(int j = 0; j < 100;j++)
	{
		Serial.println(stopSignal); // this is sloppy, not sure if 1 send is enough
	}
	
}
	
void NextStrip()
{
  Serial.print("MOOF TO NEXT STRIP");
  ParallelDrive(StripTime);
}
void ObjectDetection()
{
  Serial.print("TOWER OF POWER");
  TowerMotor.write(inDirectionUpward);
  delay(TowerHeightTime);
  StopTower();
  TowerMotor.write(inDirectionDownward);
  delay(TowerHeightTime);
  StopTower();
}
void StopTower()
{
  TowerMotor.write(stopMotorTower);
}
void DistanceAlign()
{
  while(distanceAlignment == false)
  {
    d1 = ReadingUS(trigPinFL, echoPinFL);
    d2 = ReadingUS(trigPinFR, echoPinFR);

    if(abs(d1-d2) > errorTolerance) {
      baseState = UNSET;
      parallelAlignment = false;
      ParallelAlign();
    }

    if( (d1 >= definedWallDistance - wallDistanceError and d1 <= definedWallDistance + wallDistanceError) and
        d2 >= definedWallDistance - wallDistanceError and d2 <= definedWallDistance + wallDistanceError)
    {
      Serial.print("TIS aligned sirs");
      
      distanceAlignment = true;
    }
    else{
      BasePerp();
      
      if((d1 < definedWallDistance - wallDistanceError) and (d2 < definedWallDistance - wallDistanceError))
      {
        // too close mon', back up
        // double check if statement
        Serial.print("BACKWAAAAARD");  
        
        PerpDrive(stepAlignPerp, inDirectionBackward);
        
      }
      else
      {
        Serial.print("FORWARRRRD");  
        
        PerpDrive(stepAlignPerp, inDirectionForward);
      }
      
      delay(100);
    }
  
  }
  
}
void ParallelAlign()
{
    StopDrive();
    delay(500);
    while(parallelAlignment == false)
    {
      d1 = ReadingUS(trigPinFL, echoPinFL);
      d2 = ReadingUS(trigPinFR, echoPinFR);
     
      Serial.print("d1 average value: ");
      Serial.println(d1);
    
      Serial.print("d2 average value: ");
      Serial.println(d2);
  
      delay(3000); //debug
  
      if(abs(d1-d2) <= errorTolerance)
      {
          parallelAlignment = true;
          Serial.print("All lined up ");  
      }
      else
      {
          BasePivot();
          if((d1-d2) < 0)
          { 
            PivotCCW(stepTurnTime);
            Serial.print("CCW ");  
          }
          else
          {
            Serial.print("CW ");
            PivotCW(stepTurnTime);
          }
      }
    }
}


void BaseParallel()
{
  if(baseState != PARALLEL) {
    PivotMotorBL.write(PivotMotorBLPar);
    PivotMotorBR.write(PivotMotorBRPar);
    PivotMotorFL.write(PivotMotorFLPar);
    PivotMotorFR.write(PivotMotorFRPar);
    baseState = PARALLEL;
    delay(500);
  }
}
void BasePivot()
{
  if(baseState != PIVOT) {
    PivotMotorBL.write(PivotMotorBLPiv);
    PivotMotorBR.write(PivotMotorBRPiv);
    PivotMotorFL.write(PivotMotorFLPiv);
    PivotMotorFR.write(PivotMotorFRPiv);
    baseState = PIVOT;
    delay(500);
  }
}
void BasePerp()
{
  if(baseState != PERP) {
    PivotMotorBL.write(PivotMotorBLPerp);
    PivotMotorBR.write(PivotMotorBRPerp);
    PivotMotorFL.write(PivotMotorFLPerp);
    PivotMotorFR.write(PivotMotorFRPerp);
    delay(500);
  }
}
void ParallelDrive(int driveTime)
{
  BaseParallel();
  delay(500);
  DriveMotorBL.write(0);
  DriveMotorBR.write(0);
  DriveMotorFL.write(180);
  DriveMotorFR.write(180);
  delay(driveTime); //was 2000
  StopDrive();
  //return;
}
void PerpDrive(int driveTime, int inDirection)
{
  BasePerp();
  delay(500);
  
  if(inDirection == inDirectionForward)
  {
    DriveMotorBL.write(180);
    DriveMotorBR.write(0);
    DriveMotorFL.write(180);
    DriveMotorFR.write(0);
    delay(driveTime); //was 2000
    StopDrive();
  }
  else
  {
    DriveMotorBL.write(0);
    DriveMotorBR.write(180);
    DriveMotorFL.write(0);
    DriveMotorFR.write(180);
    delay(driveTime); //was 2000
    StopDrive();
  }
}
void PivotCCW(int driveTime)
{
  BasePivot();
  delay(500);
  DriveMotorBL.write(CCWturn);
  DriveMotorBR.write(CCWturn);
  DriveMotorFL.write(CCWturn);
  DriveMotorFR.write(CCWturn);
  delay(driveTime);
  StopDrive();
}
void PivotCW(int driveTime)
{
  BasePivot();
  delay(500);
  DriveMotorBL.write(CWturn);
  DriveMotorBR.write(CWturn);
  DriveMotorFL.write(CWturn);
  DriveMotorFR.write(CWturn);
  delay(driveTime);
  StopDrive();
}
void StopDrive()
{
  DriveMotorBL.write(stopMotor);
  DriveMotorBR.write(stopMotor);
  DriveMotorFL.write(stopMotor);
  DriveMotorFR.write(stopMotor); 
}

int ReadingUS(int trigPin, int echoPin) 
{
  const int numReadings   = 20; 
  int averageSum          = 0;
  int averageCounter      = 1;
  int average             = 0;
  
  while(averageCounter < numReadings)
  {
    // Clears the trigPinFL
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin1, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance= duration*0.034/2;
    // Prints the distance on the Serial Monitor
    
    if(trigPin == trigPinFL)
    {
      Serial.print("Distance trigPinFL: "); 
      Serial.println(distance);  
      if(distance != 0)// and distance > 3)
      {
        averageCounter = averageCounter + 1;
        averageSum = averageSum + distance;
      }
    }
    if(trigPin == trigPinFR) //debug to have both
    {
      Serial.print("Distance trigPinFR: "); 
      Serial.println(distance);// and distance > 3);  
      if(distance != 0)
      {
        averageCounter = averageCounter + 1;
        averageSum = averageSum + distance;
      }
    }
  }
  average = averageSum/averageCounter;
  return average;
}
