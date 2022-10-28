/*
   --WAITADD : NEED TO ADJUST FUNCTION DETIAL
   --DEBUG : DEBUG OPTION
   --INSETUP : INPUT SETUP TAG
   --OUTSETUP : OUTPUT SETUP TAG
   --RFSETUP : nRF24L01 SETUP TAG
   --AMCHSE : AUTO/MANUAL MODE CHOOSE TAG
   --TAKEOFF : TAKE OFF FUNCTION TAG
   --ISRSETUP : ISR SETUP TAG
   --ISRPRO : ISR PROGRAM TAG

   --AUTOTIMEER : AUTO MODE TIME COUNTER
*/

#include <Wire.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"

#include "global.h"
#include "INPUT_PIN.h"
#include "OUT_PIN.h"
#include "nRF24L01_fig.h"
#include "FUNCTION_fig.h"
#include "PWM.hpp"

PWM CH1(2);
PWM CH2(3);
PWM CH3(18);
PWM CH4(19);
PWM CH5(20);

void setup() {

  Wire.begin();

  //USING FOR DEBUGGING --DEBUG
  //debug = 1;
  if (debug) {
    Serial.begin(19200);
  }

  //INPUT SET UP --INSETUP
  CH1.begin(true);
  CH2.begin(true);
  CH3.begin(true);
  CH4.begin(true);
  CH5.begin(true);

  //RC_RADIO ATTACH --OUTSETUP
  for (int i = 0; i < chanel_number; i++) {
    ppm[i] = default_servo_value;
  }

  ppm[2] = 1000;


  //PINMODE
  pinMode(A0, INPUT);

  //MODE_CODE
  MODE_CODE = 0; //SHOW AS NOT SETUP

  //nRF24L01 SET UP --RFSETUP
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(1);
  radio.setDataRate(RF24_2MBPS);  //MAX LENGTH IS 0.25MB/S
  radio.setCRCLength(RF24_CRC_8);
  radio.openWritingPipe(pipe[0]);
  radio.setAutoAck(0);
  radio.stopListening();

  RF24_DUTY_TIMER = millis();

  //ISR SETUP --ISRSETUP
  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)

  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;

  OCR1A = 100;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();

}

void loop_() {
  DATA_SEND();
}


void loop() {

  RC_READING();

  //FUNCTION CHOOSE --AMCHSE
  if (MODE_1_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_1_PWM + 50 ) {
    MODE_CODE = 1;
    MANUAL();
  } else if (MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50) {
    MODE_CODE = 2;
    AUTO();
  } else {
    MODE_CODE = 7;
  }

  DATA_SEND();

}



//MANUAL FUNCTION
void MANUAL() {

  ppm[0] = RC_1_PWM;
  ppm[1] = RC_2_PWM;
  ppm[2] = RC_3_PWM;
  ppm[3] = RC_4_PWM;
  ppm[4] = 970;

}


//AUTO FUNCTION
void AUTO() {

  if (debug) {
    Serial.println(F("=====AUTO_LOG====="));
  }

  //TAKE OFF --TAKEOFF
  TAKE_OFF_CHECK = 0;
  while (1) {
    //FOR BREAKING
    RC_READING();
    if ((MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50) and TAKE_OFF_CHECK == 0) {
      TAKE_OFF();
    } else {
      break;
    }
  }

}

void AUTO_2() {

  //START THE DUTY TIMER --AUTOTIMEER
  if (!auto_start) {
    DUTY_MS = millis();
    DUTY_SEC = 0;
    DUTY_MIN = 0;

    L1_TIME[0] = 0 ; L1_TIME[1] = 0;
    L2_TIME[0] = 0 ; L2_TIME[1] = 0;
    L3_TIME[0] = 0 ; L3_TIME[1] = 0;
    L4_TIME[0] = 0 ; L4_TIME[1] = 0;
    L5_TIME[0] = 0 ; L5_TIME[1] = 0;
    L6_TIME[0] = 0 ; L6_TIME[1] = 0;
    L7_TIME[0] = 0 ; L7_TIME[1] = 0;
    auto_start = 1;
  }

  //TIMER COUNTING
  if (auto_start and !FAILSAFE) {
    if (millis() - DUTY_MS >= 1000) {
      DUTY_SEC++;
      DUTY_MS = millis();
    }

  //COUNTING MIN&SEC
  if (DUTY_SEC >= 60) {
      DUTY_MIN ++;
      DUTY_SEC = 0;
    }
  }


  RC_READING();
  if (!FAILSAFE) {
      if      (LEVEL_SWITCH == 0 & L1_CHECKER == 0) {
      L1();
    } else if (LEVEL_SWITCH == 1 & L2_CHECKER == 0) {
      //P2();
    } else if (LEVEL_SWITCH == 2 & L3_CHECKER == 0) {
      //P3();
    } else if (LEVEL_SWITCH == 3 & L4_CHECKER == 0) {
      //P4();
    } else if (LEVEL_SWITCH == 4 & L5_CHECKER == 0) {
      //P5();
    } else if (LEVEL_SWITCH == 5 & L6_CHECKER == 0) {
      //P6();
    } else if (LEVEL_SWITCH == 6 & L7_CHECKER == 0) {
      //P7();
    } else if (LEVEL_SWITCH == 7) {
      //STOP();
    }else{
      //error();
    }

    if (!(MIN_PWM < RC_1_PWM < MAX_PWM) or !(MIN_PWM < RC_2_PWM < MAX_PWM) or !(MIN_PWM < RC_3_PWM < MAX_PWM) or !(MIN_PWM < RC_4_PWM < MAX_PWM)) {
      FAILSAFE = 1;
    }

  } else {
    MANUAL();
  }

}

//DRONE BASIC FUNCTION

void THROW() {

}
void RC_READING() {

  RC_1_PWM = CH1.getValue();
  RC_2_PWM = CH2.getValue();
  RC_3_PWM = CH3.getValue();
  RC_4_PWM = CH4.getValue();
  RC_5_PWM = CH5.getValue();


  if (1/*debug*/) {
    //Serial.println(F("=====RC_INPUT====="));
    Serial.print(RC_1_PWM);
    Serial.print(F(" "));
    Serial.print(RC_2_PWM);
    Serial.print(F(" "));
    Serial.print(RC_3_PWM);
    Serial.print(F(" "));
    Serial.print(RC_4_PWM);
    Serial.print(F(" "));
    Serial.print(RC_5_PWM);
    Serial.println();
  }
}
void DATA_SEND() {

  if ( millis() - RF24_DUTY_TIMER > RF24_DUTY_TIME ) {

    SEND_DATA.RC_INPUT[0]   =  RC_1_PWM ;
    SEND_DATA.RC_INPUT[1]   =  RC_2_PWM ;
    SEND_DATA.RC_INPUT[2]   =  RC_3_PWM ;
    SEND_DATA.RC_INPUT[3]   =  RC_4_PWM ;
    SEND_DATA.RC_INPUT[4]   =  RC_5_PWM ;
    SEND_DATA.CH_OUTPUT[0]  =  ppm[0];
    SEND_DATA.CH_OUTPUT[1]  =  ppm[1];
    SEND_DATA.CH_OUTPUT[2]  =  ppm[2];
    SEND_DATA.CH_OUTPUT[3]  =  ppm[3];
    SEND_DATA.VOLTAGE       = analogRead(A0);
    SEND_DATA.OPENMV_STATUS = 1101;
    SEND_DATA.MODE          = MODE_CODE;
    SEND_DATA.STATUS_CODE   = LEVEL_CODE ;

    radio.write(&SEND_DATA, sizeof(SEND_DATA));

    RF24_DUTY_TIMER = millis();

  }

}
void READ_OPENMV(){
  
}

//LEVEL FUNCTION
void L1() {
  
  if (!take_off_start) {
    take_off_timer[0] = DUTY_MIN;
    take_off_timer[1] = DUTY_SEC;
    take_off_duty = B00000000;
    take_off_start = 1;
  }

  if (take_off_start) {
    //ARM
    if (take_off_duty == B00000000) {
      ppm[0] = 1400 ;
      ppm[1] = 1400 ;
      ppm[2] = 1000 ;
      ppm[3] = 1400 ;
      ppm[4] = 1400 ;
      if(( DUTY_MIN*60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0]*60 + TAKE_OFF_TIMER[1] ) > 8){
        take_off_timer[0] = DUTY_MIN;
        take_off_timer[1] = DUTY_SEC;
        take_off_duty = B00000001;
      }
    }
    else if (take_off_duty == B00000001){
      //TAKE_OFF
      ppm[0] = 1400 ;
      ppm[1] = 1400 ;
      ppm[2] = 1000 ;
      ppm[3] = 1790 ;
      ppm[4] = 1400 ;
      if(( DUTY_MIN*60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0]*60 + TAKE_OFF_TIMER[1] ) > 6){
        take_off_timer[0] = DUTY_MIN;
        take_off_timer[1] = DUTY_SEC;
        take_off_duty = B00000011;
      }
    }
    else if (take_off_duty == B00000011){
      //NEED DETECT HEIGHT，OpenMV
      READ_OPENMV();
      ppm[0] = 1400;
      ppm[1] = 1400;
      ppm[2] = 1400 + (analogRead(SONAR) - IDEAL_HEIGHT);
      ppm[3] = 1400;
      ppm[4] = 1400;
      if(( DUTY_MIN*60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0]*60 + TAKE_OFF_TIMER[1] ) > 4){
        take_off_timer[0] = DUTY_MIN;
        take_off_timer[1] = DUTY_SEC;
        take_off_duty = B00000111;
      }
      
    }
  }
}

void L2(){
}

void L3(){
}

// ISR PROGRAM --ISRPRO
ISR(TIMER1_COMPA_vect) { //leave this alone
  static boolean state = true;

  TCNT1 = 0;

  if (state) { //start pulse
    digitalWrite(sigPin, onState);
    OCR1A = PPM_PulseLen * 2;
    state = false;
  }
  else { //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;

    digitalWrite(sigPin, !onState);
    state = true;

    if (cur_chan_numb >= chanel_number) {
      cur_chan_numb = 0;
      calc_rest = calc_rest + PPM_PulseLen;//
      OCR1A = (PPM_FrLen - calc_rest) * 2;
      calc_rest = 0;
    }
    else {
      OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) * 2;
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }
  }
  }
