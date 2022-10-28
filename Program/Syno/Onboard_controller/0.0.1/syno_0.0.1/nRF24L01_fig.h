/*
 * THIS IS FOR THE RF24
 */

RF24 radio(47, 46);
const uint64_t pipe[2] = { 0xE8E8F0F0A1LL, 0xE8E8F0F0A2LL } ;
unsigned int RECEIVER_CHECK = 100;

typedef struct {

  int RC_INPUT[6];
  
  int CH_OUTPUT[5};

  int voltage;
  int gimbal_angle[2];
  
  int OpenMV_status;

} SEND_DATA;

SEND_DATA send_data;
