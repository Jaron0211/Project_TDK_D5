/*
 * THIS DEFINATION IS FOR OUTPUT PIN
 */
 
//////////////////////CONFIGURATION///////////////////////////////
#define chanel_number 5  //set the number of chanels
#define default_servo_value 1500  //set the default servo value
#define PPM_FrLen 20000  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PPM_PulseLen 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 29  //set PPM signal output pin on the arduino
//////////////////////////////////////////////////////////////////

int ppm[chanel_number];

#define SANDBAG A1 
//1pitch 2roll 3thruts 4yaw

/*
Servo CH1 , CH2 , CH3 , CH4 , CH5 , CH6;

#define ch1 13
#define ch2 11
#define ch3 9
#define ch4 7
#define ch5 5
#define ch6 3

*/
