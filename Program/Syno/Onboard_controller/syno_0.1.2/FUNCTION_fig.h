/*
 *  THIS IS FOR THE FUNCTION VARIBLE AND BOOLEAN 
 */

//TAKE_OFF_VALUE
boolean TAKE_OFF_CHECK = 1;

//TIMER VALUE,USING IN AUTO
unsigned long timer ;

//DUTY TIMER
unsigned long DUTY_MS ;
int DUTY_SEC , DUTY_MIN;

//AUTO
boolean auto_start;

//LEVEL CHECKER
//LEVEL STATUS: 0 = UNDONE , 1 = FINISH , 2 = PASS

int L1_CHECKER = 0;
bool take_off_starter = 0;
int take_off_timer[2] ;
byte take_off_duty = B00000000;

int L2_CHECKER = 0;
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
int MIN_PWM = 1430;
int MAX_PWM = 1570;
