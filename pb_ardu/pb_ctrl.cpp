#include "pb_ctrl.h"

// Control Loop ---------------------------------------------------------------------------------------

unsigned long cycle_timestamp, cycle_duration;
unsigned long wait_timestamp, wait_duration = 0;
unsigned int active = 0;
unsigned int cycle = 0;

state_t state = IDLE;

void wait_micros(unsigned long duration) {
	wait_duration = duration;
	wait_timestamp = micros();
}

bool check_time(unsigned long timestamp, unsigned long interval) {
	unsigned long current_time = micros();
	unsigned long elapsed_time = current_time - timestamp;
	long remaining_time = interval - elapsed_time;
	
	if (remaining_time < LOOP_PERIOD) {
		if (remaining_time > 0) {
			delayMicroseconds(remaining_time);
		}
		return true;
	} else {
		return false;
	}
}

//Reset some globals
void reset() {

  active = 0;
  cycle = 0;
  
  //State setup
  drive_state = D_UNDEFINED;
  wheel_state = UNDEFINED;
  set_drive_state(STOP);

  //Set pivot motors to default position
  pivot_wheels(DRIVE_LR);

  //Set tower variables to default values
  memset(data,0,sizeof(data)); //set data array to be all 0
  paint_trigger_servo.write(PAINT_TRIG_OFF);
}

// ----------------------------------------------------------------------------------------------------


// Motors ---------------------------------------------------------------------------------------------

//Drive motors
Servo drive_motor [4];
const int drive_motor_pin [4] = {3, 6, 9, 12};

//Pivot motors
Servo pivot_motor [4];
const int pivot_motor_pin [4] = {4, 7, 10, 13};
const int pivot_motor_position [4][3] = {
	{0, 35, 70},
	{75, 40, 5},
	{75, 40, 5},
	{5, 40, 75}
};

//Tower motors
Servo paint_trigger_servo;
const int tower_dir_pin = 5;
const int tower_pul_pin = 8;
const int tower_enbl_pin = 2;
const int paint_trig_pin = 11;

// ----------------------------------------------------------------------------------------------------


// Ultrasonic sensors (uss) ---------------------------------------------------------------------------

uss_state_t uss_state = DISABLED;

const int trig_pin [3] = {24, 26, 28};
const int echo_pin [3] = {25, 27, 29};

float uss_reading [3];
bool readings_valid = false;
uss_t current_sensor = S_FL;

void set_uss_state(uss_state_t next_uss_state) {
	if (next_uss_state != uss_state) {
		readings_valid = false;
		current_sensor = S_FL;
		uss_state = next_uss_state;
	}
}

void read_uss() {
	if (uss_state != DISABLED) {
		if (current_sensor == S_RC) {
			uss_reading[current_sensor] = 100;
		} else {
			uss_reading[current_sensor] = read_uss(current_sensor);
		}
		/*Serial.print("Sensor: ");
		Serial.println((int)current_sensor);
		Serial.print("Reading: ");
		Serial.println(uss_reading[current_sensor]);*/
		
		int current_sensor_int = (int)current_sensor + 1;
		if (uss_state == FRONT) {
			current_sensor_int &= 1;
		} else {
			current_sensor_int %= 3;
		}
		current_sensor = (uss_t)current_sensor_int;
		
		if (current_sensor_int == 0) {
			readings_valid = true;
			
			for (int k = 0; k < (uss_state == FRONT ? 2 : 3); k++) {
				if (uss_reading[k] < 1 || uss_reading[k] > 200) {
					readings_valid = false;
				}
			}
		}/* else {
			readings_valid = false;
		}*/
	}
}

float read_uss(uss_t uss) {
	// Clears the trigPinFL
	digitalWrite(trig_pin[uss], LOW);
	delayMicroseconds(2);
	// Sets the trigPin on HIGH state for 10 micro seconds
	digitalWrite(trig_pin[uss], HIGH);
	delayMicroseconds(10);
	digitalWrite(trig_pin[uss], LOW);
	// Reads the echoPin1, returns the sound wave travel time in microseconds
	float duration = pulseIn(echo_pin[uss], HIGH);
	// Calculating the distance
	//       (us)        (m/s)       (s/us) (cm/m)
	return duration * SPEED_OF_SOUND * 1e-6 * 100 / 2;
}

// ----------------------------------------------------------------------------------------------------


// Drive ----------------------------------------------------------------------------------------------

wheel_state_t wheel_state = UNDEFINED; // Don't know initial wheel orientation.
drive_state_t drive_state = STOP;
unsigned long drive_timestamp, drive_duration = 0;

void pivot_wheels(wheel_state_t next_wheel_state) {
	if (next_wheel_state != UNDEFINED && next_wheel_state != wheel_state) {
		for (int k = 0; k < 4; k++) {
			pivot_motor[k].write(pivot_motor_position[k][next_wheel_state]);
		}
		
		wheel_state = next_wheel_state;
	}
	wait_micros(PIVOT_DELAY);
}

void write_drive_state() {
	switch (drive_state) {
		case STOP:
			drive_motor[FL].write(STOP_CMD);
			drive_motor[FR].write(STOP_CMD);
			drive_motor[BL].write(STOP_CMD);
			drive_motor[BR].write(STOP_CMD);
			//Serial.println("STOP");
			break;
		case FORWARD:
			switch (wheel_state) {
				case DRIVE_FB:
					drive_motor[FL].write(FORWARD_CMD);
					drive_motor[FR].write(REVERSE_CMD);
					drive_motor[BL].write(FORWARD_CMD);
					drive_motor[BR].write(REVERSE_CMD);
			//		Serial.println("FORWARD");
					break;
				case ROTATE:
					drive_motor[FL].write(FORWARD_CMD);
					drive_motor[FR].write(FORWARD_CMD);
					drive_motor[BL].write(FORWARD_CMD);
					drive_motor[BR].write(FORWARD_CMD);
			//		Serial.println("CLOCKWISE");
					break;
				case DRIVE_LR:
					drive_motor[FL].write(FORWARD_CMD);
					drive_motor[FR].write(FORWARD_CMD);
					drive_motor[BL].write(REVERSE_CMD);
					drive_motor[BR].write(REVERSE_CMD);
			//		Serial.println("RIGHT");
					break;
			}
			break;
		case REVERSE:
			switch (wheel_state) {
				case DRIVE_FB:
					drive_motor[FL].write(REVERSE_CMD);
					drive_motor[FR].write(FORWARD_CMD);
					drive_motor[BL].write(REVERSE_CMD);
					drive_motor[BR].write(FORWARD_CMD);
			//		Serial.println("BACKWARD");
					break;
				case ROTATE:
					drive_motor[FL].write(REVERSE_CMD);
					drive_motor[FR].write(REVERSE_CMD);
					drive_motor[BL].write(REVERSE_CMD);
					drive_motor[BR].write(REVERSE_CMD);
			//		Serial.println("ANTI-CLOCKWISE");
					break;
				case DRIVE_LR:
					drive_motor[FL].write(REVERSE_CMD);
					drive_motor[FR].write(REVERSE_CMD);
					drive_motor[BL].write(FORWARD_CMD);
					drive_motor[BR].write(FORWARD_CMD);
			//		Serial.println("LEFT");
					break;
			}
			break;
	}
}

void set_drive_state(drive_state_t new_drive_state) {
	//Serial.print("Changing drive state: ");
	if (new_drive_state != drive_state) {
		drive_state = new_drive_state;
		write_drive_state();
	}
}

void set_drive_speed(float speed) {
	//                        cm/s -> rad/s                rad/s -> RPM    RPM -> command angle (+/- 90 max)
	int command_angle = (int)(speed / DRIVE_WHEEL_RADIUS * 60 / (2*M_PI) * 80 / DRIVE_MOTOR_MAX_RPM);
	
	if (command_angle < -80) {
		command_angle = -80;
	} else if (command_angle > 80) {
		command_angle = 80;
	}
	
	switch (wheel_state) {
		case DRIVE_FB:
			drive_motor[FL].write(95 + command_angle);
			drive_motor[FR].write(95 - command_angle);
			drive_motor[BL].write(95 + command_angle);
			drive_motor[BR].write(95 - command_angle);
			break;
		case ROTATE:
			drive_motor[FL].write(95 + command_angle);
			drive_motor[FR].write(95 + command_angle);
			drive_motor[BL].write(95 + command_angle);
			drive_motor[BR].write(95 + command_angle);
			break;
		case DRIVE_LR:
			drive_motor[FL].write(95 + command_angle);
			drive_motor[FR].write(95 + command_angle);
			drive_motor[BL].write(95 - command_angle);
			drive_motor[BR].write(95 - command_angle);
			break;
	}
}

void set_drive_duration(unsigned long duration) {
	drive_duration = duration;
	drive_timestamp = micros();
}

bool moving() {
	return drive_state != STOP;
}

// ----------------------------------------------------------------------------------------------------


// STATES =============================================================================================


// Alignment ------------------------------------------------------------------------------------------

float get_alignment(float d_l, float d_r) {
	return atan((d_l - d_r) / FRONT_SENSOR_DIST);
}

bool check_alignment() {
	return (abs(uss_reading[FL] - uss_reading[FR]) < ALIGNMENT_TOLERANCE);
}

bool continue_alignment() {
	if (drive_state == FORWARD) {
		return (uss_reading[FL] - uss_reading[FR] > ALIGNMENT_TOLERANCE);
	} else {
		return (uss_reading[FR] - uss_reading[FL] > ALIGNMENT_TOLERANCE);
	}
}

void to_align_state() {
	state = ALIGN;
	pivot_wheels(ROTATE);
	set_uss_state(FRONT);
}

void align() {
	if (readings_valid) {
		if (!moving()) {
			if (!check_alignment()) {
				if (uss_reading[FL] > uss_reading[FR]) {
					set_drive_state(FORWARD);
				} else {
					set_drive_state(REVERSE);
				}
			} else {
				to_position_state();
			}
		} else if (!continue_alignment()) {
			set_drive_state(STOP);
		}
	}
}

// ----------------------------------------------------------------------------------------------------


// Positioning ----------------------------------------------------------------------------------------

float get_distance_to_wall(float d_l, float d_r, float phi) {
	max(d_l, d_r)*sin(abs(phi)) - FRONT_SENSOR_RADIUS*cos(abs(phi) + FRONT_SENSOR_ANGLE);
}

float estimate_distance_to_wall() {
	return (uss_reading[S_FL] + uss_reading[S_FR]) / 2 - 5;
}

void to_position_state() {
	state = POSITION;
	pivot_wheels(DRIVE_FB);
	set_uss_state(FRONT);
}

bool check_positioning() {
	return abs(estimate_distance_to_wall() - TARGET_DISTANCE < POSITIONING_TOLERANCE);
}

bool continue_positioning() {
	if (drive_state == FORWARD) {
                Serial.println(TARGET_DISTANCE);
                Serial.println(estimate_distance_to_wall());
		return (estimate_distance_to_wall() - TARGET_DISTANCE > POSITIONING_TOLERANCE);
	} else {
                Serial.println(TARGET_DISTANCE);
                Serial.println(estimate_distance_to_wall());
		return (TARGET_DISTANCE - estimate_distance_to_wall() > POSITIONING_TOLERANCE);
	}
}

void position() {
  
	if (readings_valid) {
          Serial.println("Reading is Valid");
		if (!moving()) {
                        Serial.println("Not Moving");

			if (!check_positioning()) {
                          Serial.println("Not Checking Position");
				if (estimate_distance_to_wall() > TARGET_DISTANCE) {
					set_drive_state(FORWARD);
				} else {
					set_drive_state(REVERSE);
				}
			} else {
				to_paint_state();
                                Serial.println("To Paint State");
			}
		} else if (!continue_positioning()) {
			set_drive_state(STOP);
                        Serial.println("To Drive State");
		}
	}
}

// ----------------------------------------------------------------------------------------------------


// Movement (along wall) ------------------------------------------------------------------------------

bool check_distance_to_corner() {
	return (uss_reading[S_RC] > TURN_DISTANCE);
}

bool check_movement_positioning() {
	return (abs(estimate_distance_to_wall() - TARGET_DISTANCE) < MOVEMENT_POSITIONING_TOLERANCE);
}

void to_move_state() {
	state = MOVE;
	pivot_wheels(DRIVE_LR);
	set_uss_state(ALL);
}

void move() {
	if (readings_valid) {
		if (!moving()) {
			if (!check_distance_to_corner()) {
				to_corner_state();
			} else if (!check_movement_positioning()) {
				to_align_state();
			} else {
				set_drive_duration(COLUMN_DRIVE_TIME);
				set_drive_state(FORWARD);
			}
		} else if (!check_movement_positioning()) {
			set_drive_state(STOP);
		} else if (check_time(drive_timestamp, drive_duration)) {
			set_drive_state(STOP);
			to_paint_state();
		}
	}
}

// ----------------------------------------------------------------------------------------------------


// Column Painting ------------------------------------------------------------------------------------

unsigned int data[EXPECTED_PIXEL_COUNT];
char byteReceived;

void to_paint_state() {
	state = PAINT;
	set_uss_state(DISABLED);
	wait_micros(1000000L);
}

void paint() {

/*  
   Serial.print('S'); //Send start byte to PI       
    //Wait until PI sends start byte for confirmation
    while (byteReceived != 'S'){
      if(Serial.available() > 0) {
        byteReceived = Serial.read();
      } 
      delay(100);
    }
*/
  data[20] = 1;
  data[40] = 1;
  Serial.println("Entered Paint");
  for (int i = 0; i < EXPECTED_PIXEL_COUNT; i ++){
    Serial.print("Painting Up: ");
    Serial.println(i);
    if (data[i] == 1){
      paint_trigger_servo.write(PAINT_TRIG_OFF);
    } else {
      paint_trigger_servo.write(PAINT_TRIG_ON);
    }
    Serial.print("Expected Pixel Count: ");
    Serial.println(EXPECTED_PIXEL_COUNT);
    Serial.print("Tower Height: ");
    Serial.println(TOWER_HEIGHT);
//    slide('L', TOWER_HEIGHT/EXPECTED_PIXEL_COUNT_LONG, TOWER_SPEED); //go Left for 500 steps at speed 500, (the lower the number is the faster the motor will go)
    slide('L', 3222, 10); //go Left for 500 steps at speed 500, (the lower the number is the faster the motor will go)
  /*
    if (i % 15 == 0 ) {
      Serial.print('C');
    }
    */
  }
    
  delay(500);
      
  for (int i = (int)EXPECTED_PIXEL_COUNT - 1; i != -1; i--){
    Serial.print("Painting Down: ");
    Serial.println(i);
    if (data[i] == 1){
      paint_trigger_servo.write(PAINT_TRIG_OFF);
    } else {
      paint_trigger_servo.write(PAINT_TRIG_ON);
    }          
//    slide('R', TOWER_HEIGHT/EXPECTED_PIXEL_COUNT_LONG, TOWER_SPEED); //go Left for 500 steps at speed 500, (the lower the number is the faster the motor will go)
    slide('R', 3222, 10); //go Left for 500 steps at speed 500, (the lower the number is the faster the motor will go)
  }       
      
  delay(500);
  /*    
  Serial.print('D');
    
  while (byteReceived != 'D'){
    byteReceived = Serial.read(); 
    delay(100);
  }
 */
//  getNextStripData();  
  
  to_move_state();	// Do jack shit for now.
}

void slide(int dir, long steps, int sspeed){
//  Serial.println("Sliding");
//  Serial.print("Tower enable pin: ");
//  Serial.println(tower_enbl_pin);
//  Serial.print("Tower pull pin: ");
//  Serial.println(tower_pul_pin);
//  Serial.print("Tower direction pin: ");
//  Serial.println(tower_dir_pin);
//  
//  Serial.print("Steps: ");
//  Serial.println(steps);
  
  if (sspeed < 0) {
    //Serial.println("Speed < 10:");
    sspeed = 10; //keeps the speed above 10
  }
  //Serial.println(sspeed);
  if (dir == 'L') {
    //Serial.println("Sliding left");
    digitalWrite(tower_dir_pin, HIGH);
  }
  else if (dir == 'R') {
    //Serial.println("Sliding right");
    digitalWrite(tower_dir_pin, LOW);
  }
  
  while (steps > 0){
    //Serial.println(steps);
    digitalWrite(tower_pul_pin, HIGH);
    //tower_pul_pin.write(HIGH);
    delayMicroseconds(10);
    digitalWrite(tower_pul_pin, LOW);
    //tower_pul_pin.write(LOW);
    delayMicroseconds(10);
    delayMicroseconds(sspeed);
    steps = steps - 1;
  }
}

void activeStatePB(){
  //If incoming byte
  if (Serial.available() > 0){
    byteReceived = Serial.read();
    switch (byteReceived) {
      //ON
      case 'Y' :
        active = 1;
      break; 
      
      //OFF
      case 'N':
        reset();
      return;
    }
  }
}

//Run at the end of the tower movement (reaching top or bottom) ask PI for the data to be used for the next strip
void getNextStripData() {
  int i = 0;
  int bytes_read = 0;
  if (active == 1){
    Serial.print("R");
    while (bytes_read < (int)EXPECTED_PIXEL_COUNT){
      while (Serial.available() > 0){
        byteReceived = Serial.read();
        switch (byteReceived){        
          case 'N':         
          reset();
        default :
          data[i] = byteReceived + 0;
          bytes_read++;
          i++;                  
        }
      }      
    }
  }
}

// ----------------------------------------------------------------------------------------------------


// Corner Handling ------------------------------------------------------------------------------------

void to_corner_state() {
	state = CORNER;
	pivot_wheels(ROTATE);
	set_uss_state(ALL);
}

void corner() {
	
}

// ----------------------------------------------------------------------------------------------------
