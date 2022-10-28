/*
 * THIS IS FOR THE RF24
 */

RF24 radio(49, 53);
const uint64_t pipe[2] = { 0xE8E8F0F1A1LL, 0xE8E8F0F1A2LL } ;
unsigned int RECEIVER_CHECK = 100;

unsigned long RF24_DUTY_TIME = 20; //1000ms SEND 1 TIMES
unsigned long RF24_DUTY_TIMER = 0;

typedef struct {

  int RC_INPUT[5];
  
  int CH_OUTPUT[5];

  int VOLTAGE;
  
  int OPENMV_STATUS;

  int MODE;

  int STATUS_CODE;

} DATA_STRUCT;

DATA_STRUCT SEND_DATA;
