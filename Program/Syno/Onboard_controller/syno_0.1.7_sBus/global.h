/*
 * THIS IS FOR GLOBAL VARIBLE
 */


boolean debug = 0;

int REVICE_VALUE = 100;

int Land = 1500;
int FlowHold = 1400;
int Stabilize = 1300; 

int MODE_1_PWM = 975;
int MODE_2_PWM = 2050;

int MODE_CODE = 0;
/*
 * 0 = NOT SETUP
 * 1 = MANUAL
 * 2 = AUTO
 * 7 = ERROR
 */
int LEVEL_CODE = 0;
/*
 * 2 DIGITAL NUM WILL SHOW THE STEP OF LEVEL
 * FIRST IS MAIN LEVEL, SECOND IS THE LEVEL STEP
 * EX: 13 MEANINGS AT THE FIRST LEVEL , AND DOING THE THIRD STEP OF THE FUNCTION
 * 
 * DEFINE THE ERROR CODE IS 77
 */

float IDEAL_HEIGHT = 0;
float mV_TO_M = 0;
int HEIGHT_P = 1;
/*
 * ADJUST HEIGHT_P TO INCREASE/DECREASE THE SPEED OF HEIGHT RESERVE, 
 * mV_TO_M IS USING TO CONVERT SONAR SIGNAL VOTAGE TO METER , 
 * IDEAL_HEIGHT IS OUT IDEAL HEIGHT THAT DRONE SHOULD BE MAINTAIN
 */

String  MISSION_COLOR = "";
