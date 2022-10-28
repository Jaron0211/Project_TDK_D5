#include <LiquidCrystal_I2C.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"

Adafruit_SSD1306 display(128, 32, &Wire, 4);
LiquidCrystal_I2C lcd(0x3f, 128, 32);

#define Y A6
#define X A7
#define BAT A2
#define light A0

RF24 radio(9, 10);
const uint64_t pipe = 0xE8E8F0F1A1LL ;

unsigned long select_timer;
unsigned long motor_timer;
unsigned long BAT_timer;

int page = 0;
int select = 0;
int select_last, page_last;

int light_strong;
float BAT_V;

String FRAME[4] = {"Messege", "Schedule", "Setting", ""};
String status[5] = {"LOSE","MANU","AUTO","PRE","ERROR"};

typedef struct {

  int CH_OUTPUT[5];

  int VOLTAGE;

  int MODE;

  int STATUS_CODE;

} RECEIVE;

RECEIVE RECEIVE_DATA ;

int LX_LOW = 0, LX_MIDDLE = 515 , LX_HIGH = 1023;
int LY_LOW = 1023, LY_MIDDLE = 527 , LY_HIGH = 0;
int RX_LOW = 0, RX_MIDDLE = 497 , RX_HIGH = 1023;
int RY_LOW = 0, RY_MIDDLE = 510 , RY_HIGH = 1023;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.fillScreen(WHITE);
  display.display();

  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(1);
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(1, pipe);
  //radio.setCRCLength(RF24_CRC_8);
  radio.setAutoAck(0);
  radio.startListening();


  pinMode(BAT, INPUT);
  BAT_V = float(analogRead(BAT) * 0.0052125);

  pinMode(light,INPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  select_timer = millis();
  motor_timer = millis();
  BAT_timer = millis();

}

void loop() {
  menu();

}

void menu() {
  while (1) {
    lcd.clear();
    display.clearDisplay();
    if (millis() - BAT_timer > 5000) {
      BAT_V = float(analogRead(BAT) * 0.0053823);
      BAT_timer = millis();
    }

    if (millis() - select_timer > 120) {

      if (analogRead(Y) > 700) {
        motor_timer = millis();
        select -= 1;
      } else if (analogRead(Y) < 300) {
        motor_timer = millis();
        select += 1;
      }

      select = constrain(select , 0 , 2);
      select_timer = millis();

    }
    if (millis() - motor_timer < 70) {
      analogWrite(3, 120);
    } else {
      analogWrite(3, 0);
    }

    select_last = select;
    page_last = page;

    SELECT_OPTION();
    Serial.print(analogRead(X));
    Serial.print(" ");
    Serial.println(analogRead(Y));


    display.setCursor(100, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print(BAT_V);

    if (select == 0) {
      display.setCursor(0, 8);
      display.setTextSize(2);
      display.print("Receive");
    } else if (select == 1) {
      display.setCursor(0, 8);
      display.setTextSize(2);
      display.print("Light");
    } else if (select == 2) {
      display.setCursor(0, 8);
      display.setTextSize(2);
      display.print(":)-<-<");
    }
    display.display();
  }
}

void SELECT_OPTION() {
  if (analogRead(X) < 300) {
    switch (page + select) {
      case 0:
        INFO_READ();
        break;
      case 1:
        MESSAGE();
        break;
      case 2:
        SETTING();
        break;
    }

  }
}


void INFO_READ() {
  display.clearDisplay();
  unsigned long lcd_timer = millis();
  while (1) {
    if (radio.available()) {
      while (radio.available()) {
        radio.read(&RECEIVE_DATA , sizeof(RECEIVE_DATA));
      }
    }

    display.clearDisplay();
    //display.drawLine(32, 16, map(LX_PWM_OUTPUT, PWM_HIGH, PWM_LOW, 48, 16), map(LY_PWM_OUTPUT, PWM_LOW, PWM_HIGH, 32, 0), WHITE);
    //display.drawLine(96, 16, map(RX_PWM_OUTPUT, PWM_LOW, PWM_HIGH, 80, 112), map(RY_PWM_OUTPUT, PWM_HIGH, PWM_LOW, 32, 0), WHITE);

    if (millis() - lcd_timer > 0) {
      lcd.setCursor(0, 0);
      lcd.print("MODE: ");
      lcd.print(status[RECEIVE_DATA.MODE]);
      lcd.print(" ");
      lcd.setCursor(0, 1);
      lcd.print("STATUS: ");
      lcd.print(RECEIVE_DATA.STATUS_CODE);
      lcd.print(" ");

      lcd.setCursor(0, 2);
      lcd.print(RECEIVE_DATA.CH_OUTPUT[0]);
      lcd.print(" ");
      lcd.setCursor(5, 2);
      lcd.print(RECEIVE_DATA.CH_OUTPUT[1]);
      lcd.print(" ");
      lcd.setCursor(10, 2);
      lcd.print(RECEIVE_DATA.CH_OUTPUT[2]);
      lcd.print(" ");
      lcd.setCursor(15, 2);
      lcd.print(RECEIVE_DATA.CH_OUTPUT[3]);
      lcd.print(" ");
      lcd_timer = millis();
    }


    display.drawLine(32, 16, map(RECEIVE_DATA.CH_OUTPUT[3], 1930, 1108, 48, 16), map(RECEIVE_DATA.CH_OUTPUT[2], 1110, 1930, 32, 0), WHITE);
    display.drawLine(96, 16, map(RECEIVE_DATA.CH_OUTPUT[0], 1110, 1930, 80, 112), map(RECEIVE_DATA.CH_OUTPUT[1], 1930, 1110, 32, 0), WHITE);
    display.setCursor(40, 24);
    display.setTextColor(WHITE);
    //display.println(RECEIVE_DATA.OPENMV_STATUS);

    display.display();

    if (analogRead(X) > 700) {
      motor_timer =  millis();
      break;
    }

  }
}
void SCHEDULE() {

}
void MESSAGE() {
  display.clearDisplay();
  unsigned long lcd_timer = millis();
  while (1) {

    display.clearDisplay();
    //display.drawLine(32, 16, map(LX_PWM_OUTPUT, PWM_HIGH, PWM_LOW, 48, 16), map(LY_PWM_OUTPUT, PWM_LOW, PWM_HIGH, 32, 0), WHITE);
    //display.drawLine(96, 16, map(RX_PWM_OUTPUT, PWM_LOW, PWM_HIGH, 80, 112), map(RY_PWM_OUTPUT, PWM_HIGH, PWM_LOW, 32, 0), WHITE);

    light_strong = light_strong * 0.9 + map(analogRead(light),0,1023,0,display.width()) * 0.1 ;
    display.setCursor(40, 0);
    display.setTextColor(WHITE);
    if(50<light_strong and light_strong<70){
      display.println("OK");
    }else if(50>light_strong){
      display.setCursor(20, 0);
      display.println("TOO LOW");
    }else if(70<light_strong){
      display.setCursor(2 0, 0);
      display.println("TOO HIGH");
    }
    display.fillRect(5, 20, light_strong, 15, WHITE);

    display.display();

    if (analogRead(X) > 700) {
      motor_timer =  millis();
      break;
    }

  }
}
void SETTING() {

}
