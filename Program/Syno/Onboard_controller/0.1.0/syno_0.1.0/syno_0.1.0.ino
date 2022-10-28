/*
 * --WAITADD : NEED TO ADJUST FUNCTION DETIAL
 * --DEBUG : DEBUG OPTION
 * --INSETUP : INPUT SETUP TAG
 * --OUTSETUP : OUTPUT SETUP TAG 
 * --RFSETUP : nRF24L01 SETUP TAG
 * --AMCHSE : AUTO/MANUAL MODE CHOOSE TAG
 * --TAKEOFF : TAKE OFF FUNCTION TAG
 * --ISRSETUP : ISR SETUP TAG
 * --ISRPRO : ISR PROGRAM TAG
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
  if(debug){
    Serial.begin(19200);
  }
  
  //INPUT SET UP --INSETUP
  CH1.begin(true);
  CH2.begin(true);
  CH3.begin(true);
  CH4.begin(true);
  CH5.begin(true);   
  
  //RC_RADIO ATTACH --OUTSETUP
  for(int i=0; i<chanel_number; i++){
    ppm[i]= default_servo_value;
  }

  ppm[2] = 1000;

  
  //PINMODE
  pinMode(A0,INPUT);

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

void loop_(){
  DATA_SEND();
}


void loop() {

  if(Serial.available()){
    while(Serial.available()){ 
      Serial.print(char(Serial.read()));
    }
    Serial.println();
  }
  
  RC_READING();
  DATA_SEND();

  //FUNCTION CHOOSE --AMCHSE
  if(MODE_1_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_1_PWM + 50 ){
    MODE_CODE = 1;
    MANUAL();
  }else if (MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50){
    MODE_CODE = 2;
    AUTO();
  }else{
    MODE_CODE = 7;
  }
  
}



//MANUAL FUNCTION
void MANUAL(){

  RC_READING();

  //RC_1_AVERGE_FILTER
  for(int i = 0 ; i <= FILTER_NUM ; i++){
    RC_1_SUM_FILTER[i+1] = RC_1_SUM_FILTER[i];
  }
  RC_1_SUM_FILTER[0] = RC_1_PWM;
  for(int i = 0 ; i <= FILTER_NUM ; i++){
    RC_1_SUM += RC_1_SUM_FILTER[i]
  }
  int RC_1_OUTPUT = RC_1_SUM / FILTER_NUM;
  
  //RC_2_AVERGE_FILTER
  for(int i = 0 ; i <= FILTER_NUM ; i++){
    RC_2_SUM_FILTER[i+1] = RC_2_SUM_FILTER[i];
  }
  RC_2_SUM_FILTER[0] = RC_2_PWM;
  for(int i = 0 ; i <= FILTER_NUM ; i++){
    RC_2_SUM += RC_2_SUM_FILTER[i]
  }
  int RC_2_OUTPUT = RC_2_SUM / FILTER_NUM;

  //RC_3_AVERGE_FILTER
  for(int i = 0 ; i <= FILTER_NUM ; i++){
    RC_3_SUM_FILTER[i+1] = RC_3_SUM_FILTER[i];
  }
  RC_3_SUM_FILTER[0] = RC_3_PWM;
  for(int i = 0 ; i <= FILTER_NUM ; i++){
    RC_3_SUM += RC_3_SUM_FILTER[i]
  }
  int RC_3_OUTPUT = RC_3_SUM / FILTER_NUM;
  
  //RC_4_AVERGE_FILTER
  for(int i = 0 ; i <= FILTER_NUM ; i++){
    RC_4_SUM_FILTER[i+1] = RC_4_SUM_FILTER[i];
  }
  RC_4_SUM_FILTER[0] = RC_4_PWM;
  for(int i = 0 ; i <= FILTER_NUM ; i++){
    RC_4_SUM += RC_4_SUM_FILTER[i]
  }
  int RC_4_OUTPUT = RC_4_SUM / FILTER_NUM;
  
  //RC_5_AVERGE_FILTER
  for(int i = 0 ; i <= FILTER_NUM ; i++){
    RC_5_SUM_FILTER[i+1] = RC_5_SUM_FILTER[i];
  }
  RC_5_SUM_FILTER[0] = RC_5_PWM;
  for(int i = 0 ; i <= FILTER_NUM ; i++){
    RC_5_SUM += RC_5_SUM_FILTER[i]
  }
  int RC_5_OUTPUT = RC_5_SUM / FILTER_NUM;
  
  ppm[0] = RC_1_OUTPUT;
  ppm[1] = RC_2_OUTPUT;
  ppm[2] = RC_3_OUTPUT;
  ppm[3] = RC_4_OUTPUT;
  ppm[4] = 970;
  
}


//AUTO FUNCTION
void AUTO(){
  
  if(debug){
    Serial.println(F("=====AUTO_LOG====="));
  }
  
  //TAKE OFF --TAKEOFF
  TAKE_OFF_CHECK = 0;
  while(1){
    //FOR BREAKING
    RC_READING();
    if((MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50) and TAKE_OFF_CHECK == 0){
      TAKE_OFF();
    }else{
      break;
    }
  }
 
}


//DRONE BASIC FUNCTION
void TAKE_OFF(){
  //ARM
  if(TAKE_OFF_CHECK == 0){
    timer = millis();
  }
  while(1){
    //FOR BREAKING
    RC_READING();
    if(!(MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50)){
      break;
    }    
    ppm[0] = 1400 ;
    ppm[1] = 1400 ;
    ppm[2] = 1000 ;
    ppm[3] = 1790 ;
    ppm[4] = 1400 ;
    DATA_SEND();      
    if(millis() - timer > 6000){
      break; 
    }
  }
  //READY TO TAKE OFF
  if(TAKE_OFF_CHECK == 0){
    timer = millis();
  }
  while(1){
    RC_READING();
    if(!(MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50)){
      break;
    }

    ppm[0] = 1400;
    ppm[1] = 1400;
    ppm[2] = 1000;
    ppm[3] = 1400;
    ppm[4] = 1400;
    DATA_SEND();    
    if (millis() - timer > 3000) {
      break;
    }
  }
  ///TAKE_OFF --WAITADD
  if(TAKE_OFF_CHECK == 0){
    timer = millis();
  }
  while(1){
    //FOR BREAKING
    RC_READING();
    if(!(MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50)){
      break;
    }    
    ppm[0] = 1400 ;
    ppm[1] = 1400 ;
    ppm[2] = 1510 ;
    ppm[3] = 1400 ; //NEED ADD TAKE OFF STABLIZE (USE OPENMV H7)
    ppm[4] = 1400 ;
    DATA_SEND();    
    if(millis() - timer > 2500){
      TAKE_OFF_CHECK = 1;
      break; 
    }
  }
}
void THROW(){
  
}
void RC_READING(){
  
  RC_1_PWM = CH1.getValue();
  RC_2_PWM = CH2.getValue();
  RC_3_PWM = CH3.getValue();
  RC_4_PWM = CH4.getValue();
  RC_5_PWM = CH5.getValue();


  if(1/*debug*/){
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
void DATA_SEND(){

  if( millis() - RF24_DUTY_TIMER > RF24_DUTY_TIME ){
    
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


// ISR PROGRAM --ISRPRO
ISR(TIMER1_COMPA_vect){  //leave this alone
  static boolean state = true;
  
  TCNT1 = 0;
  
  if(state) {  //start pulse
    digitalWrite(sigPin, onState);
    OCR1A = PPM_PulseLen * 2;
    state = false;
  }
  else{  //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;
  
    digitalWrite(sigPin, !onState);
    state = true;

    if(cur_chan_numb >= chanel_number){
      cur_chan_numb = 0;
      calc_rest = calc_rest + PPM_PulseLen;// 
      OCR1A = (PPM_FrLen - calc_rest) * 2;
      calc_rest = 0;
    }
    else{
      OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) * 2;
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }     
  }
}
