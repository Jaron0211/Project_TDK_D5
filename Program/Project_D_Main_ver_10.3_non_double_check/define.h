
Pixy2I2C pixy;

Servo CH1 , CH2 , CH3 , CH4 , CH5 , THROW_CH , STAND_CH ;

RF24 radio(8, 10);
const uint64_t pipe[2] = { 0xE8E8F0F0A1LL, 0xE8E8F0F0A2LL } ;
unsigned int RECEIVER_CHECK = 100;

//level move check
boolean TAKE_OFF_CHECK = 0;
boolean PART1_CHECK = 0;
boolean PART2_CHECK = 0;
boolean PART3_CHECK = 0;
boolean PART4_CHECK = 0;
boolean THROW_STARTER = 0;
boolean throw_timer_enable = 0;
boolean stand_check = 0;
boolean MISSION_OVER = 0;
boolean LINE_GET = 0;
boolean BREAK_CHECKER = 0;

unsigned long LINE_GET_TIMER;
unsigned long timer ;
unsigned long throw_timer;
unsigned long THROW_STARTER_TIMER;

unsigned long led_timer;
unsigned long stand_timer;
unsigned long break_timer;
unsigned long break_stater_timer;

//pixy2
boolean PIXY_CHECK = 0;
boolean CCC_ATTACH_CHECK = 0;
boolean COLOR_READ_CHECK = 0;
boolean THROW_CHECK = 0;
unsigned long CCC_ATTACH_TIMER;
unsigned long COLOR_READ_TIMER;
int CCC_ATTACH_COLOR = 0;
int LINE_COUNTER = 0;

int pixy_middle;
float pixy_angle;
int pixy_ccc_x , pixy_ccc_y;

int LINE_DETECT_VALUE_DIF, LINE_DETECT_VALUE_PRE, LINE_DETECT_VALUE_INTE;
int X_CCC_DETECT_VALUE_DIF, X_CCC_DETECT_VALUE_PRE;
int Y_CCC_DETECT_VALUE_DIF, Y_CCC_DETECT_VALUE_PRE;

int8_t x0, y0, x1, y1;
uint8_t r, g, b;

//PIGHT ANGLE compare

int X0[10], Y0[10], X1[10], Y1[10];
float Vx[10], Vy[10];

int start_x[10], start_y[10];
int start_x_count = 0;
int end_x[10], end_y[10];
int middle_x[10], middle_y[10];
int vector_saver[20] ;
int point_1[2], point_2[2];

int num;
int num_2;
float cross_checker;

//line compare

float LINE_Comparison[6];

int LINE_X0[10], LINE_Y0[10], LINE_X1[10], LINE_Y1[10];
float angle[10];
int line_num = 0;
boolean LINE_DETECT_KEEP = 1;

unsigned long right_turn_timer = 0;
int right_turn_count = 0;
int right_turn_num = 0;
boolean right_turn_enable = 0;
unsigned long turn_timer = 0;
boolean turn_over = 0;
int turn_over_2 = 0;


//line checking Anti-disease mechanism

int averge_middle[20] = {};

int LINE_PID_VALUE ;

//data class
typedef struct {

  int pitch_d;
  int roll_d;
  int yaw_d;
  int ax;
  int ay;
  int az;
  int gx;
  int gy;
  int gz;

} Print_Data;
Print_Data value;

typedef struct {

  int THROTTLE;
  int YAW;
  int PITCH;
  int ROLL;
  int FUNC_1;
  int FUNC_2;
  int MODE;
  int Test;

} RECEIVE_DATA;
RECEIVE_DATA receive_data;



