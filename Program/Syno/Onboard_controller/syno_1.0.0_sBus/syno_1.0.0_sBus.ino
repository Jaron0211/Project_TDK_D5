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
#include "SBUS.h"
#include <I2C.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include <Adafruit_NeoPixel.h>

#include "global.h"
#include "INPUT_PIN.h"
#include "OUT_PIN.h"
#include "nRF24L01_fig.h"
#include "FUNCTION_fig.h"

#define CHAR_BUF 128

#define LED_PIN    6
#define LED_COUNT  1
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

SBUS x8r(Serial1);

bool failSafe;
bool lostFrame;

void setup() {

  //USING FOR DEBUGGING --DEBUG
  debug = 0;

  //LED SETUP
  strip.begin();
  strip.show();
  strip.setBrightness(50);

  //Wire SETUP
  I2c.begin();
  I2c.timeOut(0);
  I2c.setSpeed(1);

  if (debug) { //debug) {
    Serial.begin(115200);
  }

  //INPUT SET UP --INSETU
  pinMode(A0, INPUT);
  pinMode(2, INPUT);
  pinMode(13, INPUT);


  //RC_RADIO ATTACH --OUTSETUP
  for (int i = 0; i < chanel_number; i++) {
    ppm[i] = default_servo_value;
  }

  ppm[2] = 1000;
  ppm[4] = Land;

  //OPENMV TIMER
  // openmv_time = millis();

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
  THROW(180);

  delay(100);
  strip.setPixelColor(0, 0,   50,   0) ;
  strip.show();
  delay(300);
  strip.setPixelColor(0, 0,   0,   0) ;
  strip.show();
  delay(100);
  strip.setPixelColor(0, 0,   255,   0) ;
  strip.show();
  delay(500);
  strip.setPixelColor(0, 0, 0, 0) ;
  strip.show();
  for (int i = 0; i < 2000; i++) {
    THROW(180);
  }
  delay(1500);


  //SBUS SETUP
  x8r.begin();

  //ISR SETUP --ISRSETUP
  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)


  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;

  OCR1A = 1;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);// 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();

  delay(100);

  strip.setPixelColor(0, 255, 255, 255) ;
  strip.show();
  delay(200);
  strip.setPixelColor(0, 0, 0, 0) ;
  strip.show();

  //SANDBAG SETUP
  RC_TIMER = millis();
}

void loop_3() {

  THROW(0);

}

void loop_1() {
  function_tester();

  Serial.print(F("PPM: "));
  for (int i = 0; i <= 4; i++) {
    Serial.print(ppm[i]);
    Serial.print(" ");
  }
  Serial.println();
  DATA_SEND();
}

void function_tester() {
  land();
}

void loop() {

  RC_READING();
  DATA_SEND();

  //FUNCTION CHOOSE --AMCHSE
  if (!failSafe) {
    if ((MODE_1_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_1_PWM + 50 )) {
      strip.setPixelColor(0, 0, 255, 0) ;
      MODE_CODE = 1;
      MANUAL();
      RESET();
    } else if (not(!(MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50) and !FAILSAFE)) {
      MODE_CODE = 2;
      AUTO_2();
    } else if (MODE_3_PWM - 50 < RC_5_PWM and MODE_3_PWM + 50 > RC_5_PWM) {
      strip.setPixelColor(0, 0, 0, 255) ;
      prepare();
      MODE_CODE = 7;
    }
  } else {
    strip.setPixelColor(0, 50, 0, 120) ;
  }

  strip.show();

  if (debug) {
    Serial.print(F("FAILSAFE: "));
    Serial.println(FAILSAFE);
    Serial.print(F("boolean check: "));
    Serial.println((not(!(MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50) and !FAILSAFE)));
    Serial.print(F("PPM: "));
    for (int i = 0; i <= 4; i++) {
      Serial.print(ppm[i]);
      Serial.print(" ");
    }
    Serial.println();

  }

}


//MANUAL FUNCTION
void MANUAL() {

  ppm[0] = RC_1_PWM;
  ppm[1] = RC_2_PWM;
  ppm[2] = RC_3_PWM;
  ppm[3] = RC_4_PWM;
  ppm[4] = Stabilize;

  if (0/*debug*/) {
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
void prepare() {

  ppm[0] = 1500;
  ppm[1] = 1500;
  ppm[2] = 1060;
  ppm[3] = 1500;
  ppm[4] = Stabilize;

}

//AUTO FUNCTION
void AUTO_2() {

  //START THE DUTY TIMER --AUTOTIMEER
  if (debug) {
    Serial.println(F("=====AUTO_LOG====="));
  }

  //TIMER RESET
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

  if (debug) {
    Serial.print(F("DUTY_TIMER: "));
    Serial.print(DUTY_MIN);
    Serial.print(F(" ; "));
    Serial.println(DUTY_SEC);
  }

  RC_READING();
  if (FAILSAFE == 0) {
    //strip.setPixelColor(0, 255, 255, 255) ;
    if      (LEVEL_SWITCH == 0 & L1_CHECKER == 0) {
      L1();
    } else if (LEVEL_SWITCH == 1 & L2_CHECKER == 0) {
      land();// --steptest
      //L2();
    } else if (LEVEL_SWITCH == 2 & L3_CHECKER == 0) {
      //L3();
    } else if (LEVEL_SWITCH == 3 & L4_CHECKER == 0) {
      //L4();
    } else if (LEVEL_SWITCH == 4 & L5_CHECKER == 0) {
      //L5();
    } else if (LEVEL_SWITCH == 5 & L6_CHECKER == 0) {
      //L6();
    } else if (LEVEL_SWITCH == 6 & L7_CHECKER == 0) {
      //L7();
    } else if (LEVEL_SWITCH == 7) {
      //STOP();
    } else {
      //error();
    }

    if (!(MIN_PWM < RC_1_PWM and RC_1_PWM < MAX_PWM) or !(MIN_PWM < RC_2_PWM and RC_2_PWM < MAX_PWM) or !(MIN_PWM < RC_3_PWM and RC_3_PWM < MAX_PWM) or !(MIN_PWM < RC_4_PWM and RC_4_PWM < MAX_PWM)) {
      failsafe_counter += 1;
    } else {
      failsafe_counter = 0;
    }
    if (failsafe_counter > 2) {
      Serial.println(F("FAILSAFE CHECK"));
      FAILSAFE = 1;
    }


  } else {
    strip.setPixelColor(0, 255, 0, 0) ;
    MANUAL();
  }

}

void RESET() {

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
  L3_start = 0;
  L3_duty = B00000000;

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
  failsafe_counter = 0;

  X = 0;
  Y = 0;
  Z = 0;

  PITCH_BIAS = 0;
  ROLL_BIAS = 0;
  THROTTLE_BIAS = 0;
  YAW_BIAS = 0;

  OPENMV_DATA[0] = 0;
  OPENMV_DATA[1] = 0;
  OPENMV_DATA[2] = 0;
  OPENMV_DATA[3] = 0;

  MISSION_COLOR = "0";

  //openmv_time = millis();

}
//DRONE BASIC FUNCTION

void THROW(int angle) {
  int PWM = map(angle, 0, 180, 1000, 2500);
  PWM = constrain(PWM, 1000, 2500);
  if (micros() - SERVO_TIMER <= PWM ) {
    digitalWrite(SANDBAG, PWM_STATE);
  } else if (micros() - SERVO_TIMER > PWM and micros() - SERVO_TIMER <= 20000) {
    digitalWrite(SANDBAG, !PWM_STATE);
  } else if (micros() - SERVO_TIMER >= 20000) {
    SERVO_TIMER = micros();
  }
}
void RC_READING() {

  int channels[16];
  if (x8r.read(&channels[0], &failSafe, &lostFrame)) {
    if(!lostFrame){
      RC_1_PWM = 5 * channels[0] / 8 + 880;
      RC_2_PWM = 5 * channels[1] / 8 + 880;
      RC_3_PWM = 5 * channels[2] / 8 + 880;
      RC_4_PWM = 5 * channels[3] / 8 + 880;
      RC_5_PWM = 5 * channels[4] / 8 + 880;
    }
  }

  if (debug) {
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
    //SEND_DATA.CH_OUTPUT[0]  =  ppm[0];
    //SEND_DATA.CH_OUTPUT[1]  =  ppm[1];
    //SEND_DATA.CH_OUTPUT[2]  =  ppm[2];
    //SEND_DATA.CH_OUTPUT[3]  =  ppm[3];
    //SEND_DATA.VOLTAGE       = analogRead(A0);
    //SEND_DATA.OPENMV_STATUS = 1101;
    //SEND_DATA.MODE          = MODE_CODE;
    SEND_DATA.STATUS_CODE   = LEVEL_CODE ;

    radio.write(&SEND_DATA, sizeof(SEND_DATA));

    RF24_DUTY_TIMER = millis();

  }

}

//OPENMV FUNCTION
void OPENMV_I2C_GET(int MODE) {
  if (millis() - openmv_time >= 100) {
    int32_t temp = 0;
    char buff[CHAR_BUF] = {0};
    char C[2];
    String convert;

    convert = String(MODE);
    convert.toCharArray(C, 2);

    I2c.write(0x12, MODE);
    I2c.read(0x12, 2);
    if (I2c.available() == 2) { // got length?
      Serial.println(F("get length"));
      Serial.println(temp);
      temp = I2c.receive() | (I2c.receive() << 8);
      //delay(1); // Give some setup time...

      I2c.read(0x12, temp);
      if (I2c.available() == temp) { // got full message?
        Serial.println(F("get all"));
        temp = 0;
        while (I2c.available()) buff[temp++] = I2c.receive(); //Serial.println(buff);

      } else {
        while (I2c.available()) I2c.receive(); // Toss garbage bytes.
      }
    }

    String c = buff;
    Serial.println(c);
    int first_split = c.indexOf(';');
    OPENMV_DATA[0] = c.substring(0, first_split).toInt();

    int second_spilt = c.indexOf(';', first_split + 1);
    OPENMV_DATA[1] = c.substring(first_split + 1, second_spilt).toFloat();

    int third_spilt = c.indexOf(';', second_spilt + 1);
    OPENMV_DATA[2] = c.substring(second_spilt + 1, third_spilt).toFloat();

    OPENMV_DATA[3] = c.substring(third_spilt + 1, int(c.length())).toFloat();

  }
}

void HEIGHT() {

}

//COMBINE FUNCTON
void LINE_FOLLOW() {
  //OPENMV_GET(2);
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
    if (debug) {
      Serial.println("===============L1_LOG===============");
      Serial.println(F("take_off_start!"));
    }
  }

  if (take_off_start) {
    if (debug) {
      Serial.println(take_off_duty);
    }
    //PREPARE
    if (take_off_duty == B00000000) {
      if (debug) {
        Serial.println(F("preparing"));
      }
      strip.setPixelColor(0, 0, 100, 0) ;
      ppm[0] = 1500 ;
      ppm[1] = 1500 ;
      ppm[2] = 1000 ;
      ppm[3] = 1500 ;
      ppm[4] = Stabilize ;
      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 8) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00000001;
      }
    }
    //ARM
    else if (take_off_duty == B00000001) {
      //TAKE_OFF
      if (debug) {
        Serial.println(F("ARM"));
      }
      strip.setPixelColor(0, 100, 0, 0) ;
      ppm[0] = 1500 ;
      ppm[1] = 1500 ;
      ppm[2] = 1000 ;
      ppm[3] = 2000 ;
      ppm[4] = Stabilize ;
      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 6) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00000011;
      }
    }

    //PREPARE
    else if (take_off_duty == B00000011) {
      //NEED DETECT HEIGHT，OpenMV
      if (debug) {
        Serial.println(F("take_off_ready"));
      }
      OPENMV_I2C_GET(1);

      strip.setPixelColor(0, 100, 100, 0) ;
      ppm[0] = 1500;
      ppm[1] = 1500;
      ppm[2] = 1400;
      ppm[3] = 1500;
      ppm[4] = Stabilize;
      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 4) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00000111;
      }
    }
    //TAKE OFF
    else if (take_off_duty == B00000111) {
      //NEED DETECT HEIGHT，OpenMV
      if (debug) {
        Serial.println(F("TAKE_OFF"));
      }

      OPENMV_I2C_GET(1);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);
      strip.setPixelColor(0, 0, 100, 0) ;
      if (abs(ROLL_BIAS) > 35) {
        ppm[0] = 1500 - REVISE_VALUE  * ROLL_BIAS / 80;
      } else {
        ppm[0] = 1500;
      }

      if (abs(PITCH_BIAS) > 25) {
        ppm[1] = 1500 + REVICE_VALUE  * PITCH_BIAS / 60;
      } else {
        ppm[1] = 1500 ;
      }

      ppm[2] = 1700 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);

      if (abs(YAW_BIAS) > 15) {
        ppm[3] = 1500 - ANGLE_VALUE * YAW_BIAS / 90;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = Stabilize;

      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 3) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00001111;
      }
    }
    //STAY IN A WHILE
    else if (take_off_duty == B00001111) {
      //NEED DETECT HEIGHT，OpenMV
      if (debug) {
        Serial.println(F("STABLELIZING"));
      }
      OPENMV_I2C_GET(1);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      strip.setPixelColor(0, 0, 0, 100) ;
      if (abs(ROLL_BIAS) > 35) {
        ppm[0] = 1500 - REVISE_VALUE  * ROLL_BIAS / 80;
      } else {
        ppm[0] = 1500;
      }

      if (abs(PITCH_BIAS) > 25) {
        ppm[1] = 1500 + REVICE_VALUE  * PITCH_BIAS / 60;
      } else {
        ppm[1] = 1500 ;
      }

      ppm[2] = 1500 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);

      if (abs(YAW_BIAS) > 15) {
        ppm[3] = 1500 - ANGLE_VALUE * YAW_BIAS / 90;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = FlowHold;

      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 10) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00011111;
      }
    }
    else if (take_off_duty == B00011111) {
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
    //FOLLOW LINE TO RGB LEVEL
    if (L2_duty == B00000000) {
      if (debug) {
        Serial.println("FINDING RGB");
      }

      OPENMV_I2C_GET(2);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (OPENMV_DATA[0] == 1) {
        ppm[0] = 1500 - REVISE_VALUE * PITCH_BIAS / 80;
      } else {
        ppm[0] = 1600 - REVISE_VALUE * PITCH_BIAS / 80;
      }
      ppm[1] = 1500 + REVICE_VALUE * ROLL_BIAS / 60;
      ppm[2] = 1500 ;
      ppm[3] = 1500 + ANGLE_VALUE * YAW_BIAS / 90;
      ppm[4] = FlowHold;

      if (OPENMV_DATA[0] == 1) {
        strip.setPixelColor(0, 255, 0, 0) ;
      } else {
        strip.setPixelColor(0, 0, 0, 0) ;
      }
      strip.show();

      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 8) //DUTY_TIME MAX TO 8s
        or  (OPENMV_DATA[0] == 1 and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 4)) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L2_TIME[0] = DUTY_MIN;
        L2_TIME[1] = DUTY_SEC;
        L2_duty = B00000001;
      }
    }
    //BREAKING & WAITING
    else if (L2_duty == B00000001) {
      if (debug) {
        Serial.println("BREAKING AND WAIT");
      }
      OPENMV_I2C_GET(3);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      ppm[0] = 1500 + REVISE_VALUE * (PITCH_BIAS / 80);
      ppm[1] = 1500 - REVICE_VALUE * (ROLL_BIAS / 60);
      ppm[2] = 1500 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);
      ppm[3] = 1500 + ANGLE_VALUE * (1 - YAW_BIAS / 90);
      ppm[4] = FlowHold;

      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 20) //DUTY_TIME MAX TO 20s
        or
        ((OPENMV_DATA[0] == 2 or OPENMV_DATA[0] == 3) and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 4)) //DETECT COLOR ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L2_TIME[0] = DUTY_MIN;
        L2_TIME[1] = DUTY_SEC;

        if (OPENMV_DATA[0] == 2) {
          L2_duty = B00000011;
          MISSION_COLOR = "b";
        } else if (OPENMV_DATA[0] == 3) {
          L2_duty = B00000101;
          MISSION_COLOR = "g";
        } else {
          L2_duty = B00000011;
        }
      }
    }
    //SPIN_BLUE
    else if (L2_duty == B00000011) {

      if (debug) {
        Serial.println("BLUE DETECT");
      }

      OPENMV_I2C_GET(3);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      ppm[0] = 1500 + REVISE_VALUE * (PITCH_BIAS / 80);
      ppm[1] = 1500 - REVICE_VALUE * (ROLL_BIAS / 60);
      ppm[2] = 1500 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);
      ppm[3] = 1600 ;//+ ANGLE_VALUE * (1 - YAW_BIAS/90);
      ppm[4] = FlowHold;

      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 4) {
        L2_TIME[0] = DUTY_MIN;
        L2_TIME[1] = DUTY_SEC;
        L2_duty = B00001111;
      }
    }
    //SPIN_GREEN
    else if (L2_duty == B00000101) {
      if (debug) {
        Serial.println("GREEN DETECT");
      }
      OPENMV_I2C_GET(3);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      ppm[0] = 1500 + REVISE_VALUE * (PITCH_BIAS / 80);
      ppm[1] = 1500 - REVICE_VALUE * (ROLL_BIAS / 60);
      ppm[2] = 1500 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);
      ppm[3] = 1400 ;//+ ANGLE_VALUE * (1 - YAW_BIAS/90);
      ppm[4] = FlowHold;

      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 4) {
        L2_TIME[0] = DUTY_MIN;
        L2_TIME[1] = DUTY_SEC;
        L2_duty = B00001111;
      }
    }
    //MOVE A LITTLE BIT
    else if (L2_duty == B00001111) {
      if (debug) {
        Serial.println("LEAVE RGB");
      }
      OPENMV_I2C_GET(5);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      ppm[0] = 1600 + REVISE_VALUE * (PITCH_BIAS / 80);
      ppm[1] = 1500 - REVICE_VALUE * (ROLL_BIAS / 60);
      ppm[2] = 1500 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);
      ppm[3] = 1500 + ANGLE_VALUE * (1 - YAW_BIAS / 90);
      ppm[4] = FlowHold;

      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 3) {
        L2_TIME[0] = DUTY_MIN;
        L2_TIME[1] = DUTY_SEC;
        L2_duty = B00001111;
      }
    }
    //OVER
    else if (L2_duty == B00011111) {
      LEVEL_SWITCH = 2;
      L2_CHECKER = 1;
    }
  }
}

void L3() {
  if (!L3_start) {
    L3_TIME[0] = DUTY_MIN;
    L3_TIME[1] = DUTY_SEC;
    L3_duty = B00000000;
    L3_start = 1;
  }
  if (L3_start) {
    //FOLLOW LINE AND FIND TANGENT POINT
    if (L3_duty == B00000000) {
      if (debug) {
        Serial.println("FINDING TANGENT POINT");
      }

      OPENMV_I2C_GET(6);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (OPENMV_DATA[0] == 5 ) {
        ppm[0] = 1500 + REVISE_VALUE * (PITCH_BIAS / 80);
      } else {
        ppm[0] = 1600 + REVISE_VALUE * (PITCH_BIAS / 80);
      }
      ppm[1] = 1500 - REVICE_VALUE * (ROLL_BIAS / 60);
      ppm[2] = 1500 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);
      ppm[3] = 1500 + ANGLE_VALUE * (1 - YAW_BIAS / 90);
      ppm[4] = FlowHold;

      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 6) //DUTY_TIME MAX TO 8s
        or  (OPENMV_DATA[0] == 5 and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 4)) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B00000001;
      }
    }
    //TANGENT POINT TRUNING
    else if (L3_duty == B00000001) {
      if (debug) {
        Serial.println(F("TURNING"));
      }
      OPENMV_I2C_GET(6);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      ppm[0] = 1500 + REVISE_VALUE * (PITCH_BIAS / 80);
      ppm[1] = 1500 - REVICE_VALUE * (ROLL_BIAS / 60);
      ppm[2] = 1500 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);

      if (MISSION_COLOR == "b") {
        ppm[3] = 1400 ;//+ ANGLE_VALUE * (1 - YAW_BIAS/90);
      } else if (MISSION_COLOR == "g") {
        ppm[3] = 1600 ;//+ ANGLE_VALUE * (1 - YAW_BIAS/90);
      }

      ppm[4] = FlowHold;

      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 3) //DUTY_TIME MAX TO 8s
      ) {
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B00000011;
      }
    }
    //MOVE FORWORD
    else if (L3_duty == B00000011) {
      if (debug) {
        Serial.println(F("LEAVE TANGENT POINT"));
      }

      OPENMV_I2C_GET(5);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      ppm[0] = 1600 ;//+ REVISE_VALUE * (PITCH_BIAS/80);
      ppm[1] = 1500 - REVICE_VALUE * (ROLL_BIAS / 60);
      ppm[2] = 1500 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);
      ppm[3] = 1500 ;//+ ANGLE_VALUE * (1 - YAW_BIAS/90);
      ppm[4] = FlowHold;

      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 4) //DUTY_TIME MAX TO 8s
        or  (OPENMV_DATA[0] == 0 and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 2)) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B00000111;
      }
    }
    //FIND SANDBAG AREA
    else if (L3_duty == B00000111) {
      if (debug) {
        Serial.println(F("FIND SANDBAG AREA"));
      }

      OPENMV_I2C_GET(7);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      ppm[0] = 1600 ;//+ REVISE_VALUE * (PITCH_BIAS/80);
      ppm[1] = 1500 - REVICE_VALUE * (ROLL_BIAS / 60);
      ppm[2] = 1500 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);
      ppm[3] = 1500 ;//+ ANGLE_VALUE * (1 - YAW_BIAS/90);
      ppm[4] = FlowHold;

      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 4) //DUTY_TIME MAX TO 8s
        or  (OPENMV_DATA[0] == 0 and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 2)) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B00000111;
      }
    }
  }
}

void land() {
  
  if( millis() - LED_TIMER < 100 ){
    strip.setPixelColor(0, 200,   200,   200) ;
  }else if( (millis() - LED_TIMER >= 100) and (millis() - LED_TIMER < 200) ){
    strip.setPixelColor(0, 0,   0,   0) ;
  }else{
    LED_TIMER  = millis() ;
  }
  
  ppm[4] = Land;
  ppm[0] = 1500;
  ppm[1] = 1500;
  ppm[2] = 1010;//+ (analogRead(SONAR) - IDEAL_HEIGHT);
  ppm[3] = 1500;

  strip.show();
  
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
