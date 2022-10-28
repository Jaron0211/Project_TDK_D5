/*
  20190702
  UPDATE:
  1.THIS VERSION IS FOR THE FIRST VERSION OF THE D4(CONTROLLER)
  2.CHANGE PWM RANGE FROM 1000~2000 TO 1000~1600
  3.ADD UPDATE NOTE ON THE TOP

*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"

#define L_JOY_X A0
#define L_JOY_Y A1
#define R_JOY_X A2
#define R_JOY_Y A3
#define FUNCB_U 2
#define FUNCB_D 4
#define FUNCB_R A6
#define FUNCB_L 3
#define ENC_A 9
#define ENC_B 10
#define R_JOY_B "UNDEFINE"
#define MODE_SWITCH 6
#define BATTERY A7


//OLED
Adafruit_SSD1306 display(128, 32, &Wire, 4);

//RF24
RF24 radio(7, 8);
const uint64_t pipe[2] = {0xE8E8F0F0A1LL, 0xE8E8F0F0A2LL } ;

//IO
int list[] = {"A3", "A2", "A0", "A1", "2", "4", "A6", "3", "9", "10", "6", "A7"};

//UI
String Menu[] = {"| Auto", "| Manual", "| Setting" , "| PID"};
int frame = 1;
int page = 1;
int SELECT = 1 ;

String Setting_Menu[] = {"1.Leveing", "2.ESC Cel", "3.IMU Cel", " "};\

String mode_name;

boolean FUNCTION_TEST = 0;

const unsigned char LOW_BATTERY [] PROGMEM = {
0xFF,0xFF,0xFF,0xFC,0xEF,0xE0,0x00,0x04,0xEF,0xE0,0x00,0x04,0xEF,0xE0,0x00,0x04,
0xEF,0xE0,0x00,0x04,0xEF,0xE0,0x00,0x04,0xEF,0xE0,0x00,0x04,0xEF,0xE0,0x00,0x04,
0xEF,0xE0,0x00,0x04,0xFF,0xFF,0xFF,0xFC


};

//encoder
int astate;
int bstate;
int alaststate;
int blaststate;
int CHECKER = 0;
unsigned long timer;

//funcb timer
unsigned long FUNCB_U_TIMER;
unsigned long FUNCB_D_TIMER;
unsigned long FUNCB_R_TIMER;
unsigned long button_timer;

//start timer
unsigned long star_timer ;

//joystick
int LX_LOW = 0, LX_MIDDLE = 515 , LX_HIGH = 1023;
int LY_LOW = 1023, LY_MIDDLE = 527 , LY_HIGH = 0;
int RX_LOW = 0, RX_MIDDLE = 497 , RX_HIGH = 1023;
int RY_LOW = 0, RY_MIDDLE = 510 , RY_HIGH = 1023;

int LX_PWM_OUTPUT;
int LY_PWM_OUTPUT;
int RX_PWM_OUTPUT;
int RY_PWM_OUTPUT;

int PWM_HIGH = 1800;
int PWM_MIDDLE = 1400;
int PWM_LOW = 1000;

int PID_value[3] = {25, 1, 1};

//RF24
typedef struct {

  int pitch_d;
  int roll_d;
  int yaw_d;
  int ax;
  int ay;
  int az;
  int gx;
  int gy;
  int gz;

} RECEIVE_DATA;
RECEIVE_DATA receive;
typedef struct {

  int THROTTLE;
  int YAW;
  int PITCH;
  int ROLL;
  int FUNC_1;
  int FUNC_2;
  int MODE;
  int Test;

} SEND_DATA;
SEND_DATA send_data;

//adjust timer
unsigned long adjust_timer;

//IMU CORRECTION
boolean IMU_CORRECTION_CHECK = false;

//AUTO
unsigned long AUTO_TIMER;

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void setup() {
  Serial.begin(115200);

  display.begin(0x3C);
  display.clearDisplay();

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
  pinMode(MODE_SWITCH, INPUT);
  pinMode(BATTERY, INPUT);
  if (digitalRead(FUNCB_U)) {
    while (1) {
      TEST_MODE();
    }
  }

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(1);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_8);
  radio.setAutoAck(0);
  radio.openWritingPipe(pipe[1]);
  radio.stopListening();

  alaststate = digitalRead(ENC_A);
  timer = millis();
  FUNCB_U_TIMER = millis();
  FUNCB_D_TIMER = millis();
  FUNCB_R_TIMER = millis();
}

void loop_1() {
  MANUEL();
  display.display();
}

void loop() {
  MENU();
  CLICK_IN();
  display.display();
}
void Data_send() {
  radio.write(&send_data, sizeof(send_data));
  radio.stopListening();
}

void MENU() {
  display.clearDisplay();
  show_battery();
  //frame

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

  if (frame == 1) {
    display.setCursor(10, 0);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.println(Menu[frame - 1]);
    display.setCursor(10, 16);
    display.println(Menu[frame]);
  } else if (frame == 2) {
    display.setCursor(10, 0);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.println(Menu[2]);
    display.setCursor(10, 16);
    display.println(Menu[3]);
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
    display.println( ">" );
  } else if (SELECT == 2) {
    display.setCursor(0, 16);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.println( ">" );
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

  send_data.THROTTLE = 1000;
  send_data.YAW = 1400;
  send_data.PITCH = 1400;
  send_data.ROLL = 1400;
  send_data.FUNC_1 = 0;
  send_data.FUNC_2 = 0;

  Data_send();

}
void CLICK_IN() {

  if (analogRead(FUNCB_R) > 700) {
    if (frame == 1 && SELECT == 1) {
      AUTO();
    } else if (frame == 1 && SELECT == 2) {
      MANUEL();
    } else if (frame == 2 && SELECT == 1) {
      //SETTING();
    } else if (frame == 2 && SELECT == 2) {
      //PID();
    }
  }
}
void AUTO() {
  send_data.FUNC_2 = 10;
  while (1) {
    
    display.clearDisplay();
    show_battery();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Auto mode");
    display.display();
    display.setCursor(0,16);


    if ( LX_MIDDLE - 10 > analogRead(L_JOY_X) or analogRead(L_JOY_X) > LX_MIDDLE + 10 ) {
      MANUEL();
      break;
    }
    if ( LY_MIDDLE - 10 > analogRead(L_JOY_Y) or analogRead(L_JOY_Y) > LY_MIDDLE + 10 ) {
      MANUEL();
      break;
    }
    if ( RX_MIDDLE - 10 > analogRead(R_JOY_X) or analogRead(R_JOY_X) > RX_MIDDLE + 10 ) {
      MANUEL();
      break;
    }
    if ( RY_MIDDLE - 10 > analogRead(R_JOY_Y) or analogRead(R_JOY_Y) > RY_MIDDLE + 10 ) {
      MANUEL();
      break;
    }

    send_data.THROTTLE = LY_PWM_OUTPUT;
    send_data.YAW = LX_PWM_OUTPUT;
    send_data.PITCH = RX_PWM_OUTPUT;
    send_data.ROLL = RY_PWM_OUTPUT;
    send_data.FUNC_1 = 2;

    if (digitalRead(FUNCB_U) == LOW) {
      AUTO_TIMER = millis();
    }
    if (millis() - AUTO_TIMER > 3000) {
      send_data.FUNC_2 = 1;
      Serial.print("1");
    }

    Data_send();

    if (digitalRead(FUNCB_L) == HIGH) {
      send_data.FUNC_1 = 0;
      Data_send();
      break;
    }
  }
}
void MANUEL() {
  
  FUNCB_R_TIMER = millis();
  send_data.MODE = 1;
  while (1) {
    Serial.println(freeRam ());
    display.clearDisplay();
    show_battery();
    
    //LX   
    if (analogRead(L_JOY_X) < LX_MIDDLE) {
      int value;
      if(analogRead(L_JOY_X) > 1000){
        value = 1023;
      }else{
        value = analogRead(L_JOY_X);
      }
      LX_PWM_OUTPUT = map(value, LX_LOW, LX_MIDDLE, PWM_LOW, PWM_MIDDLE);
    } else if (analogRead(L_JOY_X) > LX_MIDDLE) {
      LX_PWM_OUTPUT = map(analogRead(L_JOY_X), LX_MIDDLE, LX_HIGH, PWM_MIDDLE, PWM_HIGH);
    } else if (analogRead(L_JOY_X) == LX_MIDDLE) {
      LX_PWM_OUTPUT = PWM_MIDDLE;
    }
    
    //LY
    if (analogRead(L_JOY_Y) < LY_MIDDLE) {
      int value;
      if(analogRead(L_JOY_Y) > 1000){
        value = 1023;
      }else{
        value = analogRead(L_JOY_Y);
      }
      LY_PWM_OUTPUT = map(value, LY_LOW, LY_MIDDLE, PWM_LOW, PWM_MIDDLE);
    } else if (analogRead(L_JOY_Y) > LY_MIDDLE) {
      LY_PWM_OUTPUT = map(analogRead(L_JOY_Y), LY_MIDDLE, LY_HIGH, PWM_MIDDLE, PWM_HIGH);
    } else if (analogRead(L_JOY_Y) == LY_MIDDLE) {
      LY_PWM_OUTPUT = PWM_MIDDLE;
    }
    
    //RX
    if (analogRead(R_JOY_X) < RX_MIDDLE) {
      int value;
      if(analogRead(R_JOY_X) > 1000){
        value = 1023;
      }else{
        value = analogRead(R_JOY_X);
      }
      RX_PWM_OUTPUT = map(value, RX_LOW, RX_MIDDLE, PWM_LOW, PWM_MIDDLE) - 16;
    } else if (analogRead(R_JOY_X) > RX_MIDDLE) {
      RX_PWM_OUTPUT = map(analogRead(R_JOY_X), RX_MIDDLE, RX_HIGH, PWM_MIDDLE, PWM_HIGH) - 16;
    } else if (analogRead(R_JOY_X) == RX_MIDDLE) {
      RX_PWM_OUTPUT = PWM_MIDDLE;
    }
    
    //RY
    if (analogRead(R_JOY_Y) < RY_MIDDLE) {
      int value;
      if(analogRead(R_JOY_Y) > 1000){
        value = 1023;
      }else{
        value = analogRead(R_JOY_Y);
      }
      RY_PWM_OUTPUT = map(value, RY_LOW, RY_MIDDLE, PWM_LOW, PWM_MIDDLE) + 1;
    } else if (analogRead(R_JOY_Y) > RY_MIDDLE) {
      RY_PWM_OUTPUT = map(analogRead(R_JOY_Y), RY_MIDDLE, RY_HIGH, PWM_MIDDLE, PWM_HIGH) + 1;
    } else if (analogRead(R_JOY_Y) == RY_MIDDLE) {
      RY_PWM_OUTPUT = PWM_MIDDLE;
    }
    
    //Button
    if(FUNCTION_TEST == 0){
      if (digitalRead(FUNCB_U) == 1) {
        if (millis() - FUNCB_U_TIMER > 200) {
          send_data.Test-= 1;
          FUNCB_U_TIMER = millis();
        }
      } else if (digitalRead(FUNCB_D) == 1) {
        if (millis() - FUNCB_D_TIMER > 200) {
          send_data.Test++;
          FUNCB_D_TIMER = millis();
        }
      }
    }
    send_data.Test = constrain(send_data.Test, 0, 6);

    //Auto & Manual
    if(FUNCTION_TEST == 0){
      if (analogRead(FUNCB_R) < 500) {
          FUNCB_R_TIMER = millis();
        }else{
          if(millis() - FUNCB_R_TIMER > 300){
            FUNCTION_TEST = 1;
            FUNCB_R_TIMER = millis();
          }
        }
    }
    //safety break
    if ( LX_MIDDLE - 10 > analogRead(L_JOY_X) or analogRead(L_JOY_X) > LX_MIDDLE + 10 ) {
      FUNCTION_TEST = 0;
    }
    if ( LY_MIDDLE - 10 > analogRead(L_JOY_Y) or analogRead(L_JOY_Y) > LY_MIDDLE + 10 ) {
      FUNCTION_TEST = 0;
    }
    if ( RX_MIDDLE - 10 > analogRead(R_JOY_X) or analogRead(R_JOY_X) > RX_MIDDLE + 10 ) {
      FUNCTION_TEST = 0;
    }
    if ( RY_MIDDLE - 10 > analogRead(R_JOY_Y) or analogRead(R_JOY_Y) > RY_MIDDLE + 10 ) {
      FUNCTION_TEST = 0;
    }
    
    //SEND_DATA
    if (digitalRead(MODE_SWITCH) == 1) {
      if(millis() - button_timer > 300 ){ 
        send_data.MODE++;
        button_timer = millis();
      }
    }
    if(send_data.MODE>1){
      send_data.MODE = 0;
    }else if(send_data.MODE<0){
      send_data.MODE = 1;
    }
    send_data.MODE = constrain(send_data.MODE, 0, 1);
    Serial.print(send_data.MODE);
    Serial.print(",");

    send_data.THROTTLE = LY_PWM_OUTPUT;
    Serial.print(send_data.THROTTLE);
    Serial.print(",");
    send_data.YAW = LX_PWM_OUTPUT;
    Serial.print(send_data.YAW);
    Serial.print(",");
    send_data.PITCH = RX_PWM_OUTPUT;
    Serial.print(send_data.PITCH);
    Serial.print(",");
    send_data.ROLL = RY_PWM_OUTPUT;
    Serial.print(send_data.ROLL);
    Serial.print(",");
    
    if(FUNCTION_TEST == 0){
      send_data.FUNC_1 = 1;
      send_data.FUNC_2 = 10;
    }else if (FUNCTION_TEST == 1){
      send_data.FUNC_1 = 3;
      send_data.FUNC_2 = 0;      
    }
    Serial.print(send_data.FUNC_1);
    Serial.print(",");
    Serial.print(send_data.FUNC_2);
    Serial.print(",");
    Serial.print(send_data.Test);
    Serial.println();
    
    Data_send();

    //UI
    if(FUNCTION_TEST == 0){
      display.drawLine(32, 16, map(LX_PWM_OUTPUT, PWM_HIGH, PWM_LOW, 48, 16), map(LY_PWM_OUTPUT, PWM_LOW, PWM_HIGH, 32, 0), WHITE);
      display.drawLine(96, 16, map(RX_PWM_OUTPUT, PWM_LOW, PWM_HIGH, 80, 112), map(RY_PWM_OUTPUT, PWM_HIGH, PWM_LOW, 32, 0), WHITE);
    }else if (FUNCTION_TEST == 1){
      display.setCursor(20,10);
      display.setTextSize(2);
      display.print("Auto");
    }
    if (send_data.MODE == 0){
      mode_name = "Stabilize";
    }else if (send_data.MODE == 1){
      mode_name = "AltHold";
    }
    display.setTextSize(1);
    display.setCursor(60,0);
    display.print(mode_name);
    display.setCursor(10,0);
    switch(send_data.Test){
     case 0 :
        display.print("Take_off");
        break;
     case 1 :
        display.print("Part1");
        break;
     case 2 :
        display.print("Part2");
        break;
     case 3 :
        display.print("Part3_B");
        break;
     case 4 :
        display.print("Part3_G");
        break;
     case 5 :
        display.print("Part4_B");
        break;
     case 6 :
        display.print("Part4_G");
        break;
    }
    
    display.display();

    //break
    if (digitalRead(FUNCB_U) == 1) {
      send_data.FUNC_2 = 10;
      FUNCTION_TEST = 0;
      Data_send();
    }
    if (digitalRead(FUNCB_L) == 1) {
      send_data.FUNC_2 = 10;
      FUNCTION_TEST = 0;
      Data_send();
      break;
    }
  }
}
void TEST_MODE() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.print(analogRead(L_JOY_X)); display.print(",");
  display.print(analogRead(L_JOY_Y)); display.print(",");
  display.print(analogRead(R_JOY_X)); display.print(",");
  display.print(analogRead(R_JOY_Y)); display.print(",");
  display.print(digitalRead(FUNCB_U)); display.print(",");
  display.print(digitalRead(FUNCB_D)); display.print(",");
  display.print(analogRead(FUNCB_R)); display.print(",");
  display.print(digitalRead(FUNCB_L)); display.print(",");
  display.print(digitalRead(ENC_A)); display.print(",");
  display.print(digitalRead(ENC_B)); display.print(",");
  display.print(digitalRead(MODE_SWITCH)); display.print(",");
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
  //  Serial.print(digitalRead(L_JOY_B)); Serial.print(",");
  Serial.println();
}

void show_battery(){
  int value = analogRead(BATTERY);
  Serial.println(value);
  display.setCursor(96,4);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.drawRect(121, 4, 6, 28, WHITE);
  
  value = map(value,840,700,28,0);
  display.fillRect(122, 32-value, 4, value, WHITE);

}

void MANUEL_backup() {
  send_data.MODE = 1;
  while (1) {
    display.clearDisplay();
    
    //LX   
    if (analogRead(L_JOY_X) < LX_MIDDLE) {
      int value;
      if(analogRead(L_JOY_X) > 1000){
        value = 1023;
      }else{
        value = analogRead(L_JOY_X);
      }
      LX_PWM_OUTPUT = map(value, LX_LOW, LX_MIDDLE, PWM_LOW, PWM_MIDDLE);
    } else if (analogRead(L_JOY_X) > LX_MIDDLE) {
      LX_PWM_OUTPUT = map(analogRead(L_JOY_X), LX_MIDDLE, LX_HIGH, PWM_MIDDLE, PWM_HIGH);
    } else if (analogRead(L_JOY_X) == LX_MIDDLE) {
      LX_PWM_OUTPUT = PWM_MIDDLE;
    }
    
    //LY
    if (analogRead(L_JOY_Y) < LY_MIDDLE) {
      int value;
      if(analogRead(L_JOY_Y) > 1000){
        value = 1023;
      }else{
        value = analogRead(L_JOY_Y);
      }
      LY_PWM_OUTPUT = map(value, LY_LOW, LY_MIDDLE, PWM_LOW, PWM_MIDDLE);
    } else if (analogRead(L_JOY_Y) > LY_MIDDLE) {
      LY_PWM_OUTPUT = map(analogRead(L_JOY_Y), LY_MIDDLE, LY_HIGH, PWM_MIDDLE, PWM_HIGH);
    } else if (analogRead(L_JOY_Y) == LY_MIDDLE) {
      LY_PWM_OUTPUT = PWM_MIDDLE;
    }
    
    //RX
    if (analogRead(R_JOY_X) < RX_MIDDLE) {
      int value;
      if(analogRead(R_JOY_X) > 1000){
        value = 1023;
      }else{
        value = analogRead(R_JOY_X);
      }
      RX_PWM_OUTPUT = map(value, RX_LOW, RX_MIDDLE, PWM_LOW, PWM_MIDDLE) - 16;
    } else if (analogRead(R_JOY_X) > RX_MIDDLE) {
      RX_PWM_OUTPUT = map(analogRead(R_JOY_X), RX_MIDDLE, RX_HIGH, PWM_MIDDLE, PWM_HIGH) - 16;
    } else if (analogRead(R_JOY_X) == RX_MIDDLE) {
      RX_PWM_OUTPUT = PWM_MIDDLE;
    }
    
    //RY
    if (analogRead(R_JOY_Y) < RY_MIDDLE) {
      int value;
      if(analogRead(R_JOY_Y) > 1000){
        value = 1023;
      }else{
        value = analogRead(R_JOY_Y);
      }
      RY_PWM_OUTPUT = map(value, RY_LOW, RY_MIDDLE, PWM_LOW, PWM_MIDDLE) + 1;
    } else if (analogRead(R_JOY_Y) > RY_MIDDLE) {
      RY_PWM_OUTPUT = map(analogRead(R_JOY_Y), RY_MIDDLE, RY_HIGH, PWM_MIDDLE, PWM_HIGH) + 1;
    } else if (analogRead(R_JOY_Y) == RY_MIDDLE) {
      RY_PWM_OUTPUT = PWM_MIDDLE;
    }

    //SEND_DATA
    if (digitalRead(MODE_SWITCH) == 1) {
      if(millis() - button_timer > 300 ){ 
        send_data.MODE++;
        button_timer = millis();
      }
    }
    if(send_data.MODE > 1){
      send_data.MODE = 0;
    }else if (send_data.MODE < 0){
      send_data.MODE = 1;
    }
    send_data.MODE = constrain(send_data.MODE, 0, 1);

    send_data.THROTTLE = LY_PWM_OUTPUT;
    send_data.YAW = LX_PWM_OUTPUT;
    send_data.PITCH = RX_PWM_OUTPUT;
    send_data.ROLL = RY_PWM_OUTPUT;
    send_data.FUNC_1 = 1;
    send_data.FUNC_2 = 0;
    Data_send();

    //UI
    display.drawLine(32, 16, map(LX_PWM_OUTPUT, PWM_HIGH, PWM_LOW, 48, 16), map(LY_PWM_OUTPUT, PWM_LOW, PWM_HIGH, 32, 0), WHITE);
    display.drawLine(96, 16, map(RX_PWM_OUTPUT, PWM_LOW, PWM_HIGH, 80, 112), map(RY_PWM_OUTPUT, PWM_HIGH, PWM_LOW, 32, 0), WHITE);

    if (send_data.MODE == 0){
      mode_name = "Stabilize";
    }else if (send_data.MODE == 1){
      mode_name = "AltHold";
    }
    display.setTextSize(1);
    display.setCursor(60,0);
    display.print(mode_name);
    display.display();

    //break
    if (digitalRead(FUNCB_U) == 1) {
      send_data.FUNC_2 = 1;
      send_data.Test = 0;
      Data_send();
    }
    if (digitalRead(FUNCB_L) == 1) {
      send_data.FUNC_2 = 1;
      send_data.Test = 0;
      Data_send();
      break;
    }
  }
}
