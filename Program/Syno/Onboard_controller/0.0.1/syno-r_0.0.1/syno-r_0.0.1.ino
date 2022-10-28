#include <Wire.h>
#include <SPI.h>

#include <LiquidCrystal_I2C.h>
#include <SD.h>

#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"

//nRF24L01 SETUP
RF24 radio(7, 8);
const uint64_t pipe[2] = { 0xE8E8F0F0A1LL, 0xE8E8F0F0A2LL } ;
unsigned long timer ;

boolean GET_SIGNAL;

typedef struct {

  int RC_INPUT[6];

  int CH_OUTPUT[5];

  int voltage;
  int gimbal_angle[2];

  int OpenMV_status;

} READ_DATA;

READ_DATA read_data;

//SD SETUP
int cspin = 3;


/*
  [SD Library]
  SD  Class
  •  初始設定 SD卡及函式庫：SD.begin(cspin)
    cspin (選項): Arduino 連接SD卡模組 SS 或 CS的 Pin腳

  •  檢查括號內的檔案名稱是否存在：exists()

  •  建立目錄：SD.mkdir(filename)

  •  開啟檔案：SD.open(filepath, mode)
   mode : FILE_READ  開啟唯讀檔案，從檔案開始處讀取.
               FILE_WRITE 開啟讀寫檔案，從檔案最後處開始寫入

  •  移除檔案：SD.remove(filename)

  •  移除目錄：SD.rmdir(filename)

  File  Class
  •  關閉檔案：file.close()

  •  將data 字串寫到檔案中，不跳行：file.print(data)

  •  將data 字串寫到檔案中，不跳行：file.println(data)

  •  將 data 字串寫入檔案中：file.write(data)
*/


void setup() {
  SD.begin(cspin);

  int i = 0;
  while (1) {
    char file_name = "%u.txt", i;
    if (!(SD.exists(file_name))) {
      SD.open(file_name, FILE_WRITE);
      break;
    }
  }

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(1);
  radio.setDataRate(RF24_2MBPS);//MAX LENGTH IS 0.25MB/S
  radio.setCRCLength(RF24_CRC_8);
  radio.openReadingPipe(pipe[1], 1);
  radio.setAutoAck(0);
  radio.startListening();

  Serial.begin(9600);
  timer = millis();
}

void loop() {
  RF24_READ();

  if (GET_SIGNAL) {
    timer = millis();
  }

  if ( millis() - timer < 1000) {
    if (GET_SIGNAL) {
      Serial.print(read_data.RC_INPUT[0]);
      Serial.print(read_data.RC_INPUT[1]);
      Serial.print(read_data.RC_INPUT[2]);
      Serial.print(read_data.RC_INPUT[3]);
      Serial.print(read_data.RC_INPUT[4]);
      Serial.print(read_data.RC_INPUT[5]);

      Serial.print(read_data.CH_OUTPUT[0]);
      Serial.print(read_data.CH_OUTPUT[1]);
      Serial.print(read_data.CH_OUTPUT[2]);
      Serial.print(read_data.CH_OUTPUT[3]);
      Serial.print(read_data.CH_OUTPUT[4]);

      Serial.print(read_data.voltage);

      Serial.print(read_data.gimbal_angle[0]);
      Serial.print(read_data.gimbal_angle[1]);

      Serial.print(read_data.OpenMV_status);
    }
  } else {
    Serial.print(F("TIME_OUT"));
  }

}

void RF24_READ() {

  radio.startListening();

  if (radio.available()) {
    radio.read(&read_data, sizeof(&read_data));
    GET_SIGNAL = 1;
  } else {
    GET_SIGNAL = 0;
  }

}
