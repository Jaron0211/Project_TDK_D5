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
   --steptest : PART TEST TAG

   --AUTOTIMEER : AUTO MODE TIME COUNTER
*/
#include "SBUS.h"
#include <I2C.h>
#include <Adafruit_NeoPixel.h>

#include "global.h"
#include "INPUT_PIN.h"
#include "OUT_PIN.h"
#include "FUNCTION_fig.h"

#define CHAR_BUF 128

#define LED_PIN    6
#define LED_COUNT  1
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

SBUS R2008SB(Serial1);

void setup() {

  //LED SETUP
  strip.begin();
  strip.show();
  strip.setBrightness(50);

  //Wire SETUP
  I2c.begin();
  I2c.timeOut(500);
  I2c.setSpeed(1);


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
  openmv_time = millis();

  //MODE_CODE
  MODE_CODE = 0; //SHOW AS NOT SETUP


  for (int i = 0; i < 2000; i++) {
    THROW(180);
  }
  delay(1500);

  //SBUS SETUP
  R2008SB.begin();

  //ISR SETUP --ISRSETUP
  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)


  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;

  OCR1A = 100;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);// 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();

  strip.setPixelColor(0, 0,   50,   0) ;
  strip.show();
  delay(300);
  strip.setPixelColor(0, 0,   255,   0) ;
  strip.show();
  delay(500);
  strip.setPixelColor(0, 0, 0, 0) ;
  strip.show();

  //SANDBAG SETUP
  RC_TIMER = millis();
}

//function test
void loop_t() {
  delay(100);
  function_tester();
  for (int i = 0; i <= 4; i++) {
    Serial.print(ppm[i]);
    Serial.print(" ");
  }
  Serial.println();
  strip.show();
}
void function_tester() {
  RESET();
  MISSION_COLOR = "g";
  L4_duty = B00011111 ;
  L4_start = 1;
  L4();
}

//loop
void loop() {

  RC_READING();
  MODE_CODE = 7;

  //FUNCTION CHOOSE --AMCHSE
  if (!failSafe) {
    if ((MODE_1_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_1_PWM + 50 )) {
      strip.setPixelColor(0, 0, 255, 0) ;
      MODE_CODE = 1;
      MANUAL();
      RESET();
    } else if (MODE_3_PWM - 50 < RC_5_PWM and MODE_3_PWM + 50 > RC_5_PWM) {
      strip.setPixelColor(0, 0, 0, 255) ;
      LOITER();
      RESET();
      MODE_CODE = 3;
    } else if (not(!(MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50) and !FAILSAFE)) {
      MODE_CODE = 2;
      AUTO_2();
    }
  } else {
    MODE_CODE = 1;
    strip.setPixelColor(0, 50, 0, 120) ;
    land();
  }

  strip.show();
}

//Syno FUNCTION
void AUTO_2() {

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

  RC_READING();
  if (FAILSAFE == 0) {
    if      (LEVEL_SWITCH == 0 & L1_CHECKER == 0) {
      L1();
    } else if (LEVEL_SWITCH == 1 & L2_CHECKER == 0) {
      L2();
    } else if (LEVEL_SWITCH == 2 & L3_CHECKER == 0) {
      L3();
    } else if (LEVEL_SWITCH == 3 & L4_CHECKER == 0) {
      land();
      //L4();
    } else if (LEVEL_SWITCH == 4 & L5_CHECKER == 0) {
      //land();
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
      FAILSAFE = 1;
    }


  } else {
    strip.setPixelColor(0, 255, 0, 0) ;
    LOITER();
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
  L4_start = 0;
  L4_duty = B00000000;

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
  YAW_BIAS = 90;

  OPENMV_DATA[0] = 0;
  OPENMV_DATA[1] = 0;
  OPENMV_DATA[2] = 0;
  OPENMV_DATA[3] = 0;

  MISSION_COLOR = "0";
  HAS_THROW = 0;

}

void RC_READING() {

  delay(5);
  float percent = 0.2;
  int channels[8];
  if (R2008SB.read(&channels[0], &failSafe, &lostFrame)) {
    /*
      RC_1_PWM = RC_1_PWM * (1 - percent) + (0.7 * channels[0] + 795) * (percent) ;
      RC_2_PWM = RC_2_PWM * (1 - percent) + (0.7 * channels[1] + 795) * (percent) ;
      RC_3_PWM = RC_3_PWM * (1 - percent) + (0.7 * channels[2] + 795) * (percent) ;
      RC_4_PWM = RC_4_PWM * (1 - percent) + (0.7 * channels[3] + 795) * (percent) ;
      RC_5_PWM = RC_5_PWM * (1 - percent) + (0.7 * channels[4] + 795) * (percent) ;
    */

    RC_1_PWM = map((0.7 * channels[0] + 795), 1051, 1971, 1000, 2000);
    RC_2_PWM = map((0.7 * channels[1] + 795), 1051, 1971, 1000, 2000);
    RC_3_PWM = map((0.7 * channels[2] + 795), 1051, 1971, 1000, 2000);
    RC_4_PWM = map((0.7 * channels[3] + 795), 1051, 1971, 1000, 2000);
    RC_5_PWM = (0.7 * channels[4] + 795);
  }
}

//DRONE BASIC FUNCTION
void LOITER() {
  ppm[0] = RC_1_PWM;
  ppm[1] = RC_2_PWM;
  ppm[2] = RC_3_PWM;
  ppm[3] = RC_4_PWM;
  ppm[4] = loiter;
}

void MANUAL() {

  ppm[0] = RC_1_PWM;
  ppm[1] = RC_2_PWM;
  ppm[2] = RC_3_PWM;
  ppm[3] = RC_4_PWM;
  ppm[4] = Stabilize;

}

void land() {

  if ( millis() - LED_TIMER < 100 ) {
    strip.setPixelColor(0, 200,   200,   200) ;
  } else if ( (millis() - LED_TIMER >= 100) and (millis() - LED_TIMER < 200) ) {
    strip.setPixelColor(0, 0,   0,   0) ;
  } else {
    LED_TIMER  = millis() ;
  }

  ppm[4] = Land;
  ppm[0] = 1500;
  ppm[1] = 1500;
  ppm[2] = 1010;//+ (analogRead(SONAR) - IDEAL_HEIGHT);
  ppm[3] = 1500;

  strip.show();

}

void THROW(int angle) {

  int PWM = map(angle, 0, 180, 1100, 2400);
  PWM = constrain(PWM, 1100, 2400);
  if (micros() - SERVO_TIMER <= PWM ) {
    digitalWrite(SANDBAG, PWM_STATE);
  } else if (micros() - SERVO_TIMER > PWM and micros() - SERVO_TIMER <= 20000) {
    digitalWrite(SANDBAG, !PWM_STATE);
  } else if (micros() - SERVO_TIMER >= 20000) {
    SERVO_TIMER = micros();
  }
}

//SENSOR FUNCTION
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

      temp = I2c.receive() | (I2c.receive() << 8);
      delay(1);

      I2c.read(0x12, temp);
      if (I2c.available() == temp) {
        temp = 0;
        while (I2c.available()) buff[temp++] = I2c.receive();
      } else {
        while (I2c.available()) I2c.receive(); // Toss garbage bytes.
      }
    }

    String c = buff;
    int first_split = c.indexOf(';');
    OPENMV_DATA[0] = c.substring(0, first_split).toInt();

    int second_spilt = c.indexOf(';', first_split + 1);
    OPENMV_DATA[1] = c.substring(first_split + 1, second_spilt).toFloat();

    int third_spilt = c.indexOf(';', second_spilt + 1);
    OPENMV_DATA[2] = c.substring(second_spilt + 1, third_spilt).toFloat();

    OPENMV_DATA[3] = c.substring(third_spilt + 1, int(c.length())).toFloat();
    openmv_time = millis();
  }
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
  }

  if (take_off_start) {
    //PREPARE
    if (take_off_duty == B00000000) {

      strip.setPixelColor(0, 100, 100, 0) ;
      ppm[0] = 1500 ;
      ppm[1] = 1500 ;
      ppm[2] = 1000 ;
      ppm[3] = 1500 ;
      ppm[4] = loiter ;

      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 7) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00000001;
      }
    }
    //ARM
    else if (take_off_duty == B00000001) {
      //TAKE_OFF

      strip.setPixelColor(0, 100, 0, 100) ;
      ppm[0] = 1500 ;
      ppm[1] = 1500 ;
      ppm[2] = 1000 ;
      ppm[3] = 2000 ;
      ppm[4] = loiter ;

      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 4) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00000011;
      }
    }
    //PREPARE
    else if (take_off_duty == B00000011) {
      //NEED DETECT HEIGHT，OpenMV

      OPENMV_I2C_GET(1);

      strip.setPixelColor(0, 0, 0, 100) ;
      ppm[0] = 1500;
      ppm[1] = 1500;
      ppm[2] = 1300;
      ppm[3] = 1500;
      ppm[4] = loiter;

      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 3) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00000111;
      }
    }
    else if (take_off_duty == B00000111) {
      //NEED DETECT HEIGHT，OpenMV

      OPENMV_I2C_GET(1);

      strip.setPixelColor(0, 100, 100, 100) ;
      ppm[0] = 1500;
      ppm[1] = 1500;
      ppm[2] = 1500;
      ppm[3] = 1500;
      ppm[4] = loiter;

      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 3) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00001111;
      }
    }
    //TAKE OFF
    else if (take_off_duty == B00001111) {
      //NEED DETECT HEIGHT，OpenMV

      if ( millis() - LED_TIMER < 100 ) {
        strip.setPixelColor(0, 255,   0,   0) ;
      } else if ( (millis() - LED_TIMER >= 100) and (millis() - LED_TIMER < 200) ) {
        strip.setPixelColor(0, 0,   0,   0) ;
      } else {
        LED_TIMER  = millis() ;
      }

      OPENMV_I2C_GET(1);

      if (ROLL_BIAS > 25) {
        ppm[0] = 1550 ;
      } else if (ROLL_BIAS < -25) {
        ppm[0] = 1450 ;
      } else {
        ppm[0] = 1500;
      }

      if (PITCH_BIAS > 20) {
        ppm[1] = 1550;
      } else if (PITCH_BIAS < -20) {
        ppm[1] = 1450;
      } else {
        ppm[1] = 1500 ;
      }

      ppm[2] = 1700 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1600 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1400;
      } else {
        ppm[3] = 1500;
      }
      ppm[4] = loiter;

      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 6) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00011111;
      }
    }
    //MODE CHANGE STABE
    else if (take_off_duty == B00011111) {

      OPENMV_I2C_GET(1);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      strip.setPixelColor(0, 0, 0, 100) ;
      if (ROLL_BIAS > 25) {
        ppm[0] = 1550 ;
      } else if (ROLL_BIAS < -25) {
        ppm[0] = 1450 ;
      } else {
        ppm[0] = 1500;
      }

      if (PITCH_BIAS > 20) {
        ppm[1] = 1550;
      } else if (PITCH_BIAS < -20) {
        ppm[1] = 1450;
      } else {
        ppm[1] = 1500 ;
      }

      ppm[2] = 1500 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 80) {
        ppm[3] = 1600 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 80) {
        ppm[3] = 1400;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;

      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) > 1) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B00111111;
      }
    }
    //STAY A WHILE
    else if (take_off_duty == B00111111) {
      //NEED DETECT HEIGHT，OpenMV

      OPENMV_I2C_GET(1);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      strip.setPixelColor(0, 0, 0, 100) ;
      if (ROLL_BIAS > 25) {
        ppm[0] = 1550 ;
      } else if (ROLL_BIAS < -25) {
        ppm[0] = 1450 ;
      } else {
        ppm[0] = 1500;
      }

      if (PITCH_BIAS > 20) {
        ppm[1] = 1550;
      } else if (PITCH_BIAS < -20) {
        ppm[1] = 1450;
      } else {
        ppm[1] = 1500 ;
      }

      ppm[2] = 1500 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 80) {
        ppm[3] = 1600 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 80) {
        ppm[3] = 1400;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;

      if (( DUTY_MIN * 60 + DUTY_SEC ) - ( TAKE_OFF_TIMER[0] * 60 + TAKE_OFF_TIMER[1] ) >= 8) {
        TAKE_OFF_TIMER[0] = DUTY_MIN;
        TAKE_OFF_TIMER[1] = DUTY_SEC;
        take_off_duty = B01111111;
      }
    }
    //OVER
    else if (take_off_duty == B01111111) {
      strip.setPixelColor(0, 0, 0, 0) ;
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
    color_detect_counter = millis();
  }

  if (L2_start) {
    //FOLLOW LINE TO RGB LEVEL
    if (L2_duty == B00000000) {

      OPENMV_I2C_GET(2);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 25) {
        ppm[0] = 1550 ;
      } else if (ROLL_BIAS < -25) {
        ppm[0] = 1450 ;
      } else {
        ppm[0] = 1500;
      }

      if (OPENMV_DATA[0] == 1) {
        if (PITCH_BIAS > 20) {
          ppm[1] = 1550;
        } else if (PITCH_BIAS < -20) {
          ppm[1] = 1450;
        } else {
          ppm[1] = 1500 ;
        }
      } else if(OPENMV_DATA[0] != 1){
        ppm[1] = 1410;
      }

      ppm[2] = 1500 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1600 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1400;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;


      if ( millis() - LED_TIMER < 200 ) {
        if (OPENMV_DATA[0] == 1) {
          strip.setPixelColor(0, 255, 0, 0) ;
        } else if (OPENMV_DATA[0] == 2) {
          strip.setPixelColor(0, 0, 0, 255) ;
        } else if (OPENMV_DATA[0] == 3) {
          strip.setPixelColor(0, 0, 255, 0) ;
        } else {
          strip.setPixelColor(0, 255, 255, 255) ;
        }
      } else if ( (millis() - LED_TIMER >= 200) and (millis() - LED_TIMER < 300) ) {
        strip.setPixelColor(0, 0,   0,   0) ;
      } else {
        LED_TIMER  = millis() ;
      }

      strip.show();

      if (
        (   ( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 8) //DUTY_TIME MAX TO 8s
        or  (OPENMV_DATA[0] == 1 and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 4)) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L2_TIME[0] = DUTY_MIN;
        L2_TIME[1] = DUTY_SEC;
        L2_duty = B00000001;
      }
    }
    //BREAKING & WAITING
    else if (L2_duty == B00000001) {

      OPENMV_I2C_GET(3);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 15) {
        ppm[0] = 1550 ;
      } else if (ROLL_BIAS < -15) {
        ppm[0] = 1450 ;
      } else {
        ppm[0] = 1500;
      }

      if (PITCH_BIAS > 12) {
        ppm[1] = 1550;
      } else if (PITCH_BIAS < -12) {
        ppm[1] = 1450;
      } else {
        ppm[1] = 1500 ;
      }

      ppm[2] = 1500 ;//+ (analogRead(SONAR) - IDEAL_HEIGHT);

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1570 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1430;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;

      if ( millis() - LED_TIMER < 200 ) {
        if (OPENMV_DATA[0] == 1) {
          strip.setPixelColor(0, 255, 0, 0) ;
        } else if (OPENMV_DATA[0] == 2) {
          strip.setPixelColor(0, 0, 0, 255) ;
        } else if (OPENMV_DATA[0] == 3) {
          strip.setPixelColor(0, 0, 255, 0) ;
        } else {
          strip.setPixelColor(0, 255, 0, 255) ;
        }
      } else if ( (millis() - LED_TIMER >= 200) and (millis() - LED_TIMER < 300) ) {
        strip.setPixelColor(0, 0,   0,   0) ;
      } else {
        LED_TIMER  = millis() ;
      }

      strip.show();

      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 20) //DUTY_TIME MAX TO 20s
        or
        ((OPENMV_DATA[0] == 2 or OPENMV_DATA[0] == 3) and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 4)) //DETECT COLOR ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L2_TIME[0] = DUTY_MIN;
        L2_TIME[1] = DUTY_SEC;

        if (OPENMV_DATA[0] == 2) {
          MISSION_COLOR == "b";
          L2_duty = B00000011;
        } else if (OPENMV_DATA[0] == 3) {
          MISSION_COLOR == "g";
        } else {
          MISSION_COLOR == "g";
        }
        L2_duty = B00000011;
      }
    }

    //OVER
    else if (L2_duty == B00000011) {
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

      OPENMV_I2C_GET(4);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (OPENMV_DATA[0] == 0) {
        if(MISSION_COLOR == "g"){
          ppm[0] = 1400;
        }else if(MISSION_COLOR == "b"){
          ppm[0] = 1600;
        }else{
          ppm[0] = 1400;
        }
      } else if (OPENMV_DATA[0] == 5) {
        if (ROLL_BIAS > 15) {
          ppm[0] = 1570;
        } else if (ROLL_BIAS < -15) {
          ppm[0] = 1430;
        } else {
          ppm[0] = 1500 ;
        }
      }
      
      if (PITCH_BIAS > 12) {
        ppm[1] = 1570 ;
      } else if (PITCH_BIAS < -12) {
        ppm[1] = 1430 ;
      } else {
        ppm[1] = 1500;
      }


      ppm[2] = 1500 ;

      if (YAW_BIAS > 0 and abs(YAW_BIAS) < 7) {
        ppm[3] = 1570 ;
      } else if (YAW_BIAS < 0 and abs(YAW_BIAS) < 7) {
        ppm[3] = 1430;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;
      
      if(OPENMV_DATA[0] == 0){
        strip.setPixelColor(0, 0, 0, 0) ;
      }else if(OPENMV_DATA[0] == 5){
        strip.setPixelColor(0, 255, 255, 255) ;
      }
      
      if(not((OPENMV_DATA[0] == 5) and (PITCH_BIAS < 15 and PITCH_BIAS > -15) and (ROLL_BIAS < 12 and ROLL_BIAS > -12))){
        tangent_detect_counter = millis();
      }
      boolean stable_on_tangent = 0;
      if(millis() - tangent_detect_counter > 300){
        stable_on_tangent = 1;
      }
      strip.show();
      
      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 10) //DUTY_TIME MAX TO 10s
        or  (stable_on_tangent == 1 and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 8)) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B00000001;
      }
    }
    //STABLE ON TANGENT LINE
    else if(L3_duty == B00000001){
      
      OPENMV_I2C_GET(4);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 18) {
        ppm[0] = 1570;
      } else if (ROLL_BIAS < -18) {
        ppm[0] = 1430;
      } else {
        ppm[0] = 1500 ;
      }
      
      if (PITCH_BIAS > 15) {
        ppm[1] = 1570 ;
      } else if (PITCH_BIAS < -15) {
        ppm[1] = 1430 ;
      } else {
        ppm[1] = 1500;
      }

      ppm[2] = 1500 ;

      if (YAW_BIAS > 0 and abs(YAW_BIAS) < 10) {
        ppm[3] = 1570 ;
      } else if (YAW_BIAS < 0 and abs(YAW_BIAS) < 10) {
        ppm[3] = 1430;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;
      
      if(OPENMV_DATA[0] == 0){
        strip.setPixelColor(0, 0, 0, 0) ;
      }else if(OPENMV_DATA[0] == 5){
        strip.setPixelColor(0, 255, 255, 255) ;
      }
      
      strip.show();
      
      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 4) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B00000011;
      }
    }
    //MOVE FORWORD
    else if (L3_duty == B00000011) {

      OPENMV_I2C_GET(5);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 15) {
        ppm[0] = 1570 ;
      } else if (ROLL_BIAS < -15) {
        ppm[0] = 1430 ;
      } else {
        ppm[0] = 1500;
      }

      ppm[1] = 1400;
      ppm[2] = 1500 ;

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1570 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1430;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;
      strip.setPixelColor(0, 255, 0, 255) ;
      
      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 2) //DUTY_TIME MAX TO 8s
        or  (OPENMV_DATA[0] == 0 and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 2)) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B00000111;
      }
    }
    //FIND SANDBAG AREA AND THROW
    else if (L3_duty == B00000111) {

      if(MISSION_COLOR == "b"){
        strip.setPixelColor(0, 0, 0, 255) ;
        OPENMV_I2C_GET(72);
      }else if(MISSION_COLOR == "g"){
        strip.setPixelColor(0, 0, 255, 0) ;
        OPENMV_I2C_GET(73);
      }else{
        OPENMV_I2C_GET(73);
      }

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 15) {
        ppm[0] = 1570 ;
      } else if (ROLL_BIAS < -15) {
        ppm[0] = 1430 ;
      } else {
        ppm[0] = 1500;
      }
      
      ppm[1] = 1400;

      ppm[2] = 1500 ;

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1600 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1400;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;

      //CHECK THROW MOTION ENABLE
      if(HAS_THROW == 0 and OPENMV_DATA[0] == 5){
        HAS_THROW = 1;
      }
      
      //ENABLE THORW_COUNTER
      if(!HAS_THROW){
        throw_counter = millis();
        //strip.setPixelColor(0, 50, 100, 255) ;
      }else{
        strip.setPixelColor(0, 255, 255, 255) ;
      }

      //THROW
      if( HAS_THROW and ((millis() - throw_counter) > 500) and ((millis() - throw_counter) < 1000)){
          THROW(10);
      }

      if(( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 8){
          THROW(10);
      }
      
      
      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 10) //DUTY_TIME MAX TO 8s
        or  ((millis() - throw_counter > 3000) and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 3)) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        digitalWrite(SANDBAG, LOW);
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B00001111;
      }
    }
    //FOLLOW LINE AND FIND TANGENT POINT
    else if (L3_duty == B00001111) {

      OPENMV_I2C_GET(6);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 15) {
        ppm[0] = 1570 ;
      } else if (ROLL_BIAS < -15) {
        ppm[0] = 1430 ;
      } else {
        ppm[0] = 1500;
      }

      if (OPENMV_DATA[0] == 0) {
        ppm[1] = 1400;
      } else if (OPENMV_DATA[0] == 5) {
        if (PITCH_BIAS > 12) {
          ppm[1] = 1570;
        } else if (PITCH_BIAS < -12) {
          ppm[1] = 1430;
        } else {
          ppm[1] = 1500 ;
        }
      }

      ppm[2] = 1500 ;

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1570 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1430;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;
      
      if(OPENMV_DATA[0] == 0){
        strip.setPixelColor(0, 0, 0, 0) ;
      }else if(OPENMV_DATA[0] == 5){
        strip.setPixelColor(0, 255, 255, 255) ;
      }
      
      if(not((OPENMV_DATA[0] == 5) and (PITCH_BIAS < 15 and PITCH_BIAS > -15) and (ROLL_BIAS < 12 and ROLL_BIAS > -12))){
        tangent_detect_counter = millis();
      }
      boolean stable_on_tangent = 0;
      if(millis() - tangent_detect_counter > 500){
        stable_on_tangent = 1;
      }
      
      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 10) //DUTY_TIME MAX TO 10s
        or  (stable_on_tangent == 1 and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 3)) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B00011111;
      }
    }
    //TANGENT POINT TRUNING
    else if (L3_duty == B00011111) {

      OPENMV_I2C_GET(6);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 18) {
        ppm[0] = 1570 ;
      } else if (ROLL_BIAS < -18) {
        ppm[0] = 1430 ;
      } else {
        ppm[0] = 1500;
      }

      if (PITCH_BIAS > 15) {
        ppm[1] = 1570;
      } else if (PITCH_BIAS < -15) {
        ppm[1] = 1430;
      } else {
        ppm[1] = 1500 ;
      }

      ppm[2] = 1500 ;

      if (MISSION_COLOR == "g") {
        strip.setPixelColor(0, 0, 255, 0) ;
        ppm[3] = 1600 ;
      } else if (MISSION_COLOR == "b") {
        strip.setPixelColor(0, 0, 0, 255) ;
        ppm[3] = 1400;
      } else {
        strip.setPixelColor(0, 255, 0, 0) ;
        ppm[3] = 1600 ;
      }

      ppm[4] = loiter;

      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) >= 2) 
      ) {
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B00111111;
      }
    }
    //TANGENT POINT STABLE
    else if (L3_duty == B00111111){
      OPENMV_I2C_GET(6);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 18) {
        ppm[0] = 1570 ;
      } else if (ROLL_BIAS < -18) {
        ppm[0] = 1430 ;
      } else {
        ppm[0] = 1500;
      }

      if (PITCH_BIAS > 15) {
        ppm[1] = 1570;
      } else if (PITCH_BIAS < -15) {
        ppm[1] = 1430;
      } else {
        ppm[1] = 1500 ;
      }

      ppm[2] = 1500 ;

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1570 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1430;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;
      strip.setPixelColor(0, 100, 255, 100) ;
      strip.show();
      
      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 5) 
      ) {
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B01111111;
      }
    }
    //MOVE FORWORD
    else if (L3_duty == B01111111) {

      OPENMV_I2C_GET(5);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 15) {
        ppm[0] = 1570 ;
      } else if (ROLL_BIAS < -15) {
        ppm[0] = 1430 ;
      } else {
        ppm[0] = 1500;
      }

      ppm[1] = 1400;
      ppm[2] = 1500 ;

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1600 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1400;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;
      strip.setPixelColor(0, 255, 0, 255) ;
      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 3) //DUTY_TIME MAX TO 8s
        or  (OPENMV_DATA[0] == 0 and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 2)) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B11111111;
      }
    }
      
    //OVER
    else if (L3_duty == B11111111) {
      LEVEL_SWITCH = 3;
      L3_CHECKER = 1;
    }
  }
}

void L4() {
  if (!L4_start) {
    L4_TIME[0] = DUTY_MIN;
    L4_TIME[1] = DUTY_SEC;
    L4_duty = B00000000;
    L4_start = 1;
  }
  if (L4_start) {
    //FOLLOW LINE AND FIND TANGENT POINT
    if (L4_duty == B00001111) {

      OPENMV_I2C_GET(6);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 15) {
        ppm[0] = 1570 ;
      } else if (ROLL_BIAS < -15) {
        ppm[0] = 1430 ;
      } else {
        ppm[0] = 1500;
      }

      if (OPENMV_DATA[0] == 0) {
        ppm[1] = 1400;
      } else if (OPENMV_DATA[0] == 5) {
        if (PITCH_BIAS > 12) {
          ppm[1] = 1570;
        } else if (PITCH_BIAS < -12) {
          ppm[1] = 1430;
        } else {
          ppm[1] = 1500 ;
        }
      }

      ppm[2] = 1500 ;

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1570 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1430;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;
      
      if(OPENMV_DATA[0] == 0){
        strip.setPixelColor(0, 0, 0, 0) ;
      }else if(OPENMV_DATA[0] == 5){
        strip.setPixelColor(0, 255, 255, 255) ;
      }
      
      if(not((OPENMV_DATA[0] == 5) and (PITCH_BIAS < 15 and PITCH_BIAS > -15) and (ROLL_BIAS < 12 and ROLL_BIAS > -12))){
        tangent_detect_counter = millis();
      }
      boolean stable_on_tangent = 0;
      if(millis() - tangent_detect_counter > 500){
        stable_on_tangent = 1;
      }
      
      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L4_TIME[0] * 60 + L4_TIME[1] ) > 10) //DUTY_TIME MAX TO 10s
        or  (stable_on_tangent == 1 and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L4_TIME[0] * 60 + L4_TIME[1] ) > 3)) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L4_TIME[0] = DUTY_MIN;
        L4_TIME[1] = DUTY_SEC;
        L4_duty = B00011111;
      }
    }
    //TANGENT POINT TRUNING
    else if (L4_duty == B00011111) {

      OPENMV_I2C_GET(6);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 18) {
        ppm[0] = 1570 ;
      } else if (ROLL_BIAS < -18) {
        ppm[0] = 1430 ;
      } else {
        ppm[0] = 1500;
      }

      if (PITCH_BIAS > 15) {
        ppm[1] = 1570;
      } else if (PITCH_BIAS < -15) {
        ppm[1] = 1430;
      } else {
        ppm[1] = 1500 ;
      }

      ppm[2] = 1500 ;

      if (MISSION_COLOR == "g") {
        strip.setPixelColor(0, 0, 255, 0) ;
        ppm[3] = 1600 ;
      } else if (MISSION_COLOR == "b") {
        strip.setPixelColor(0, 0, 0, 255) ;
        ppm[3] = 1400;
      } else {
        strip.setPixelColor(0, 255, 0, 0) ;
        ppm[3] = 1600 ;
      }

      ppm[4] = loiter;

      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L4_TIME[0] * 60 + L4_TIME[1] ) >= 2) 
      ) {
        L4_TIME[0] = DUTY_MIN;
        L4_TIME[1] = DUTY_SEC;
        L4_duty = B00111111;
      }
    }
    //TANGENT POINT STABLE
    else if (L3_duty == B00111111){
      OPENMV_I2C_GET(6);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 18) {
        ppm[0] = 1570 ;
      } else if (ROLL_BIAS < -18) {
        ppm[0] = 1430 ;
      } else {
        ppm[0] = 1500;
      }

      if (PITCH_BIAS > 15) {
        ppm[1] = 1570;
      } else if (PITCH_BIAS < -15) {
        ppm[1] = 1430;
      } else {
        ppm[1] = 1500 ;
      }

      ppm[2] = 1500 ;

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1570 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1430;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;
      strip.setPixelColor(0, 100, 255, 100) ;
      strip.show();
      
      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 5) 
      ) {
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B01111111;
      }
    }
    //MOVE FORWORD
    else if (L3_duty == B01111111) {

      OPENMV_I2C_GET(5);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 15) {
        ppm[0] = 1570 ;
      } else if (ROLL_BIAS < -15) {
        ppm[0] = 1430 ;
      } else {
        ppm[0] = 1500;
      }

      ppm[1] = 1400;
      ppm[2] = 1500 ;

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1600 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 83) {
        ppm[3] = 1400;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;
      strip.setPixelColor(0, 255, 0, 255) ;
      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 3) //DUTY_TIME MAX TO 8s
        or  (OPENMV_DATA[0] == 0 and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L3_TIME[0] * 60 + L3_TIME[1] ) > 2)) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L3_TIME[0] = DUTY_MIN;
        L3_TIME[1] = DUTY_SEC;
        L3_duty = B11111111;
      }
    }
    //FIND LANDING AREA
    else if (L4_duty == B00001111) {
      OPENMV_I2C_GET(9);

      PITCH_BIAS = int(OPENMV_DATA[2]);
      ROLL_BIAS = int(OPENMV_DATA[1]);
      YAW_BIAS = int(OPENMV_DATA[3]);

      if (ROLL_BIAS > 25) {
        ppm[0] = 1550 ;
      } else if (ROLL_BIAS < -25) {
        ppm[0] = 1450 ;
      } else {
        ppm[0] = 1500;
      }

      if (PITCH_BIAS > 20) {
        ppm[1] = 1550;
      } else if (PITCH_BIAS < -20) {
        ppm[1] = 1450;
      } else {
        ppm[1] = 1500 ;
      }

      ppm[2] = 1500 ;

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 80) {
        ppm[3] = 1600 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 80) {
        ppm[3] = 1400;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;
      strip.setPixelColor(0, 100, 255, 100) ;
      if (
        ( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 8 )
      {
        L4_TIME[0] = DUTY_MIN;
        L4_TIME[1] = DUTY_SEC;
        L4_duty = B00011111;
      }
    }
    //CHOOSE LANDING COLOR
    else if (L4_duty == B00011111) {
      if (MISSION_COLOR == "b") {
        strip.setPixelColor(0, 0, 0, 255) ;
        OPENMV_I2C_GET(10);
      }
      else if (MISSION_COLOR == "g") {
        strip.setPixelColor(0, 0, 255, 0) ;
        OPENMV_I2C_GET(11);
      }

      if (ROLL_BIAS > 25) {
        ppm[0] = 1550 ;
      } else if (ROLL_BIAS < -25) {
        ppm[0] = 1450 ;
      } else {
        ppm[0] = 1500;
      }

      ppm[1] = 1550;
      ppm[2] = 1500 ;

      if (YAW_BIAS < 0 and abs(YAW_BIAS) < 80) {
        ppm[3] = 1600 ;
      } else if (YAW_BIAS > 0 and abs(YAW_BIAS) < 80) {
        ppm[3] = 1400;
      } else {
        ppm[3] = 1500;
      }

      ppm[4] = loiter;

      if (
        (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 5) //DUTY_TIME MAX TO 8s
        or  (OPENMV_DATA[0] == 1 and (( DUTY_MIN * 60 + DUTY_SEC ) - ( L2_TIME[0] * 60 + L2_TIME[1] ) > 2)) //DETECT RED AND ALSO DUTY_TIME BIGGER THEN 4s
      ) {
        L4_TIME[0] = DUTY_MIN;
        L4_TIME[1] = DUTY_SEC;
        L4_duty = B00111111;
      }
    }
    //OVER
    else if (L4_duty == B00111111) {
      LEVEL_SWITCH = 4;
      L4_CHECKER = 1;
    }
  }
}


// ISR PROGRAM --ISRPRO
ISR(TIMER1_COMPA_vect) {
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