/*
 *THIS PROGRAM DIDN'T USE IN FINAL PROJECT 
 *
 *this program is for the earlir develop tool, it's show the data from the Syno system(see /Syno),data transfer via RF24L01 module.
 *
 *It did't using because by the lagging when data is transmitting, this time delay would cause system processing loop frequency only around 1Hz, too slow for our algorithm.
 * 
 */


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"

#define L_JOY_X A3
#define L_JOY_Y A2
#define R_JOY_X A0
#define R_JOY_Y A1
#define FUNCB_U 2
#define FUNCB_D 4
#define FUNCB_R A6
#define FUNCB_L 3
#define ENC_A 9
#define ENC_B 10
#define L_JOY_B 6
#define R_JOY_B "UNDEFINE"

//RF24
RF24 radio(7, 8);
const uint64_t pipe = 0xE8E8F0F1A1LL ;

//OLED
Adafruit_SSD1306 display(128, 32, &Wire, 4);

//IO
int list[] = {"A3", "A2", "A0", "A1", "2", "4", "A6", "3", "9", "10", "6", "A7"};

//UI
String Menu[] = {"1.Info", "2.Manual", "3.Setting" , "    "};
int frame = 1;
int page = 1;
int SELECT = 1 ;

String Setting_Menu[] = {"1.J-stick", "2.PID"};

//funcb timer
unsigned long FUNCB_U_TIMER;
unsigned long FUNCB_D_TIMER;

//joystick
int LX_LOW = 0, LX_MIDDLE = 506 , LX_HIGH = 1023;
int LY_LOW = 0, LY_MIDDLE = 526 , LY_HIGH = 1023;
int RX_LOW = 0, RX_MIDDLE = 525 , RX_HIGH = 1023;
int RY_LOW = 0, RY_MIDDLE = 512 , RY_HIGH = 1023;

int LX_PWM_OUTPUT;
int LY_PWM_OUTPUT;
int RX_PWM_OUTPUT;
int RY_PWM_OUTPUT;

//RF24
typedef struct {

  int RC_INPUT[5];
  
  int CH_OUTPUT[5];

  int VOLTAGE;
  
  int OPENMV_STATUS;

  int MODE;

  int STATUS_CODE;

} RECEIVE;

RECEIVE RECEIVE_DATA ;


boolean alaststate;

void setup() {

  display.begin(0x3C);
  Serial.begin(9600);
  display.clearDisplay();
//  display.drawBitmap(0, 0,  logo, 128, 32, 1);
  display.display();
  delay(2000);

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(1);
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(1,pipe);
  radio.setCRCLength(RF24_CRC_8);
  radio.setAutoAck(0);
  radio.startListening();

  pinMode(L_JOY_X, INPUT);
  pinMode(L_JOY_Y, INPUT);
  pinMode(R_JOY_X, INPUT);
  pinMode(R_JOY_Y, INPUT);
  pinMode(FUNCB_U, INPUT);
  pinMode(FUNCB_D, INPUT);
  pinMode(FUNCB_R, INPUT);
  pinMode(FUNCB_L, INPUT);
  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  pinMode(L_JOY_B, INPUT);
  if (digitalRead(FUNCB_U)) {
    while (1) {
      TEST_MODE();
    }
  }

  //alaststate = digitalRead(ENC_A);
//  timer = millis();
  FUNCB_U_TIMER = millis();
  FUNCB_D_TIMER = millis();
}

void loop() {
  INFO_READ();
  display.display();
}

void MENU() {
  display.clearDisplay();

  //frame
  if (frame == 1) {
    display.setCursor(10, 0);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.print(Menu[frame - 1]);
    display.setCursor(10, 16);
    display.print(Menu[frame]);
  } else if (frame == 2) {
    display.setCursor(10, 0);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.println(Menu[2]);
    display.setCursor(10, 16);
    display.println(Menu[3]);
  }
  if (SELECT > 2) {
    if (frame == 1) {
      frame++;
      SELECT = 1;
    }
  } else if (SELECT < 1) {
    if (frame == 2) {
      frame--;
      SELECT = 2;
    }
  }

  //RANGE_CHECK
  if (SELECT > 2) {
    SELECT = 2;
  } else if (SELECT < 1) {
    SELECT = 1;
  }

  //select
  if (SELECT == 1) {
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.print( ">" );
  } else if (SELECT == 2) {
    display.setCursor(0, 16);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.print( ">" );
  }


  //FUNC_B
  if (digitalRead(FUNCB_U) == 1) {
    if (millis() - FUNCB_U_TIMER > 200) {
      SELECT--;
      FUNCB_U_TIMER = millis();
    }
  } else if (digitalRead(FUNCB_D) == 1) {
    if (millis() - FUNCB_D_TIMER > 200) {
      SELECT++;
      FUNCB_D_TIMER = millis();
    }
  }


}

void CLICK_IN() {

  if (analogRead(FUNCB_R) > 500) {
    if (frame == 1 && SELECT == 1) {
      INFO_READ();
    } else if (frame == 1 && SELECT == 2) {
      MANUEL();
    } else if (frame == 2 && SELECT == 1) {
      SETTING();
    }
  }
}

void AUTO() {
  while (1) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("Nothing Here :)");
    display.display();
    if (digitalRead(FUNCB_L) == HIGH) {
      break;
    }
  }
}

void MANUEL() {
  while (1) {
    //LX
    display.clearDisplay();
    if (analogRead(L_JOY_X) < LX_MIDDLE) {
      LX_PWM_OUTPUT = map(analogRead(L_JOY_X), LX_LOW, LX_MIDDLE, 500, 1250);
    } else if (analogRead(L_JOY_X) >= LX_MIDDLE) {
      LX_PWM_OUTPUT = map(analogRead(L_JOY_X), LX_MIDDLE, LX_HIGH, 1250, 2000);
    }
    //LY
    if (analogRead(L_JOY_Y) < LY_MIDDLE) {
      LY_PWM_OUTPUT = map(analogRead(L_JOY_Y), LY_LOW, LY_MIDDLE, 500, 1250);
    } else if (analogRead(L_JOY_Y) >= LY_MIDDLE) {
      LY_PWM_OUTPUT = map(analogRead(L_JOY_Y), LY_MIDDLE, LY_HIGH, 1250, 2000);
    }
    //RX
    if (analogRead(R_JOY_X) < RX_MIDDLE) {
      RX_PWM_OUTPUT = map(analogRead(R_JOY_X), RX_LOW, RX_MIDDLE, 500, 1250);
    } else if (analogRead(R_JOY_X) >= RX_MIDDLE) {
      RX_PWM_OUTPUT = map(analogRead(R_JOY_X), RX_MIDDLE, RX_HIGH, 1250, 2000);
    }
    //RY
    if (analogRead(R_JOY_Y) < RY_MIDDLE) {
      RY_PWM_OUTPUT = map(analogRead(R_JOY_Y), RY_LOW, RY_MIDDLE, 500, 1250);
    } else if (analogRead(R_JOY_Y) >= RY_MIDDLE) {
      RY_PWM_OUTPUT = map(analogRead(R_JOY_Y), RY_MIDDLE, RY_HIGH, 1250, 2000);
    }

    //SEND_DATA
    /*SEND.THROTTLE = LX_PWM_OUTPUT;
    SEND.YAW = LY_PWM_OUTPUT;
    SEND.PITCH = RX_PWM_OUTPUT;
    SEND.ROLL = RY_PWM_OUTPUT;
    */
    /*
        display.setCursor(10, 0);
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.print(LX_PWM_OUTPUT);
        display.setCursor(10, 16);
        display.print(LY_PWM_OUTPUT);
        display.setCursor(60, 0);
        display.print(RX_PWM_OUTPUT);
        display.setCursor(60, 16);
        display.print(RY_PWM_OUTPUT);
    */
    display.drawLine(32, 16, map(LX_PWM_OUTPUT, 500, 2000, 48, 16), map(LY_PWM_OUTPUT, 500, 2000, 32, 0), WHITE);
    display.drawLine(96, 16, map(RX_PWM_OUTPUT, 500, 2000, 80, 112), map(RY_PWM_OUTPUT, 500, 2000, 32, 0), WHITE);


    display.setTextSize(1);
    display.setCursor(55, 0);
    display.print(LX_PWM_OUTPUT);
    display.setCursor(55, 8);
    display.print(LY_PWM_OUTPUT);
    display.setCursor(55, 16);
    display.print(RX_PWM_OUTPUT);
    display.setCursor(55, 24);
    display.print(RY_PWM_OUTPUT);
    display.display();

    if (digitalRead(FUNCB_L) == 1) {
      break;
    }
  }
}

void SETTING() {
  while (1) {
    display.clearDisplay();
    if (frame == 1) {
      display.setCursor(10, 0);
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.print(Setting_Menu[frame - 1]);
      display.setCursor(10, 16);
      display.print(Setting_Menu[frame]);
    } else if (frame == 2) {
      frame = 1;
    }

    //RANGE_CHECK
    if (SELECT > 2) {
      SELECT = 2;
    } else if (SELECT < 1) {
      SELECT = 1;
    }

    //select
    if (SELECT == 1) {
      display.setCursor(0, 0);
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.print( ">" );
    } else if (SELECT == 2) {
      display.setCursor(0, 16);
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.print( ">" );
    }

    //FUNC_B
    if (digitalRead(FUNCB_U) == 1) {
      if (millis() - FUNCB_U_TIMER > 200) {
        SELECT--;
        FUNCB_U_TIMER = millis();
      }
    } else if (digitalRead(FUNCB_D) == 1) {
      if (millis() - FUNCB_D_TIMER > 200) {
        SELECT++;
        FUNCB_D_TIMER = millis();
      }
    }
    if (digitalRead(FUNCB_L) == 1) {
      frame = 1;
      SELECT = 1;
      break;
    }
    display.display();
  }
}

void INFO_READ(){
  if(radio.available()){
    while(radio.available()){
      radio.read(&RECEIVE_DATA , sizeof(RECEIVE_DATA));
    }
  }
  
  display.clearDisplay();
  /*
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.print(RECEIVE_DATA.RC_INPUT[0]);
  display.print(" ");
  display.print(RECEIVE_DATA.RC_INPUT[1]);
  display.print(" ");
  display.print(RECEIVE_DATA.RC_INPUT[2]);
  display.print(" ");
  display.print(RECEIVE_DATA.RC_INPUT[3]);
  display.println(" ");
  display.print(RECEIVE_DATA.RC_INPUT[4]);
  display.print(" ");
  display.print(RECEIVE_DATA.CH_OUTPUT[0]);
  display.print(" ");
  display.print(RECEIVE_DATA.CH_OUTPUT[1]);
  display.print(" ");
  display.print(RECEIVE_DATA.CH_OUTPUT[2]);
  display.print(" ");
  display.println(RECEIVE_DATA.CH_OUTPUT[3]);
  */
  display.drawLine(32, 16, map(RECEIVE_DATA.CH_OUTPUT[3], 1912, 1092, 48, 16), map(RECEIVE_DATA.CH_OUTPUT[2], 1092, 1912, 32, 0), WHITE);
  display.drawLine(96, 16, map(RECEIVE_DATA.CH_OUTPUT[0], 1092, 1912, 80, 112), map(RECEIVE_DATA.CH_OUTPUT[1], 1912, 1092, 32, 0), WHITE);
  display.setCursor(40,24);
  display.setTextColor(WHITE);
  display.println(RECEIVE_DATA.OPENMV_STATUS);
}


void TEST_MODE() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.print(analogRead(L_JOY_X)); display.print(",");
  display.print(analogRead(L_JOY_Y)); display.print(",");
  display.print(analogRead(R_JOY_X)); display.print(",");
  display.print(analogRead(R_JOY_Y)); display.println(",");
  display.print(digitalRead(FUNCB_U)); display.print(",");
  display.print(digitalRead(FUNCB_D)); display.print(",");
  display.print(analogRead(FUNCB_R)); display.print(",");
  display.print(digitalRead(FUNCB_L)); display.print(",");
  display.print(digitalRead(ENC_A)); display.print(",");
  display.print(digitalRead(ENC_B)); display.print(",");
  display.print(digitalRead(L_JOY_B)); display.print(",");
  display.display();

  Serial.print(analogRead(L_JOY_X)); Serial.print(",");
  Serial.print(analogRead(L_JOY_Y)); Serial.print(",");
  Serial.print(analogRead(R_JOY_X)); Serial.print(",");
  Serial.print(analogRead(R_JOY_Y)); Serial.print(",");
  Serial.print(digitalRead(FUNCB_U)); Serial.print(",");
  Serial.print(digitalRead(FUNCB_D)); Serial.print(",");
  Serial.print(analogRead(FUNCB_R)); Serial.print(",");
  Serial.print(digitalRead(FUNCB_L)); Serial.print(",");
  Serial.print(digitalRead(ENC_A)); Serial.print(",");
  Serial.print(digitalRead(ENC_B)); Serial.print(",");
  Serial.print(digitalRead(L_JOY_B)); Serial.print(",");
  Serial.println();
}
