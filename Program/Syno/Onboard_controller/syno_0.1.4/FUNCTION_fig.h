/*
 *  THIS IS FOR THE FUNCTION VARIBLE AND BOOLEAN 
 */

//DUTY TIMER
unsigned long DUTY_MS ;
int DUTY_SEC , DUTY_MIN;

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
int L4_CHECKER = 0;
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

//failsafe
int MIN_PWM = 1410;
int MAX_PWM = 1590;

//BIAS TO CONTROL
int X,Y,Z;
int PITCH_BIAS , ROLL_BIAS , THROTTLE_BIAS , YAW_BIAS;

//OPENMV INPUT CHAR
String MEMORY;
String OPENMV_DATA[4];
unsigned long openmv_time;

//PWM TIMER
unsigned long RC_1_rising_timing,RC_2_rising_timing,RC_3_rising_timing,RC_4_rising_timing,RC_5_rising_timing;
unsigned long pre_RC_1_rising_timing;
