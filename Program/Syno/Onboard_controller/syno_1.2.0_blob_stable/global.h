/*
 * THIS IS FOR GLOBAL VARIBLE
 */

int x_bias = 0;
int y_bias = 0;
int yaw_bias = 0;

boolean debug = 0;

int REVICE_VALUE = 100;

int loiter = 1150;
int Land = 1500;
int FlowHold = 1400;
int Stabilize = 1300; 

int MODE_1_PWM = 895;
int MODE_2_PWM = 2123;
int MODE_3_PWM = 1511;

int MODE_CODE = 0;
/*
 * 0 = NOT SETUP
 * 1 = MANUAL
 * 2 = AUTO
 * 3 = PREPARE
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

String  MISSION_COLOR = "";
