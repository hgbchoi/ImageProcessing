#include <Servo.h>
#include "pb_ctrl.h"


void setup() {
	Serial.begin(9600);
	
	for (int k = 0; k < 4; k++) {
		drive_motor[k].attach(drive_motor_pin[k]);
		pivot_motor[k].attach(pivot_motor_pin[k]);
	}

  //Base pin setup
	for (int k = 0; k < 2; k++) {
		pinMode(trig_pin[k], OUTPUT);
		pinMode(echo_pin[k], INPUT);
	}

  //Tower pin setup
  pinMode(tower_dir_pin, OUTPUT);
  pinMode(tower_pul_pin, OUTPUT);
  pinMode(tower_enbl_pin, OUTPUT);
  digitalWrite(tower_enbl_pin, HIGH);
  paint_trigger_servo.attach(paint_trig_pin);
  //Set paint trigger pin to default (off) position
  paint_trigger_servo.write(PAINT_TRIG_OFF);

  //Image capture data setup
  byteReceived = '0';
  cycle = 0;
  memset(data,0,sizeof(data)); //set data array to be all 0

  //State setup
	drive_state = D_UNDEFINED;
	wheel_state = UNDEFINED;
	set_drive_state(STOP);
	
	//Set pivot motors to default position
	pivot_wheels(DRIVE_LR);
}

unsigned long alt_timestamp = 0;

void loop() {
//  activeStatePB();

//  while (active == 1){
   
  
  	cycle_timestamp = micros();
  	
  	// Can check for commands incoming from serial every cycle prior to executing actions for current state.
  	
  	read_uss();
  	
  	if (check_time(wait_timestamp, wait_duration)) {
  		wait_duration = 0;
  	}
  	//wait_duration = 1;	Prevent the state machine from running.
  	if (wait_duration == 0) {
  		// Main state machine for performing actions.
  		switch (state) {
  			case IDLE:
  				// Start orienting to wall by default for now.
  				to_align_state();
                                Serial.println("IDLE STATE");
  				break;
  			case ALIGN:
  				align();
                                Serial.println("ALIGN STATE");
  				break;
  			case POSITION:
  				position();
                                Serial.println("POSITION STATE");
  				break;
  			case MOVE:
  				move();
                                Serial.println("MOVE STATE");
  				break;
  			case PAINT:
                                Serial.println("PAINT STATE");
  				paint();
  				break;
  			case CORNER:
  				corner();
  				break;
  		}
 	}
  	
  	/*Serial.print("Wheel state: ");
  	Serial.println((int)wheel_state);
  	Serial.print("Drive state: ");
  	Serial.println((int)drive_state);*/
  	
  	//Serial.print("State: ");
  	//Serial.println((int)state);
  	//Serial.print("Alignment phase: ");
  	//Serial.println(rot_fb_drive);
  	/*if (readings_valid) {
  		Serial.print("FL reading: ");
  		Serial.println(uss_reading[FL]);
  		Serial.print("FR reading: ");
  		Serial.println(uss_reading[FR]);
  		Serial.print("Computed angle: ");
  		Serial.println(get_alignment(uss_reading[FL], uss_reading[FR]));
  	}*/
  	
  	cycle_duration = micros() - cycle_timestamp;
  	if (cycle_duration < LOOP_PERIOD && LOOP_PERIOD - cycle_duration > 100) {
  		delayMicroseconds(LOOP_PERIOD - cycle_duration - 100);
  	}
  	//Serial.print("Cycle duration: ");
  	//Serial.println(cycle_duration);
  	cycle++;
  	if (cycle == 100) {
  		cycle = 0;
  		//Serial.println(micros() - alt_timestamp);
  		alt_timestamp = micros();
  	}
//  }
}
