#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"

Adafruit_SSD1306 display(128, 32, &Wire, 4);

#define Y A6
#define X A7
#define BAT A2

RF24 radio(9, 10);
const uint64_t pipe = 0xE8E8F0F1A1LL ;

unsigned long select_timer;
unsigned long motor_timer;
unsigned long BAT_timer;

int page = 0;
int select = 0;
int select_last, page_last;

float BAT_V;

String FRAME[4] = {"Messege", "Schedule", "Setting", ""};

typedef struct {

  int RC_INPUT[5];
  
  int CH_OUTPUT[5];

  int VOLTAGE;
  
  int OPENMV_STATUS;

  int MODE;

  int STATUS_CODE;

} RECEIVE;

RECEIVE RECEIVE_DATA ;

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  // put your setup code here, to run once:
  pinMode(BAT, INPUT);
  BAT_V = float(analogRead(BAT) * 0.0052125);

  display.fillScreen(WHITE);
  display.display();

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(1);
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(1, pipe);
  radio.setCRCLength(RF24_CRC_8);
  radio.setAutoAck(0);
  radio.startListening();

  select_timer = millis();
  motor_timer = millis();
  BAT_timer = millis();

}

void loop() {
  menu();

}

void menu() {
  while (1) {
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
      display.setCursor(0,8);
      display.setTextSize(2);
      display.print("Receive");
    } else if (select == 1) {
      display.setCursor(0,8);
      display.setTextSize(2);
      display.print("Setting");
    } else if (select == 2) {
      display.setCursor(0,8);
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
  while (1) {
    if (radio.available()) {
      while (radio.available()) {
        radio.read(&RECEIVE_DATA , sizeof(RECEIVE_DATA));
      }
    }

      display.clearDisplay();
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
    
    /*
    display.drawLine(32, 16, map(RECEIVE_DATA.CH_OUTPUT[3], 1912, 1092, 48, 16), map(RECEIVE_DATA.CH_OUTPUT[2], 1092, 1912, 32, 0), WHITE);
    display.drawLine(96, 16, map(RECEIVE_DATA.CH_OUTPUT[0], 1092, 1912, 80, 112), map(RECEIVE_DATA.CH_OUTPUT[1], 1912, 1092, 32, 0), WHITE);
    display.setCursor(40, 24);
    display.setTextColor(WHITE);
    display.println(RECEIVE_DATA.OPENMV_STATUS);
    */
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

}
void SETTING() {

}
