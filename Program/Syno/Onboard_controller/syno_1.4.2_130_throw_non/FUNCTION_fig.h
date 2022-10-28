/*
 *  THIS IS FOR THE FUNCTION VARIBLE AND BOOLEAN 
 */

//DUTY TIMER
unsigned long DUTY_MS ;
int DUTY_SEC , DUTY_MIN;

//THROW 
unsigned long SERVO_TIMER;
int PWM_STATE = 1;
boolean THROW_ACT = 0;

//AUTO
boolean auto_start = 0;

//LEVEL CHECKER
//LEVEL STATUS: 0 = UNDONE , 1 = FINISH , 2 = PASS

int L1_CHECKER = 0;
bool take_off_start = 0;
int TAKE_OFF_TIMER[2] ;
byte take_off_duty = B00000000;

int L2_CHECKER = 0;
bool L2_start = 0;
byte L2_duty = B00000000;

int L3_CHECKER = 0;
bool L3_start = 0;
byte L3_duty = B00000000;

int L4_CHECKER = 0;
boolean L4_start = 0;
byte L4_duty = B00000000;

int L5_CHECKER = 0;
int L6_CHECKER = 0;
int L7_CHECKER = 0;

int8_t LEVEL_SWITCH = B00000000;

int L1_TIME[2] = {0,0};
int L2_TIME[2] = {0,0};
int L3_TIME[2] = {0,0};
int L4_TIME[2] = {0,0};
int L5_TIME[2] = {0,0};
int L6_TIME[2] = {0,0};
int L7_TIME[2] = {0,0};

//failsafe boolean
boolean FAILSAFE = 0;
int failsafe_counter = 0;

//failsafe
int MIN_PWM = 1410;
int MAX_PWM = 1590;

//BIAS TO CONTROL
int REVISE_VALUE = 100;//PWM REVISE TO ADJUST POSITION
int ANGLE_VALUE = 60;

int X,Y,Z;
int PITCH_BIAS , ROLL_BIAS , THROTTLE_BIAS , YAW_BIAS;

//OPENMV INPUT CHAR
int OPENMV_DATA[4];
unsigned long openmv_time = 0;

//mission timer
unsigned long color_detect_counter = 0;
unsigned long tangent_detect_counter = 0;
unsigned long throw_counter = 0;

//LED BLINK
unsigned long LED_TIMER = 0;

//THROW CHECK
boolean HAS_THROW = 0;
