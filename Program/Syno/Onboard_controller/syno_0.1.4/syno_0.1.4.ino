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
#include <SoftwareSerial.h>
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
//#include "PWM.hpp"

#include <Adafruit_NeoPixel.h>
#define LED_PIN    6
#define LED_COUNT  1
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

/*
 * 2->INT4
 * 3->INT5
 * 18->INT3
 * 19->INT2
 * 20->INT1
 * 
PWM CH1(2);
PWM CH2(3);
PWM CH3(18);
PWM CH4(19);
PWM CH5(20);
*/

SoftwareSerial openmv(10, 11);

void setup() {

  debug = 1;

  strip.begin();           
  strip.show();            
  strip.setBrightness(50); 

  openmv.begin(9600);
  openmv.setTimeout(80);
  
  Wire.begin();  
  
  //USING FOR DEBUGGING --DEBUG
  //debug = 1;

  if (debug) {
    Serial.begin(9600);
  }

  //INPUT SET UP --INSETUP
  /*
  CH1.begin(true);
  CH2.begin(true);
  CH3.begin(true);
  CH4.begin(true);
  CH5.begin(true);
  */

  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), RC_1_RISING , RISING );
  attachInterrupt(digitalPinToInterrupt(3), RC_2_RISING , RISING );
  attachInterrupt(digitalPinToInterrupt(18), RC_3_RISING , RISING );
  attachInterrupt(digitalPinToInterrupt(19), RC_4_RISING , RISING );
  attachInterrupt(digitalPinToInterrupt(20), RC_5_RISING , RISING );
  
  
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

  //openmv timer
  openmv_time = millis();

  delay(100);
  strip.setPixelColor(0,255,   255,   255) ;
  strip.show();
  delay(200);
  strip.setPixelColor(0,0,   255,   0) ;
  strip.show();
  delay(100);
  strip.setPixelColor(0,0,   0,   255) ;
  strip.show();
  delay(200);
  strip.setPixelColor(0,255,   0,   0) ;
  strip.show();
  delay(100);
  strip.setPixelColor(0,0,0,0) ;
  strip.show();

  delay(1000);

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

  EIMSK = b00111110;
  EICRA = b01010101;
  EICRB = b01010101;
  
  sei();
  
  
  delay(100);
  
  strip.setPixelColor(0,255,   255,   255) ;
  strip.show();
  delay(200);
  strip.setPixelColor(0,0,   0,   0) ;
  strip.show();
}

void loop(){
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

  ppm[0] = RC_1_PWM;
  ppm[1] = RC_2_PWM;
  ppm[2] = RC_3_PWM;
  ppm[3] = RC_4_PWM;
  ppm[4] = RC_5_PWM;

  delay(50);
}

void loop_1() {

  RC_READING();
  //DATA_SEND();

  //FUNCTION CHOOSE --AMCHSE
  if ((MODE_1_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_1_PWM + 50 )) {
    strip.setPixelColor(0,0,255,0) ;
    MODE_CODE = 1;
    MANUAL();
    RESET();
  } else if (not(!(MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50) and !FAILSAFE)) {
    MODE_CODE = 2;
    AUTO_2();
  } else if(RC_5_PWM < 1000 or RC_5_PWM > 2000){
    strip.setPixelColor(0,50,0,120) ;
  } else {
    strip.setPixelColor(0,0,0,255) ;
    prepare();
    MODE_CODE = 7;
  }

  strip.show();
  
  if(debug){
    Serial.print(F("FAILSAFE: "));
    Serial.println(FAILSAFE);
    Serial.print(F("boolean check: "));
    Serial.println((not(!(MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50) and !FAILSAFE)));
  }

}



//MANUAL FUNCTION
void MANUAL() {

  RC_1_OUTPUT = 0.8 * RC_1_OUTPUT + 0.2 * RC_1_PWM;
  RC_2_OUTPUT = 0.8 * RC_2_OUTPUT + 0.2 * RC_2_PWM;
  RC_3_OUTPUT = 0.8 * RC_3_OUTPUT + 0.2 * RC_3_PWM;
  RC_4_OUTPUT = 0.8 * RC_4_OUTPUT + 0.2 * RC_4_PWM;

  ppm[0] = RC_1_OUTPUT;
  ppm[1] = RC_2_OUTPUT;
  ppm[2] = RC_3_OUTPUT;
  ppm[3] = RC_4_OUTPUT;
  ppm[4] = Stabilize;

  if(debug){
    Serial.print(RC_1_OUTPUT);
    Serial.print(F(" "));
    Serial.print(RC_2_OUTPUT);
    Serial.print(F(" "));
    Serial.print(RC_3_OUTPUT);
    Serial.print(F(" "));
    Serial.print(RC_4_OUTPUT);
    Serial.print(F(" "));
    Serial.print(RC_5_PWM);
    Serial.println();
  }
}

//PREPARE FUNCTION
void prepare(){
  
  ppm[0] = 1500;
  ppm[1] = 1500;
  ppm[2] = 1060;
  ppm[3] = 1500;
  ppm[4] = 1200;
  
}

//AUTO FUNCTION
void AUTO_2() {

  //START THE DUTY TIMER --AUTOTIMEER
  if (debug) {
    Serial.println(F("=====AUTO_LOG====="));
  }
  
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
    Serial.println(F("time counting.."));
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

  if(debug){
    Serial.print(F("DUTY_TIMER: "));
    Serial.print(DUTY_MIN);
    Serial.print(F(" ; "));
    Serial.println(DUTY_SEC);
  }


  RC_READING();
  if (FAILSAFE == 0) {
    if      (LEVEL_SWITCH == 0 & L1_CHECKER == 0) {
      L1();
    } else if (LEVEL_SWITCH == 1 & L2_CHECKER == 0) {
      L2();
    } else if (LEVEL_SWITCH == 2 & L3_CHECKER == 0) {
      L3();
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
    } else {
      //error();
    }
    strip.setPixelColor(0,255,255,255) ;

    if (!(MIN_PWM < RC_1_PWM and RC_1_PWM < MAX_PWM) or !(MIN_PWM < RC_2_PWM and RC_2_PWM < MAX_PWM) or !(MIN_PWM < RC_3_PWM and RC_3_PWM < MAX_PWM) or !(MIN_PWM < RC_4_PWM and RC_4_PWM< MAX_PWM)) {
      Serial.println(F("FAILSAFE CHECK"));
      FAILSAFE = 1;

    }

  } else {
    strip.setPixelColor(0,255,0,0) ;
    MANUAL();
  }

}

void RESET(){
  
  auto_start = 0;
  L1_CHECKER = 0;
  take_off_start = 0;
  TAKE_OFF_TIMER[0] = 0 ;
  TAKE_OFF_TIMER[1] = 0 ;
  take_off_duty = B00000000;

  L2_CHECKER = 0;
  L2_start = 0;
  L2_duty = B00000000;

  L3_CHECKER = 0;
  L4_CHECKER = 0;
  L5_CHECKER = 0;
  L6_CHECKER = 0;
  L7_CHECKER = 0;

  LEVEL_SWITCH = B00000000;

  L1_TIME[0] =  0;
  L1_TIME[1] =  0;
  L2_TIME[0] =  0;
  L2_TIME[1] =  0;
  L3_TIME[0] =  0;
  L3_TIME[1] =  0;
  L4_TIME[0] =  0;
  L4_TIME[1] =  0;
  L5_TIME[0] =  0;
  L5_TIME[1] =  0;
  L6_TIME[0] =  0;
  L6_TIME[1] =  0;
  L7_TIME[0] =  0;
  L7_TIME[1] =  0;
  

  FAILSAFE = 0;

  X = 0;
  Y = 0;
  Z = 0;
  
  PITCH_BIAS = 0;
  ROLL_BIAS = 0;
  THROTTLE_BIAS = 0;
  YAW_BIAS = 0;

  MEMORY = " ";
  
  OPENMV_DATA[0] = "0";
  OPENMV_DATA[1] = "0";
  OPENMV_DATA[2] = "0";
  OPENMV_DATA[3] = "0";
  
  openmv_time = millis();

}
//DRONE BASIC FUNCTION

void THROW() {

}
void RC_READING() {
  /*
  RC_1_PWM = CH1.getValue();
  RC_2_PWM = CH2.getValue();
  RC_3_PWM = CH3.getValue();
  RC_4_PWM = CH4.getValue();
  RC_5_PWM = CH5.getValue();
  */

  if (0/*debug*/) {
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

void OPENMV_GET(int MODE_NUM) {
    
  if(millis() - openmv_time >= 100){
    if(MODE_NUM == 1){
      openmv.write("1");
    }else if(MODE_NUM == 2){
      openmv.write("2");
    }else if(MODE_NUM == 3){
      openmv.write("3");
    }
    
    openmv_time = millis();
  }
  if (openmv.available()) {
    String c = openmv.readStringUntil('\n');
    Serial.print(c);
    Serial.println();
    
    int first_split = c.indexOf(';');
    OPENMV_DATA[0] = c.substring(0, first_split).toInt();

    int second_spilt = c.indexOf(';', first_split + 1);
    OPENMV_DATA[1] = c.substring(first_split + 1, second_spilt).toFloat();

    int third_spilt = c.indexOf(';',second_spilt + 1);
    OPENMV_DATA[2] = c.substring(second_spilt + 1, third_spilt).toFloat();
    
    OPENMV_DATA[3] = c.substring(third_spilt + 1, int(c.length())).toFloat();
    
    Serial.print(OPENMV_DATA[0]);
    Serial.print(",");
    Serial.print(OPENMV_DATA[1]);
    Serial.print(",");
    Serial.print(OPENMV_DATA[2]);
    Serial.print(",");
    Serial.println(OPENMV_DATA[3]);

    if(OPENMV_DATA[0] == "1"){
      strip.setPixelColor(0,255,   0,   0) ;
      strip.show(); 
    }else if(OPENMV_DATA[0] == "2"){
      strip.setPixelColor(0,0,   0,   255) ;
      strip.show(); 
    }else if(OPENMV_DATA[0] == "3"){
      strip.setPixelColor(0,0,   255,   0) ;
      strip.show(); 
    }else if(OPENMV_DATA[0] == "0"){
      strip.setPixelColor(0,0,   0,   0);
      strip.show();
    }
    
  }

}


void HEIGHT() {

}

//COMBINE FUNCTON
void LINE_FOLLOW() {
  OPENMV_GET(2);
  PITCH_BIAS = 0;
  ROLL_BIAS = 0;
  YAW_BIAS = 0;

}

//LEVEL FUNCTION
void L1() {

  if (!take_off_start and !FAILSAFE) {
    TAKE_OFF_TIMER[0] = DUTY_MIN;
    TAKE_OFF_TIMER[1] = DUTY_SEC;
    take_off_duty = B00000000;
    take_off_start = 1;
    if(debug){
      Serial.println(F("take_off_start!"));
    }
  }

  if (take_off_start) {
    if(debug){
      Serial.println(take_off_duty);
    }
    //PREPARE
    if (take_off_duty == B00000000) {
      Serial.println(F("preparing"));
      ppm[0] = 1500 ;
      ppm[1] = 1500 ;
      ppm[2] = 1060 ;
      ppm[3] = 1500 ;
      ppm[4] = AltHold ;
      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 8) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00000001;
      }
    }
    //ARM
    else if (take_off_duty == B00000001) {
      //TAKE_OFF
      ppm[0] = 1500 ;
      ppm[1] = 1500 ;
      ppm[2] = 1100 ;
      ppm[3] = 1900 ;
      ppm[4] = AltHold ;
      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 6) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00000011;
      }
    }
    
    //PREPARE
    else if (take_off_duty == B00000011) {
      //NEED DETECT HEIGHT，OpenMV
      //OPENMV_GET(3);
      ppm[0] = 1500;
      ppm[1] = 1500;
      ppm[2] = 1500;
      ppm[3] = 1500;
      ppm[4] = AltHold;
      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 4) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00000111;
      }
    }
    //TAKE_OFF WITH REVISE
    else if (take_off_duty == B00000111) {
      //NEED DETECT HEIGHT，OpenMV
      OPENMV_GET(3);
      ppm[0] = 1500 - PITCH_BIAS;
      ppm[1] = 1500 - ROLL_BIAS;
      ppm[2] = 1500 + (analogRead(SONAR) - IDEAL_HEIGHT);
      ppm[3] = 1500 - YAW_BIAS;
      ppm[4] = AltHold;
      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 7) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00001111;
      }
    }
    else if (take_off_duty == B00001111) {
      LEVEL_SWITCH = 1;
      L1_CHECKER = 1;
    }
  }
}

void L2() {
  if (!L2_start) {
    L2_TIME[0] = DUTY_MIN;
    L2_TIME[1] = DUTY_SEC;
    L2_duty = B00000000;
    L2_start = 1;
  }

  if (L2_start) {
    if (L2_duty == B00000000) {
      //NEED DETECT HEIGHT，OpenMV
      LINE_FOLLOW();
      ppm[0] = 1400 - PITCH_BIAS;
      ppm[1] = 1400 - ROLL_BIAS;
      ppm[2] = 1400 + HEIGHT_P * (analogRead(SONAR) - IDEAL_HEIGHT);
      ppm[3] = 1400 - YAW_BIAS;
      ppm[4] = AltHold;
      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 7) {
        L2_TIME[0] = DUTY_MIN;
        L2_TIME[1] = DUTY_SEC;
        L2_duty = B00000001;
      }
    }
  }
}

void L3() {
}

ISR(INT0_vect){
  
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
