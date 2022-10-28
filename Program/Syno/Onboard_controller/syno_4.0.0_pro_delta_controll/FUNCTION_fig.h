/*
 *  THIS IS FOR THE FUNCTION VARIBLE AND BOOLEAN 
 */

//sandbag servo reverse
boolean SANDBAG_REVERSE = 0;


//AltHold PID
float pitch_revise;
float roll_revise;

float P = 1.5;
float I = 0.08;
float D = 2;

float PITCH_I_ERROR = 0;
float PITCH_INTERGRATE_ERROR = 0;

float PITCH_D_ERROR = 0;
float PITCH_PRE_D_ERROR = 0;
float PITCH_DELTA_ERROR = 0;
boolean PITCH_DELTA_SAVE = 0;


float ROLL_I_ERROR = 0;
float ROLL_INTERGRATE_ERROR = 0;

float ROLL_D_ERROR = 0;
float ROLL_PRE_D_ERROR = 0;
float ROLL_DELTA_ERROR = 0;
boolean ROLL_DELTA_SAVE = 0;

unsigned long D_TIMER = 0;

//------TIMER AND CHECKER------//

//DUTY TIMER
unsigned long DUTY_MS ;
int DUTY_SEC , DUTY_MIN;

//THROW 
unsigned long SERVO_TIMER;
int PWM_STATE = 1;
//THROW CHECK
int THROW_ANGLE = 0;
boolean HAS_THROW = 0;

//RC_READING DELAY
unsigned long RC_READING_TIMER = 0;
boolean THROW_IS_OK = 0;

//AUTO
boolean auto_start = 0;

//LEVEL CHECKER
//LEVEL STATUS: 0 = UNDONE , 1 = FINISH , 2 = PASS

int L1_CHECKER = 0;
bool take_off_start = 0;
int TAKE_OFF_TIMER[2] ;
String take_off_duty = "0";

int L2_CHECKER = 0;
bool L2_start = 0;
String L2_duty = "0";

int L3_CHECKER = 0;
bool L3_start = 0;
String L3_duty = "0";

int L4_CHECKER = 0;
boolean L4_start = 0;
String L4_duty = "0";

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

//BIAS
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

//tangent checker
boolean not_on_tangent = 0;
unsigned long on_tangent_counter = 0;

//red detect checker
boolean RED_DETECT = 0;
unsigned long RED_DETECT_TIMER = 0;

//CAN READING
boolean can_flash = 0;

//leave waiting area 
boolean leave_waiting = 0;
unsigned long leave_waiting_timer = 0; 

//fly speed PWM controll
unsigned long PWM_TIMER = 0;

//adjust timer
unsigned long ADJUST_TIMER = 0;

