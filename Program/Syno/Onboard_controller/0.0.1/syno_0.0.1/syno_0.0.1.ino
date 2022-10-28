/*
 * --WAITADD : NEED TO ADJUST FUNCTION DETIAL
 * --DEBUG : DEBUG OPTION
 * --INSETUP : INPUT SETUP TAG
 * --OUTSETUP : OUTPUT SETUP TAG 
 * --RFSETUP : nRF24L01 SETUP TAG
 * --AMCHSE : AUTO/MANUAL MODE CHOOSE TAG
 * --TAKEOFF : TAKE OFF FUNCTION TAG
 */

#include <Wire.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include <SPI.h>
#include <Servo.h>

#include "global.h"
#include "INPUT_PIN.h"
#include "OUT_PIN.h"
#include "nRF24L01_fig.h"
#include "FUNCTION_fig.h"

void setup() {
  
  Wire.begin();
  Serial.begin(2000000);
  
  //USING FOR DEBUGGING --DEBUG
  //debug = 1;
  
  //INPUT SET UP --INSETUP
  pinMode(RC_1,INPUT);
  pinMode(RC_2,INPUT);
  pinMode(RC_3,INPUT);
  pinMode(RC_4,INPUT);
  pinMode(RC_5,INPUT);

  //RC_RADIO ATTACH --OUTSETUP
  CH1.attach(ch1);
  CH2.attach(ch2);
  CH3.attach(ch3);
  CH4.attach(ch4);
  CH5.attach(ch5);
  CH6.attach(ch6);

  CH1.writeMicroseconds(900);
  CH2.writeMicroseconds(900);
  CH3.writeMicroseconds(900);
  CH4.writeMicroseconds(900);
  CH5.writeMicroseconds(900);
  CH6.writeMicroseconds(900);

  //nRF24L01 SET UP --RFSETUP
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(1);
  radio.setDataRate(RF24_2MBPS);//MAX LENGTH IS 0.25MB/S
  radio.setCRCLength(RF24_CRC_8);
  radio.openWritingPipe(pipe[1]);
  radio.setAutoAck(0);
  radio.stopListening();

}

void loop() {
  
  DATA_SEND();
  RC_READING();

  //FUNCTION CHOOSE --AMCHSE
  if(MODE_1_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_1_PWM + 50 ){
    MANUAL();
  }else if (MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50){
    AUTO();
  }
  
}

//MANUAL FUNCTION
void MANUAL(){
  
  CH1.writeMicroseconds(RC_1_PWM);
  CH2.writeMicroseconds(RC_2_PWM);
  CH3.writeMicroseconds(RC_3_PWM);
  CH4.writeMicroseconds(RC_4_PWM);
  CH5.writeMicroseconds(RC_5_PWM);
}


//AUTO FUNCTION
void AUTO(){
  
  if(debug){
    Serial.print(F("=====AUTO_LOG====="));
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
    CH1.writeMicroseconds(1400);
    CH2.writeMicroseconds(1400);
    CH3.writeMicroseconds(1000);
    CH4.writeMicroseconds(1790);
    CH5.writeMicroseconds(1400);
    
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

    CH1.writeMicroseconds(1400);
    CH2.writeMicroseconds(1400);
    CH3.writeMicroseconds(1000);
    CH4.writeMicroseconds(1400);
    CH5.writeMicroseconds(1400);
    
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
    CH1.writeMicroseconds(1400);
    CH2.writeMicroseconds(1400);
    CH3.writeMicroseconds(1510);
    CH4.writeMicroseconds(1400); //NEED ADD TAKE OFF STABLIZE (USE OPENMV H7)
    CH5.writeMicroseconds(1400);
    
    if(millis() - timer > 2500){
      TAKE_OFF_CHECK = 1;
      break; 
    }
  }
}
void THROW(){

}
void RC_READING(){
  RC_1_PWM = pulseIn(RC_1,1);
  RC_2_PWM = pulseIn(RC_2,1);
  RC_3_PWM = pulseIn(RC_3,1);
  RC_4_PWM = pulseIn(RC_4,1);
  RC_5_PWM = pulseIn(RC_5,1);

  if(debug){
    Serial.println(F("=====RC_INPUT====="));
    Serial.print(F("RC_1_PWM: "));
    Serial.println(RC_1_PWM);
    Serial.print(F("RC_2_PWM: "));
    Serial.println(RC_2_PWM);
    Serial.print(F("RC_3_PWM: "));
    Serial.println(RC_3_PWM);
    Serial.print(F("RC_4_PWM: "));
    Serial.println(RC_4_PWM);
    Serial.print(F("RC_5_PWM: "));
    Serial.println(RC_5_PWM);
  }
}
void DATA_SEND(){ 
  radio.write(&send_data, sizeof(send_data));
  radio.stopListening();
}
