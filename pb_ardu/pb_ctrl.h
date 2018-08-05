#include <math.h>
#include <Servo.h>
#include <Arduino.h>



// Control Loop ---------------------------------------------------------------------------------------

// Target frequency of main loop in cycles/second
#define LOOP_FREQ 100
#define LOOP_PERIOD 1000000L/LOOP_FREQ
extern unsigned long cycle_timestamp, cycle_duration;

typedef enum {IDLE, ALIGN, POSITION, MOVE, PAINT, CORNER} state_t;
extern state_t state;

extern unsigned long wait_timestamp, wait_duration;
void wait_micros(unsigned long duration);

void activeStatePB();
void reset();
bool check_time(unsigned long timestamp, unsigned long interval);

// ----------------------------------------------------------------------------------------------------


// Motors ---------------------------------------------------------------------------------------------

typedef enum {FL, FR, BL, BR} motor_t;

//Drive Motors
extern Servo drive_motor [4];
extern const int drive_motor_pin [4];

//Drive motor constants
#define DRIVE_MOTOR_MAX_RPM 50
#define DRIVE_WHEEL_RADIUS 1.25
#define DRIVE_MOTOR_RADIUS 16

#define FORWARD_CMD 135
#define STOP_CMD 90
#define REVERSE_CMD 45

//Paint trigger motor constants
#define PAINT_TRIG_OFF 90
#define PAINT_TRIG_ON 150

//Tower motor constants
#define TOWER_SPEED 5

//Pivot Motors
extern Servo pivot_motor [4];
extern const int pivot_motor_pin [4];
extern const int pivot_motor_position [4][3];

//Tower Motor
extern Servo paint_trigger_servo;
extern const int tower_pul_pin;
extern const int tower_dir_pin;
extern const int tower_enbl_pin;
extern const int paint_trig_pin;

// ----------------------------------------------------------------------------------------------------


// Image capture --------------------------------------------------------------------------------------

#define BYTES_PER_CAPTURE 15
#define CAPTURES_PER_STRIP 6
#define EXPECTED_PIXEL_COUNT_LONG (long)BYTES_PER_CAPTURE*CAPTURES_PER_STRIP
#define EXPECTED_PIXEL_COUNT BYTES_PER_CAPTURE*CAPTURES_PER_STRIP
#define TOWER_HEIGHT 290000

extern unsigned int data[EXPECTED_PIXEL_COUNT];
extern unsigned int cycle;
extern unsigned int active;
extern char byteReceived;

// ----------------------------------------------------------------------------------------------------


// Ultrasonic sensors (uss) ---------------------------------------------------------------------------

#define SPEED_OF_SOUND 343.2
#define FRONT_SENSOR_DIST 20

#define FRONT_SENSOR_RADIUS 20
#define FRONT_SENSOR_ANGLE M_PI/4

typedef enum {S_FL, S_FR, S_RC} uss_t;

typedef enum {DISABLED, FRONT, ALL} uss_state_t;
extern uss_state_t uss_state;

void set_uss_state(uss_state_t next_uss_state);

extern const int trig_pin [3];
extern const int echo_pin [3];

extern float uss_reading [3];
extern bool readings_valid;
extern uss_t current_sensor;

void read_uss();

float read_uss(uss_t uss);

// ----------------------------------------------------------------------------------------------------


// Drive ----------------------------------------------------------------------------------------------

#define DRIVE_FB_SPEED 5
#define DRIVE_LR_SPEED 5
#define ROTATE_SPEED 5

#define PIVOT_DELAY 1000000L

typedef enum {DRIVE_FB, ROTATE, DRIVE_LR, UNDEFINED} wheel_state_t;
extern wheel_state_t wheel_state;

typedef enum {STOP, FORWARD, REVERSE, D_UNDEFINED} drive_state_t;
extern drive_state_t drive_state;

void write_drive_state();
void pivot_wheels(wheel_state_t next_wheel_state);
void set_drive_state(drive_state_t new_drive_state);
void set_drive_speed(float speed);
void set_drive_duration(unsigned long duration);
bool moving();

extern unsigned long drive_timestamp, drive_duration;

// ----------------------------------------------------------------------------------------------------


// STATES =============================================================================================


// Alignment ------------------------------------------------------------------------------------------

#define ANGLE_TOLERANCE 0.1
#define ALIGNMENT_TOLERANCE 0.5

float get_alignment(float d_l, float d_r);

bool check_alignment();

void to_align_state();
void align();

// ----------------------------------------------------------------------------------------------------


// Positioning ----------------------------------------------------------------------------------------

#define TARGET_DISTANCE 8
#define POSITIONING_TOLERANCE 0.25

float get_distance_to_wall(float d_1, float d_r, float phi);
float estimate_distance_to_wall();

bool check_positioning();

void to_position_state();
void position();

// ----------------------------------------------------------------------------------------------------


// Movement (along wall) ------------------------------------------------------------------------------

#define COLUMN_DRIVE_TIME 1000000L
#define TURN_DISTANCE 13
#define MOVEMENT_POSITIONING_TOLERANCE 2

bool check_distance_to_corner();
bool check_movement_positioning();

void to_move_state();
void move();

// ----------------------------------------------------------------------------------------------------


// Column Painting ------------------------------------------------------------------------------------

void to_paint_state();
void paint();
void slide(int dir, long steps, int sspeed);
void getNextStripData();

// ----------------------------------------------------------------------------------------------------


// Corner Handling ------------------------------------------------------------------------------------

#define ROTATE_90_TIME 4000

void to_corner_state();
void corner();

// ----------------------------------------------------------------------------------------------------
