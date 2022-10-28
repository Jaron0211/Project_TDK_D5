#include <Wire.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include <SPI.h>
#include <Servo.h>
#include <Pixy2I2C.h>
#include "component.h"
#include "define.h"

#define FL 9
#define FR 6
#define BL 5
#define BR 3

#define G A0
#define B A1
#define R A2


void setup() {
  analogWrite(R, 255);
  pixy.init();
  analogWrite(R, 0);

  analogWrite(B, 255);
  Serial.begin(2000000);
  Serial.println(F("check wire"));
  Wire.begin();
  analogWrite(B, 0);

  CH1.attach(9);
  CH2.attach(6);
  CH3.attach(5);
  CH4.attach(3);
  CH5.attach(7, 1000, 1800);
  THROW_CH.attach(2);
  STAND_CH.attach(4);

  THROW_CH.write(10);

  CH1.writeMicroseconds(900);
  CH2.writeMicroseconds(900);
  CH3.writeMicroseconds(900);
  CH4.writeMicroseconds(900);

  Serial.println(F("check rf24"));
  analogWrite(G, 255);
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(1);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_8);
  radio.openReadingPipe(0, pipe[1]);
  radio.setAutoAck(0);
  radio.startListening();
  analogWrite(G, 0);

  Serial.println(F("check pixy2_color"));
  led_timer = millis();

  stand_check = 1;

}

void loop_1() {
  LINE_RIGHT_ANGLE_AND_STRAGHT();
}

void loop() {
  THROW_CH.write(10);
  Data_receive();
  CH1.writeMicroseconds(1400);
  CH2.writeMicroseconds(1400);
  CH3.writeMicroseconds(1000);
  CH4.writeMicroseconds(1400);

  //LED
  if (RECEIVER_CHECK < 1500) {
    CH5.writeMicroseconds(1800);
    analogWrite(G, 0);
    led_timer = millis();
    Serial.println(RECEIVER_CHECK);
  } else {
    CH5.writeMicroseconds(1800);
    unsigned long count = millis() - led_timer;
    if (1300 > count && count > 1200) {
      analogWrite(G, 255);
    } else if (1350 > count && count > 1300) {
      analogWrite(G, 0);
    } else if (1400 > count && count > 1350) {
      analogWrite(G, 255);
    } else if (count > 1450) {
      analogWrite(G, 0);
      led_timer = millis();
    }
  }

  if (receive_data.FUNC_1 == 1) {
    FLY_APM();
  } else if (receive_data.FUNC_1 == 2) {
    AUTO();
  } else if (receive_data.FUNC_1 == 3) {
    PART_TEST();
  }
}

void AUTO() {
  TAKE_OFF_CHECK = 0;
  //TAKE_OFF
  Serial.println(F("Auto"));
  while (1) {
    Serial.println(F("take_off"));
    Data_receive();
    if (receive_data.FUNC_1 == 2 && receive_data.FUNC_2 != 10 && TAKE_OFF_CHECK == 0) {
      TAKE_OFF();
    } else {
      //TAKE_OFF_CHECK = 0;
      break;
    }
  }
  //PART1
  PART1_CHECK  = 0;
  while (1) {
    Data_receive();
    if (receive_data.FUNC_1 == 2 && receive_data.FUNC_2 != 10  && TAKE_OFF_CHECK == 1 && PART1_CHECK == 0) {
      PART1();
    } else {
      //PART1_CHECK  = 0;
      break;
    }
  }
  //PART2
  PART2_CHECK = 0;
  while (1) {
    Data_receive();
    if (receive_data.FUNC_1 == 2 && receive_data.FUNC_2 != 10  && PART1_CHECK == 1 && PART2_CHECK == 0) {
      PART2();
    } else {
      //PART2_CHECK  = 0;
      break;
    }
  }
  //PART3
  PART3_CHECK  = 0;
  while (1) {
    Data_receive();
    if (receive_data.FUNC_1 == 2 && receive_data.FUNC_2 != 10  && PART2_CHECK == 1 && PART3_CHECK == 0) {
      PART3();
    } else {
      //PART3_CHECK  = 0;
      break;
    }
  }
  //PART4
  PART4_CHECK  = 0;
  while (1) {
    Data_receive();
    if (receive_data.FUNC_1 == 2 && receive_data.FUNC_2 != 10  && PART3_CHECK == 1 && PART4_CHECK == 0) {
      PART4();
    } else {
      //PART4_CHECK  = 0;
      break;
    }
  }
  while (1) {
    Data_receive();
    if (receive_data.FUNC_1 == 2 && receive_data.FUNC_2 != 10 && PART4_CHECK == 1) {
      CH1.writeMicroseconds(1400);
      CH2.writeMicroseconds(1400);
      CH3.writeMicroseconds(1000);
      CH4.writeMicroseconds(1400);
      CH5.writeMicroseconds(1800);
    } else {
      break;
    }
  }
}

void TAKE_OFF() {
  Serial.println(F("TAKE_OFF"));
  //ARM
  if (TAKE_OFF_CHECK == 0) {
    timer = millis();
  }
  while (1) {
    analogWrite(R, 255);
    Data_receive();
    if (receive_data.FUNC_1 != 2 or receive_data.FUNC_2 == 10 ) {
      Serial.println(F("BREAK"));
      break;
    }
    Serial.print(F("ARMED"));
    CH1.writeMicroseconds(1400);
    CH2.writeMicroseconds(1400);
    CH3.writeMicroseconds(1000);
    CH4.writeMicroseconds(1790);
    CH5.writeMicroseconds(1400);
    Serial.println(millis() - timer);

    if (millis() - timer > 6000) {
      break;
    }
  }
  //READY TO TAKE OFF
  if (TAKE_OFF_CHECK == 0) {
    timer = millis();
  }
  while (1) {
    analogWrite(R, 255);
    Data_receive();
    if (receive_data.FUNC_1 != 2 or receive_data.FUNC_2 == 10) {
      break;
    }
    Serial.print(F("WARN UP "));
    CH1.writeMicroseconds(1400);
    CH2.writeMicroseconds(1400);
    CH3.writeMicroseconds(1000);
    CH4.writeMicroseconds(1400);
    CH5.writeMicroseconds(1400);
    Serial.println(millis() - timer);

    if (millis() - timer > 1000) {
      break;
    }
  }
  if (TAKE_OFF_CHECK == 0) {
    timer = millis();
  }
  while (1) {
    analogWrite(R, 0);
    analogWrite(G, 255);
    Data_receive();
    if (receive_data.FUNC_1 != 2 or receive_data.FUNC_2 == 10) {
      break;
    }
    Serial.print(F("PREPARE"));
    LINE_DETECT();
    CH1.writeMicroseconds(1400);
    CH2.writeMicroseconds(1400);
    CH3.writeMicroseconds(1400);
    CH4.writeMicroseconds(1400);
    CH5.writeMicroseconds(1400);
    Serial.println(millis() - timer);

    if (millis() - timer > 5000) {
      break;
    }
  }
  //TAKE OFF
  if (TAKE_OFF_CHECK == 0) {
    timer = millis();
  }
  while (1) {
    analogWrite(G, 0);
    analogWrite(B, 255);
    Data_receive();
    if (receive_data.FUNC_1 != 2 or receive_data.FUNC_2 == 10) {
      break;
    }

    LINE_DETECT();

    CH1.writeMicroseconds(1400);
    CH2.writeMicroseconds(1400);
    CH3.writeMicroseconds(1510);
    if (abs(pixy_angle) < 40) {
      CH4.writeMicroseconds(1400 - (pixy_angle * LINE_DIRECTION_VALUE * 1.2));
    } else {
      CH4.writeMicroseconds(1400);
    }
    CH5.writeMicroseconds(1400);

    Serial.print(F("TAKE OFF"));
    Serial.println(millis() - timer);

    if ((millis() - timer > 2500)) {
      break;
    }
  }
  //stable

  //get out circle
  if (TAKE_OFF_CHECK == 0) {
    stand_check = 1;
    timer = millis();
  }
  while (1) {
    analogWrite(B, 0);
    analogWrite(R, 0);
    analogWrite(G, 255);
    Data_receive();

    if (receive_data.FUNC_1 != 2 or receive_data.FUNC_2 == 10) {
      break;
    }
    LINE_DETECT();
    CH1.writeMicroseconds(1400);
    CH2.writeMicroseconds(1360);
    CH3.writeMicroseconds(1400);
    CH4.writeMicroseconds(1400);
    CH5.writeMicroseconds(1400);
    Serial.print(F("OUT OF CIRCLE"));
    stand_close();
    Serial.println(millis() - timer);
    if (millis() - timer > 500) {
      analogWrite(B, 0);
      TAKE_OFF_CHECK = 1;
      break;
    }
  }
}

//起飛區到顏色辨識區
void PART1() {
  analogWrite(G, 0);
  analogWrite(B, 0);
  Serial.println(F("part1"));
  break_stater_timer = millis();
  //FOLLOW LINE TO THE COLOR DETECT AREA
  while (1) {
    Data_receive();
    if (receive_data.FUNC_1 != 2 or receive_data.FUNC_2 == 10 or PART1_CHECK != 0  ) {
      if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 or receive_data.Test != 1) {
        Serial.print(receive_data.FUNC_1);
        Serial.print(receive_data.FUNC_2);
        Serial.println(PART1_CHECK);
        Serial.println(F("part1 break"));
        break;
      }
    }
    Serial.println(F("FOLLOWING LINE"));
    LINE_DETECT();
    CCC_DETECT_COLOR_WAITING();

    if ( millis() - break_stater_timer > 2500) {
      if (CCC_ATTACH_CHECK == 0) {
        CCC_ATTACH_TIMER = millis();
      }
      //CHECK FOR ONE SECONDS
      if (millis() - CCC_ATTACH_TIMER > 400) {
        Serial.println(F("ATTACH,NEXT LEVEL"));
        PART1_CHECK = 1;
        break;
      }
    }
  }
}
//顏色記憶&等待
void PART2() {
  analogWrite(R, 0);
  analogWrite(G, 0);
  analogWrite(B, 0);
  break_timer = millis();
  while (1) {
    analogWrite(B, 255);
    Data_receive();
    if (receive_data.FUNC_1 != 2 or receive_data.FUNC_2 == 10 or PART2_CHECK != 0) {
      if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 or receive_data.Test != 2) {
        break;
      }
    }
    Serial.println(F("break"));
    LINE_FOLLOW_BREAK();

    if (millis() - break_timer > 700) {
      analogWrite(B, 0);
      break;
    }
  }
  //WAITING UNTIL COLOR CHANGE(REMENBER IN VALUE "CCC_ATTACH_COLOR" ,1 = B ; 2 = G)
  analogWrite(R, 0);
  analogWrite(G, 0);
  analogWrite(B, 0);
  while (1) {
    Data_receive();
    if (receive_data.FUNC_1 != 2 or receive_data.FUNC_2 == 10 or PART2_CHECK != 0) {
      if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 or receive_data.Test != 2) {
        break;
      }
    }
    Serial.println(F("WAITING FOR COLOR CHANGE"));
    CCC_WAITING();

    if (COLOR_READ_CHECK) {
      Serial.print(F("GET COLOR : "));
      Serial.println(CCC_ATTACH_COLOR);
      PART2_CHECK = 1;
      analogWrite(R, 0);
      analogWrite(G, 0);
      analogWrite(B, 0);
      break;
    }
  }
}
//顏色辨識後尋跡&丟沙包 and 尋跡到降落的天殺的參考點
void PART3() {
  analogWrite(R, 0);
  analogWrite(G, 0);
  analogWrite(B, 0);
  THROW_STARTER_TIMER = millis();
  turn_over = 0;
  turn_over_2 = 0;
  while (1) {
    Data_receive();
    if (receive_data.FUNC_1 != 2 or receive_data.FUNC_2 == 10 or PART3_CHECK != 0) {
      if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 or receive_data.Test != 3 or CCC_ATTACH_COLOR != 1) {
        if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 or receive_data.Test != 4 or CCC_ATTACH_COLOR != 2) {
          break;
        }
      }
    }

    //還要加入丟完沙包後的轉彎直角與到降落區的辨識
    if (!turn_over) {
      Serial.println("Right turn 1");
      LINE_RIGHT_ANGLE_AND_STRAGHT();
    } else {
      Serial.println(F("THROW AREA FINDING"));
      LINE_FOLLOW();
      if ( millis() - THROW_STARTER_TIMER > 2000 ) {
        if (!THROW_CHECK) {
          COLOR_DETECT();
          //BLUE
          if (CCC_ATTACH_COLOR == 1) {
            if (b >= 250 and r < 250 and g < 250) {
              Serial.print(millis() - COLOR_READ_TIMER);
              Serial.print(F("FIND BLUE AREA"));
              stand_open();
              if (millis() - COLOR_READ_TIMER > 600) {
                THROW();
              }
            } else {
              COLOR_READ_TIMER = millis();
            }
            //green
          } else if (CCC_ATTACH_COLOR == 2) {
            if (g >= 250 and r < 250 and b < 250) {
              Serial.print(millis() - COLOR_READ_TIMER);
              Serial.print(F("FIND GREEN AREA"));
              stand_open();
              if (millis() - COLOR_READ_TIMER > 600) {
                THROW();
              }
            } else {
              COLOR_READ_TIMER = millis();
            }
          }
        } else if (THROW_CHECK) {
          if (millis() - COLOR_READ_TIMER > 4000) {
            stand_close();
            THROW_CH.write(10);
          }
          if(turn_over_2 == 3){
            Serial.println(F("FINDING LANDING AREA"));
            CCC_DETECT_COLOR_FINDER();
            if (CCC_ATTACH_CHECK == 0) {
              CCC_ATTACH_TIMER = millis();
            }
            if (millis() - CCC_ATTACH_TIMER > 400) {
              Serial.println(F("FIND LANDING POINT"));
              PART3_CHECK = 1;
              break;
            }
          }else{
            Serial.print(F("Right angle detect: "));
            Serial.println(turn_over_2);
            LINE_RIGHT_ANGLE_AND_STRAGHT();
          }
        }
      }
    }
  }
}
//降落區
void PART4() {
  analogWrite(R, 0);
  analogWrite(G, 0);
  analogWrite(B, 0);
  //STABLE ON THE RED POINT
  Serial.println(F("LAND"));
  if (PART4_CHECK == 0) {
    timer = millis();
  }
  while (1) {
    Data_receive();
    if (receive_data.FUNC_1 != 2 or receive_data.FUNC_2 == 10 or PART4_CHECK != 0) {
      if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 or receive_data.Test != 5 or CCC_ATTACH_COLOR != 1) {
        if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 or receive_data.Test != 6 or CCC_ATTACH_COLOR != 2) {
          break;
        }
      }
    }
    stand_close();

    CH1.writeMicroseconds(1400);
    CH2.writeMicroseconds(1360);
    CH3.writeMicroseconds(1400);
    CH4.writeMicroseconds(1400);
    CH5.writeMicroseconds(1400);
    analogWrite(R, 255);
    Serial.print(millis() - timer);
    Serial.print(F("CHECKING MIDDLE POINT"));
    if (millis() - timer > 300) {
      break;
    }
  }
  //GO LANDING ON RIGHT COLOR
  if (PART4_CHECK == 0) {
    timer = millis();
  }
  while (1) {
    Data_receive();
    if (receive_data.FUNC_1 != 2 or receive_data.FUNC_2 == 10 or PART4_CHECK != 0) {
      if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 or receive_data.Test != 5 or CCC_ATTACH_COLOR != 1) {
        if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 or receive_data.Test != 6 or CCC_ATTACH_COLOR != 2) {
          break;
        }
      }
    }
    //blue
    if (CCC_ATTACH_COLOR == 1) {
      CH1.writeMicroseconds(1450);
      CH2.writeMicroseconds(1400);
      CH3.writeMicroseconds(1400);
      CH4.writeMicroseconds(1400);
      CH5.writeMicroseconds(1400);
      Serial.print(millis() - timer);
      analogWrite(B, 255);
      analogWrite(R, 0);
      analogWrite(G, 0);
      Serial.println(F("MOVE TO THE BLUE"));
      if (millis() - timer > 1000) {
        break;
      }
      //green
    } else if (CCC_ATTACH_COLOR == 2) {
      CH1.writeMicroseconds(1350);
      CH2.writeMicroseconds(1400);
      CH3.writeMicroseconds(1400);
      CH4.writeMicroseconds(1400);
      CH5.writeMicroseconds(1400);
      analogWrite(B, 0);
      analogWrite(R, 0);
      analogWrite(G, 255);
      Serial.print(millis() - timer);
      Serial.println(F("MOVE TO THE GREEN"));
      if (millis() - timer > 1000) {
        break;
      }
    }
  }

  //LANDING
  if (PART4_CHECK == 0) {
    timer = millis();
  }
  while (1) {
    Data_receive();
    if (receive_data.FUNC_1 != 2 or receive_data.FUNC_2 == 10 or PART4_CHECK != 0) {
      if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 or receive_data.Test != 5 or CCC_ATTACH_COLOR != 1) {
        if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 or receive_data.Test != 6 or CCC_ATTACH_COLOR != 2) {
          break;
        }
      }
    }
    stand_open();
    CH1.writeMicroseconds(1400);
    CH2.writeMicroseconds(1400);
    CH3.writeMicroseconds(1300);
    CH4.writeMicroseconds(1400);
    CH5.writeMicroseconds(1800);
    Serial.print(millis() - timer);
    Serial.print(F("LANDING"));
    if (millis() - timer > 6000) {
      PART4_CHECK = 1;
      break;
    }
  }
  //DISARMED
  if (PART4_CHECK == 0) {
    timer = millis();
  }
  while (1) {
    Data_receive();
    if (receive_data.FUNC_1 != 2 or receive_data.FUNC_2 == 10 or PART4_CHECK != 0) {
      if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 or receive_data.Test != 5 or CCC_ATTACH_COLOR != 1) {
        if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 or receive_data.Test != 6 or CCC_ATTACH_COLOR != 2) {
          break;
        }
      }
    }
    stand_open();
    CH1.writeMicroseconds(1400);
    CH2.writeMicroseconds(1400);
    CH3.writeMicroseconds(1000);
    CH4.writeMicroseconds(1400);
    CH5.writeMicroseconds(1800);
    Serial.print(millis() - timer);
    Serial.print(F("LANDING"));
    if (millis() - timer > 8000) {
      PART4_CHECK = 1;
      break;
    }
  }

}

void PART_TEST() {
  Serial.println(F("Part_test"));
  analogWrite(R, 255);
  MISSION_OVER = 0;
  COLOR_READ_CHECK = 0;
  CCC_ATTACH_COLOR = 0;
  turn_over = 0;
  while (1) {
    Data_receive();
    if (receive_data.FUNC_1 != 3 or receive_data.FUNC_2 != 0 ) {
      analogWrite(R, 0);
      break;
    }
    if (MISSION_OVER == 1) {
      Serial.print(F("PASS"));
      THROW_CH.write(10);
      FLY_APM();
    } else if (MISSION_OVER == 0) {
      switch (receive_data.Test) {
        case 0 :
          Serial.print(F("Take_off"));
          TAKE_OFF();
          break;
        case 1 :
          Serial.print(F("Part1"));
          PART1();
          break;
        case 2 :
          Serial.print(F("Part2"));
          PART2();
          break;
        case 3 :
          Serial.print(F("Part3_B"));
          CCC_ATTACH_COLOR = 1;
          COLOR_READ_CHECK = 1;
          PART3();
          break;
        case 4 :
          Serial.print(F("Part3_G"));
          CCC_ATTACH_COLOR = 1;
          COLOR_READ_CHECK = 1;
          PART3();
          break;
        case 5 :
          Serial.print(F("Part4_B"));
          CCC_ATTACH_COLOR = 1;
          COLOR_READ_CHECK = 1;
          PART4();
          break;
        case 6 :
          Serial.print(F("Part4_G"));
          CCC_ATTACH_COLOR = 2;
          COLOR_READ_CHECK = 1;
          PART4();
          break;
      }
      MISSION_OVER = 1;
    }
  }
}

//combine function
void LINE_DETECT() {
  Wire.beginTransmission(0x53);
  Wire.write(0xae);
  Wire.write(0xc1);
  Wire.write(0x30);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.write(0x07);
  Wire.endTransmission();
  Wire.requestFrom(0x53, 14);

  if (Wire.available()) {
    uint16_t Head1 = Wire.read();
    uint16_t Head2 = Wire.read();
    uint8_t typr = Wire.read();
    uint8_t payload = Wire.read();
    uint8_t checksum1 = Wire.read() ;
    uint8_t checksum2 = Wire.read();
    if (typr == 49) {
      int i = 0;
      while (Wire.available()) {

        int8_t feature = Wire.read();
        int8_t length = Wire.read();
        x0 = Wire.read();
        y0 = Wire.read();
        x1 = Wire.read();
        y1 = Wire.read();
        int8_t data5 = Wire.read();
        int8_t data6 = Wire.read();

      }
      pixy_angle = atan((float)(y1 - y0) / (x1 - x0)) * 57.296;
      if (pixy_angle < 0) {
        pixy_angle = map(pixy_angle, -90, 0, 0, -90);
      } else if (pixy_angle > 0) {
        pixy_angle = map(pixy_angle, 90, 0, 0, 90);
      }
      pixy_middle = ((x1 + x0)  - 78) / 2;

      LINE_COUNTER = 0;
      if (pixy_middle < -100) {
        pixy_angle = 0;
        pixy_middle = 0;
        x0 = 39;
        x1 = 39;
        y0 = 26;
        y1 = 26;
        LINE_GET = 0;
        Serial.println(F("NO LINE"));
      } else {
        LINE_GET = 1;
        Serial.print(F("Line_middle: "));
        Serial.print(pixy_middle);
        Serial.print(F("  Line_angle: "));
        Serial.println(pixy_angle);
      }
    }
  }
}
void COLOR_DETECT() {
  pixy.changeProg("video");
  if (pixy.video.getRGB(pixy.frameWidth / 2, pixy.frameHeight / 2, &r, &g, &b) == 0)
  {
    Serial.print(F("red:"));
    Serial.print(r);
    analogWrite(R, r);
    Serial.print(F(" green:"));
    Serial.print(g);
    analogWrite(G, g);
    Serial.print(F(" blue:"));
    Serial.println(b);
    analogWrite(B, b);
  } else {
    r = 0;
    g = 0;
    b = 0;
  }
}
void CCC_DETECT_TAKE_OFF_POINT_OLD() {
  pixy.ccc.getBlocks();
  if (pixy.ccc.numBlocks) {

    Serial.print(F("Detected "));
    Serial.println(pixy.ccc.numBlocks);

    for (int i = 0; i < pixy.ccc.numBlocks; i++) {
      if (pixy.ccc.blocks[i].m_signature == 7) {
        CCC_ATTACH_CHECK = 1;
        pixy_ccc_x = map(pixy.ccc.blocks[i].m_x , 0, 315, -157, 157);
        pixy_ccc_y = map(pixy.ccc.blocks[i].m_y , 0, 207, -104, 104);
      }
    }
  } else {
    CCC_ATTACH_CHECK = 0;
    pixy_ccc_x = 0;
    pixy_ccc_y = 0;
  }
}
void CCC_DETECT_COLOR_WAITING() {
  pixy.ccc.getBlocks();
  if (pixy.ccc.numBlocks) {

    Serial.print(F("Detected "));
    Serial.println(pixy.ccc.numBlocks);

    for (int i = 0; i < pixy.ccc.numBlocks; i++) {
      if (pixy.ccc.blocks[i].m_signature == 1) {
        analogWrite(R, 255);
        analogWrite(G, 0);
        analogWrite(B, 0);
        CCC_ATTACH_CHECK = 1;
        COLOR_READ_CHECK = 0;
        pixy_ccc_x = map(pixy.ccc.blocks[i].m_x , 0, 315, -158, 158);
        pixy_ccc_y = map(pixy.ccc.blocks[i].m_y , 0, 207, -104, 104);
      } else if (pixy.ccc.blocks[i].m_signature == 2) {
        analogWrite(R, 0);
        analogWrite(G, 0);
        analogWrite(B, 255);
        CCC_ATTACH_CHECK = 1;
        CCC_ATTACH_COLOR = 1;
        COLOR_READ_CHECK = 1;
        pixy_ccc_x = map(pixy.ccc.blocks[i].m_x , 0, 315, -158, 158);
        pixy_ccc_y = map(pixy.ccc.blocks[i].m_y , 0, 207, -104, 104);
      } else if (pixy.ccc.blocks[i].m_signature == 3) {
        analogWrite(R, 0);
        analogWrite(G, 255);
        analogWrite(B, 0);
        CCC_ATTACH_CHECK = 1;
        CCC_ATTACH_COLOR = 2;
        COLOR_READ_CHECK = 1;
        pixy_ccc_x = map(pixy.ccc.blocks[i].m_x , 0, 315, -158, 158);
        pixy_ccc_y = map(pixy.ccc.blocks[i].m_y , 0, 207, -104, 104);
      }
      Serial.print(F("X: "));
      Serial.print(pixy_ccc_x);
      Serial.print(F("Y: "));
      Serial.println(pixy_ccc_y);
    }
  } else {
    CCC_ATTACH_CHECK = 0;
    pixy_ccc_x = 0;
    pixy_ccc_y = 0;
  }
}
void CCC_DETECT_COLOR_FINDER() {
  COLOR_DETECT();
  if (r > 250 && g < 150 && b < 150) {
    CCC_ATTACH_CHECK = 1;
  } else {
    CCC_ATTACH_CHECK = 0;
  }

}

//new function for downside pixy2
void LINE_DETECT_D() {
  pixy.changeProg("line");
  pixy.line.getAllFeatures();
  line_num = 0;

  for ( int i = 0; i < pixy.line.numVectors; i++) {

    LINE_X0[i] = pixy.line.vectors[i].m_x0;
    LINE_Y0[i] = pixy.line.vectors[i].m_y0;
    LINE_X1[i] = pixy.line.vectors[i].m_x1;
    LINE_Y1[i] = pixy.line.vectors[i].m_y1;
    angle[i] = atan((float)(LINE_Y1[i] - LINE_Y0[i]) / (LINE_X1[i] - LINE_X0[i])) * 57.296;

    if (angle[i] < 0) {
      angle[i] = map(angle[i], -90, 0, 0, -90);
    } else if (angle[i] > 0) {
      angle[i] = map(angle[i], 90, 0, 0, 90);
    }

    line_num = i + 1;
    if (i == max_num + 1) {
      break;
    }
  }
}
void RIGHT_ANGLE_DETECT() {
  pixy.changeProg("line");
  pixy.line.getAllFeatures();
  start_x_count = 0;

  for ( int8_t i = 0; i <= pixy.line.numVectors; i++) {
    float value_x, value_y;
    value_x = float((pixy.line.vectors[i].m_x1 - pixy.line.vectors[i].m_x0)) / float(sqrt((pixy.line.vectors[i].m_x1 - pixy.line.vectors[i].m_x0) * (pixy.line.vectors[i].m_x1 - pixy.line.vectors[i].m_x0) + (pixy.line.vectors[i].m_y1 - pixy.line.vectors[i].m_y0) * (pixy.line.vectors[i].m_y1 - pixy.line.vectors[i].m_y0)));
    value_y = float((pixy.line.vectors[i].m_y1 - pixy.line.vectors[i].m_y0)) / float(sqrt((pixy.line.vectors[i].m_x1 - pixy.line.vectors[i].m_x0) * (pixy.line.vectors[i].m_x1 - pixy.line.vectors[i].m_x0) + (pixy.line.vectors[i].m_y1 - pixy.line.vectors[i].m_y0) * (pixy.line.vectors[i].m_y1 - pixy.line.vectors[i].m_y0)));

    X0[i] = pixy.line.vectors[i].m_x0;
    Y0[i] = pixy.line.vectors[i].m_y0;
    X1[i] = pixy.line.vectors[i].m_x1;
    Y1[i] = pixy.line.vectors[i].m_y1;
    Vx[i] = value_x;
    Vy[i] = value_y;
    num = i;
    if (i >= max_num) {
      break;
    }

  }

  for (int i = 0 ; i < num ; i++ ) {
    for (int j = num - 1 ; j > i; j -= 1 ) {
      cross_checker = float((Vx[i] * Vy[j]) - (Vy[i] * Vx[j])) ;
      if ( -0.7 > cross_checker or cross_checker > 0.7) {
        int compare_num = 0;

        if (sqrt((X0[i] - X0[j]) * (X0[i] - X0[j]) + ((Y0[i] - Y0[j]) * (Y0[i] - Y0[j]))) <= COMPARE_DIS) {
          compare_num += 1;
        }
        if (sqrt((X0[i] - X1[j]) * (X0[i] - X1[j]) + ((Y0[i] - Y1[j]) * (Y0[i] - Y1[j]))) <= COMPARE_DIS) {
          compare_num += 2;
        }
        if (sqrt((X1[i] - X0[j]) * (X1[i] - X0[j]) + ((Y1[i] - Y0[j]) * (Y1[i] - Y0[j]))) <= COMPARE_DIS) {
          compare_num += 4;
        }
        if (sqrt((X1[i] - X1[j]) * (X1[i] - X1[j]) + ((Y1[i] - Y1[j]) * (Y1[i] - Y1[j]))) <= COMPARE_DIS) {
          compare_num += 8;
        }

        num_2 = start_x_count;
        if (compare_num == 1) {
          start_x_count++;
          start_x[num_2] = X1[i];
          start_y[num_2] = Y1[i];
          middle_x[num_2] = (X0[i] + X0[j]) / 2;
          middle_y[num_2] = (Y0[i] + Y0[j]) / 2;
          end_x[num_2] = X1[j];
          end_y[num_2] = Y1[j];
          Serial.print(F("LINE: "));
          Serial.println(num_2);
          Serial.println("type 1");
          Serial.println(start_x_count);
        } else if (compare_num == 2) {
          start_x_count++;
          start_x[num_2] = X1[i];
          start_y[num_2] = Y1[i];
          middle_x[num_2] = (X0[i] + X1[j]) / 2;
          middle_y[num_2] = (Y0[i] + Y1[j]) / 2;
          end_x[num_2] = X0[j];
          end_y[num_2] = Y0[j];
          Serial.print(F("LINE: "));
          Serial.println(num_2);
          Serial.println("type 2");
          Serial.println(start_x_count);
        } else if (compare_num == 4) {
          start_x_count++;
          start_x[num_2] = X0[i];
          start_y[num_2] = Y0[i];
          middle_x[num_2] = (X1[i] + X0[j]) / 2;
          middle_y[num_2] = (Y1[i] + Y0[j]) / 2;
          end_x[num_2] = X1[j];
          end_y[num_2] = Y1[j];
          Serial.print(F("LINE: "));
          Serial.println(num_2);
          Serial.println("type 3");
          Serial.println(start_x_count);
        } else if (compare_num == 8) {
          start_x_count++;
          start_x[num_2] = X0[i];
          start_y[num_2] = Y0[i];
          middle_x[num_2] = (X1[i] + X1[j]) / 2;
          middle_y[num_2] = (Y1[i] + Y1[j]) / 2;
          end_x[num_2] = X0[j];
          end_y[num_2] = Y0[j];
          Serial.print(F("LINE: "));
          Serial.println(num_2);
          Serial.println("type 4");
          Serial.println(start_x_count);
        }
      }
    }
  }
  if(start_x_count > 0){
    for (int i = 0; i <= start_x_count-1 ; i++) {
      Serial.println("直角 " + String(i + 1));
      Serial.print("start: (" + String(start_x[i]) + " , " + String(start_y[i]) + " )");
      Serial.print("middle: (" + String(middle_x[i]) + " , " + String(middle_y[i]) + " )");
      Serial.println("end: (" + String(end_x[i]) + " , " + String(end_y[i]) + " )");
    }
  }
}
void LINE_DOUBLE_CHECK() {

  //line detect
  LINE_DETECT();

  if (LINE_GET == 1) {
    LINE_GET_TIMER = millis();
    //averge filter
    averge_middle[averge_component - 1] = averge_middle[averge_component];
    averge_middle[averge_component] = pixy_middle;

    int line_middle_filte = 0;

    for (int i = 1 ; i <= averge_component ; i++) {
      line_middle_filte += averge_middle[i];
    }
    line_middle_filte = line_middle_filte / averge_component ;

    LINE_Comparison[0] = x0;
    LINE_Comparison[1] = y0;
    LINE_Comparison[2] = x1;
    LINE_Comparison[3] = y1;
    LINE_Comparison[4] = pixy_angle;
    LINE_Comparison[5] = line_middle_filte;

  } else if (LINE_GET == 0) {
    LINE_DETECT_D();

    Serial.println(F("front lost,downside detect"));
    int b_origin = LINE_Comparison[3] - (LINE_Comparison[2]) * ((LINE_Comparison[2] - LINE_Comparison[0]) / (LINE_Comparison[3] - LINE_Comparison[1]));
    Serial.println(line_num);
    for (int i = 0 ; i < line_num ; i++) {
      Serial.println(LINE_Comparison [0]);
      Serial.println(LINE_Comparison [1]);
      Serial.println(LINE_Comparison [2]);
      Serial.println(LINE_Comparison [3]);
      Serial.println(LINE_Comparison [4]);
      Serial.println(LINE_Comparison [5]);
      Serial.println(b_origin);
      Serial.println(abs(b_origin - (LINE_Y1[i] - (LINE_X1[i] * (LINE_X1[i] - LINE_X0[i]) / (LINE_Y1[i] - LINE_Y0[i])))));
      if (abs(pixy_angle - angle[i]) < detect_angle) {
        if (abs(b_origin - (LINE_Y1[i] - (LINE_X1[i] * (LINE_X1[i] - LINE_X0[i]) / (LINE_Y1[i] - LINE_Y0[i])))) < detect_distance) {

          LINE_Comparison [0] = LINE_X0[i];
          LINE_Comparison [1] = LINE_Y0[i];
          LINE_Comparison [2] = LINE_X1[i];
          LINE_Comparison [3] = LINE_Y1[i];
          LINE_Comparison [4] = angle[i];
          LINE_Comparison [5] = ((LINE_X1[i] + LINE_X0[i]) - 78) / 2;

          averge_middle[averge_component - 1] = averge_middle[averge_component];
          averge_middle[averge_component] =  LINE_Comparison[5];

          break;
        }
      }
    }
    Serial.print( LINE_Comparison [5]);
    Serial.print(F(" "));
    Serial.println(LINE_Comparison [4]);
    if (millis() - LINE_GET_TIMER  > 50) {

      if (LINE_Comparison [5] > 0) {
        LINE_Comparison [5] -= 1;
      } else if (LINE_Comparison [5] < 0) {
        LINE_Comparison [5] += 1;
      }

      if (LINE_Comparison [4] > 0) {
        LINE_Comparison [4] -= 1;
      } else if (LINE_Comparison [4] < 0) {
        LINE_Comparison [4] += 1;
      }

      LINE_GET_TIMER = millis();
    }
  }

  //PID

  LINE_DETECT_VALUE_DIF = LINE_DETECT_VALUE_PRE - LINE_Comparison[5] ;
  LINE_DETECT_VALUE_PRE = LINE_Comparison[5];

  LINE_DETECT_VALUE_INTE += LINE_Comparison[5];

  LINE_PID_VALUE = (LINE_Comparison[5] * LINE_DETECT_VALUE_P) + (LINE_DETECT_VALUE_DIF * LINE_DETECT_VALUE_D) + (LINE_DETECT_VALUE_INTE * LINE_DETECT_VALUE_I);

  CH1.writeMicroseconds(1400 + LINE_PID_VALUE * sin(LINE_Comparison [4]) - 40 * cos(LINE_Comparison [4]));
  CH2.writeMicroseconds(1400 - 40 * cos(LINE_Comparison [4]) + LINE_PID_VALUE * sin(LINE_Comparison [4]));
  CH3.writeMicroseconds(1400);
  CH4.writeMicroseconds(1400 - (LINE_Comparison [4] * LINE_DIRECTION_VALUE));
  CH5.writeMicroseconds(1400);

  Serial.print(1400 + LINE_PID_VALUE * sin(LINE_Comparison [4]));
  Serial.print(F("  "));
  Serial.println(1400 - 40 * cos(LINE_Comparison [4]));

}
void LINE_RIGHT_ANGLE_AND_STRAGHT_OLD() {
  //line detect
  LINE_DETECT();
  LINE_DETECT_D();
  start_x_count = 0;

  for ( int8_t i = 0; i < pixy.line.numVectors; i++) {
    float value_x, value_y;
    value_x = float((pixy.line.vectors[i].m_x1 - pixy.line.vectors[i].m_x0)) / float(sqrt((pixy.line.vectors[i].m_x1 - pixy.line.vectors[i].m_x0) * (pixy.line.vectors[i].m_x1 - pixy.line.vectors[i].m_x0) + (pixy.line.vectors[i].m_y1 - pixy.line.vectors[i].m_y0) * (pixy.line.vectors[i].m_y1 - pixy.line.vectors[i].m_y0)));
    value_y = float((pixy.line.vectors[i].m_y1 - pixy.line.vectors[i].m_y0)) / float(sqrt((pixy.line.vectors[i].m_x1 - pixy.line.vectors[i].m_x0) * (pixy.line.vectors[i].m_x1 - pixy.line.vectors[i].m_x0) + (pixy.line.vectors[i].m_y1 - pixy.line.vectors[i].m_y0) * (pixy.line.vectors[i].m_y1 - pixy.line.vectors[i].m_y0)));

    X0[i] = pixy.line.vectors[i].m_x0;
    Y0[i] = pixy.line.vectors[i].m_y0;
    X1[i] = pixy.line.vectors[i].m_x1;
    Y1[i] = pixy.line.vectors[i].m_y1;
    Vx[i] = value_x;
    Vy[i] = value_y;

    LINE_X0[i] = pixy.line.vectors[i].m_x0;
    LINE_Y0[i] = pixy.line.vectors[i].m_y0;
    LINE_X1[i] = pixy.line.vectors[i].m_x1;
    LINE_Y1[i] = pixy.line.vectors[i].m_y1;
    angle[i] = atan((float)(LINE_Y1[i] - LINE_Y0[i]) / (LINE_X1[i] - LINE_X0[i])) * 57.296;

    num = i;
    line_num = i + 1;
    if (i >= max_num) {
      break;
    }

  }

  for (int i = 0 ; i < num ; i++ ) {
    for (int j = num - 1 ; j > i; j -= 1 ) {
      cross_checker = float((Vx[i] * Vy[j]) - (Vy[i] * Vx[j])) ;
      if ( -0.8 > cross_checker or cross_checker > 0.8) {
        int compare_num = 0;

        if (sqrt((X0[i] - X0[j]) * (X0[i] - X0[j]) + ((Y0[i] - Y0[j]) * (Y0[i] - Y0[j]))) <= COMPARE_DIS) {
          compare_num += 1;
        }
        if (sqrt((X0[i] - X1[j]) * (X0[i] - X1[j]) + ((Y0[i] - Y1[j]) * (Y0[i] - Y1[j]))) <= COMPARE_DIS) {
          compare_num += 2;
        }
        if (sqrt((X1[i] - X0[j]) * (X1[i] - X0[j]) + ((Y1[i] - Y0[j]) * (Y1[i] - Y0[j]))) <= COMPARE_DIS) {
          compare_num += 4;
        }
        if (sqrt((X1[i] - X1[j]) * (X1[i] - X1[j]) + ((Y1[i] - Y1[j]) * (Y1[i] - Y1[j]))) <= COMPARE_DIS) {
          compare_num += 8;
        }

        num = start_x_count;
        if (compare_num == 1) {
          start_x_count++;
          start_x[num] = X1[i];
          start_y[num] = Y1[i];
          middle_x[num] = (X0[i] + X0[j]) / 2;
          middle_y[num] = (Y0[i] + Y0[j]) / 2;
          end_x[num] = X1[j];
          end_y[num] = Y1[j];
        } else if (compare_num == 2) {
          start_x_count++;
          start_x[num] = X1[i];
          start_y[num] = Y1[i];
          middle_x[num] = (X0[i] + X1[j]) / 2;
          middle_y[num] = (Y0[i] + Y1[j]) / 2;
          end_x[num] = X0[j];
          end_y[num] = Y0[j];
        } else if (compare_num == 4) {
          start_x_count++;
          start_x[num] = X0[i];
          start_y[num] = Y0[i];
          middle_x[num] = (X1[i] + X0[j]) / 2;
          middle_y[num] = (Y1[i] + Y0[j]) / 2;
          end_x[num] = X1[j];
          end_y[num] = Y1[j];
        } else if (compare_num == 8) {
          start_x_count++;
          start_x[num] = X0[i];
          start_y[num] = Y0[i];
          middle_x[num] = (X1[i] + X1[j]) / 2;
          middle_y[num] = (Y1[i] + Y1[j]) / 2;
          end_x[num] = X0[j];
          end_y[num] = Y0[j];
        }
      }
    }
  }
  Serial.println(F("start_x_count: "));
  Serial.println(start_x_count);

  if (LINE_GET == 1) {
    LINE_GET_TIMER = millis();
    //averge filter
    averge_middle[averge_component - 1] = averge_middle[averge_component];
    averge_middle[averge_component] = pixy_middle;

    int line_middle_filte = 0;

    for (int i = 1 ; i <= averge_component ; i++) {
      line_middle_filte += averge_middle[i];
    }
    line_middle_filte = line_middle_filte / averge_component ;

    LINE_Comparison[0] = x0;
    LINE_Comparison[1] = y0;
    LINE_Comparison[2] = x1;
    LINE_Comparison[3] = y1;
    LINE_Comparison[4] = pixy_angle;
    LINE_Comparison[5] = line_middle_filte;

  }

  //compare right turn with middle line
  for (int i = 0; i < start_x_count; i++) {
    int b_origin = LINE_Comparison[3] - (LINE_Comparison[2]) * ((LINE_Comparison[2] - LINE_Comparison[0]) / (LINE_Comparison[3] - LINE_Comparison[1]));
    if (abs(LINE_Comparison [4] - (atan((float)(middle_y[i] - start_y[i]) / (middle_x[i] - start_x[i])) * 57.296)) < detect_angle) {
      if (abs(b_origin - (middle_y[i] - (middle_x[i] * (middle_x[i] - start_x[i]) / (middle_y[i] - start_y[i])))) < detect_distance) {
        right_turn_num = i;
        break;
      }
    }

    right_turn_timer = millis();

  }

  if (right_turn_enable) {
    analogWrite(B, 255);
    analogWrite(R, 0);
    analogWrite(G, 0);
    if ((millis() - break_timer < 400) and BREAK_CHECKER == 0) { //break
      LINE_FOLLOW_BREAK();
    } else if ((millis() - break_timer > 400) and (millis() - break_timer < 2000) and BREAK_CHECKER == 0) { //break end up and turn
      BREAK_CHECKER = 1;
      CH1.writeMicroseconds(1400 - (middle_x[right_turn_num] - 39) * X_CCC_DETECT_VALUE_P);
      CH2.writeMicroseconds(1400 - (middle_y[right_turn_num] - 26) * X_CCC_DETECT_VALUE_P);
      CH3.writeMicroseconds(1400);
      CH4.writeMicroseconds(1350);
      CH5.writeMicroseconds(1400);
    } else if ((millis() - break_timer > 5000) or (abs(pixy_angle) <= 10)) {
      right_turn_enable = 0;
      CH1.writeMicroseconds(1400 - (middle_x[right_turn_num] - 39) * X_CCC_DETECT_VALUE_P);
      CH2.writeMicroseconds(1400 - (middle_y[right_turn_num] - 26) * X_CCC_DETECT_VALUE_P);
      CH3.writeMicroseconds(1400);
      CH4.writeMicroseconds(1350);
      CH5.writeMicroseconds(1400);
      turn_over = 1;
    } else {
      CH1.writeMicroseconds(1400 - (middle_x[right_turn_num] - 39) * X_CCC_DETECT_VALUE_P);
      CH2.writeMicroseconds(1400 - (middle_y[right_turn_num] - 26) * X_CCC_DETECT_VALUE_P);
      CH3.writeMicroseconds(1400);
      CH4.writeMicroseconds(1350);
      CH5.writeMicroseconds(1400);
    }
  } else {
    analogWrite(B, 0);
    analogWrite(R, 255);
    analogWrite(G, 0);
    //PID
    LINE_DETECT_VALUE_DIF = LINE_DETECT_VALUE_PRE - LINE_Comparison[5] ;
    LINE_DETECT_VALUE_PRE = LINE_Comparison[5];

    LINE_DETECT_VALUE_INTE += LINE_Comparison[5];

    LINE_PID_VALUE = (LINE_Comparison[5] * LINE_DETECT_VALUE_P) + (LINE_DETECT_VALUE_DIF * LINE_DETECT_VALUE_D) + (LINE_DETECT_VALUE_INTE * LINE_DETECT_VALUE_I);

    CH1.writeMicroseconds(1400 + LINE_PID_VALUE * sin(LINE_Comparison [4]));
    CH2.writeMicroseconds(1400 - 40 * cos(LINE_Comparison [4]));
    CH3.writeMicroseconds(1400);
    CH4.writeMicroseconds(1400 - (LINE_Comparison [4] * LINE_DIRECTION_VALUE));
    CH5.writeMicroseconds(1400);

    Serial.print(1400 + LINE_PID_VALUE * sin(LINE_Comparison [4]));
    Serial.print(F("  "));
    Serial.println(1400 - 40 * cos(LINE_Comparison [4]));

    //detect to 0.5s
    if (millis() - right_turn_timer > 500) {
      right_turn_enable = true;
    } else {
      right_turn_enable = false;
    }

  }
}

void LINE_RIGHT_ANGLE_AND_STRAGHT_v1() {
  LINE_DETECT();
  RIGHT_ANGLE_DETECT();

  if (start_x_count > 0) {
    Serial.println("1");
    for (int i = 0; i <= start_x_count - 1; i++) {
      int b_origin = y1 - (x1) * ((x1 - x0) / (y1 - y0));
      Serial.print("b_origin: ");
      Serial.println(b_origin);
      if (abs(pixy_angle - (atan((float)(middle_y[i] - start_y[i]) / (middle_x[i] - start_x[i])) * 57.296)) < detect_angle) {
        if (abs(b_origin - (middle_y[i] - (middle_x[i] * (middle_x[i] - start_x[i]) / (middle_y[i] - start_y[i])))) < detect_distance) {
          right_turn_num = i;
          Serial.println("break");
          break;
        }
      }
    }
  } else {
    Serial.println("0");
    right_turn_timer = millis();
  }


  if (right_turn_enable) {
    analogWrite(B, 255);
    analogWrite(R, 0);
    analogWrite(G, 0);
    Serial.println(millis() - break_timer);
    if ((millis() - break_timer < 400) and BREAK_CHECKER == 0) { //break
      LINE_FOLLOW_BREAK();
      Serial.println(F("BREAK"));
    } else if ((millis() - break_timer > 400) and (millis() - break_timer < 2000) and BREAK_CHECKER == 0) { //break end up and turn
      BREAK_CHECKER = 1;
      CH1.writeMicroseconds(1400 - (middle_x[right_turn_num] - 39) * X_CCC_DETECT_VALUE_P);
      CH2.writeMicroseconds(1400 - (middle_y[right_turn_num] - 26) * X_CCC_DETECT_VALUE_P);
      CH3.writeMicroseconds(1400);
      CH4.writeMicroseconds(1350);
      CH5.writeMicroseconds(1400);
      Serial.println(F("1"));
    } else if ((millis() - break_timer > 5000) or (abs(pixy_angle) <= 10)) {
      right_turn_enable = 0;
      CH1.writeMicroseconds(1400 - (middle_x[right_turn_num] - 39) * X_CCC_DETECT_VALUE_P);
      CH2.writeMicroseconds(1400 - (middle_y[right_turn_num] - 26) * X_CCC_DETECT_VALUE_P);
      CH3.writeMicroseconds(1400);
      CH4.writeMicroseconds(1350);
      CH5.writeMicroseconds(1400);
      turn_over = 1;
      Serial.println(F("2"));
    } else {
      CH1.writeMicroseconds(1400 - (middle_x[right_turn_num] - 39) * X_CCC_DETECT_VALUE_P);
      CH2.writeMicroseconds(1400 - (middle_y[right_turn_num] - 26) * X_CCC_DETECT_VALUE_P);
      CH3.writeMicroseconds(1400);
      CH4.writeMicroseconds(1350);
      CH5.writeMicroseconds(1400);
      Serial.println(F("3"));
    }
  } else {
    analogWrite(B, 0);
    analogWrite(R, 255);
    analogWrite(G, 0);
    //PID
    LINE_DETECT_VALUE_DIF = LINE_DETECT_VALUE_PRE - pixy_middle ;
    LINE_DETECT_VALUE_PRE = pixy_middle;

    LINE_DETECT_VALUE_INTE += pixy_middle;
    LINE_DETECT_VALUE_INTE = constrain(LINE_DETECT_VALUE_INTE, -20, 20);

    LINE_PID_VALUE = (pixy_middle * LINE_DETECT_VALUE_P) + (LINE_DETECT_VALUE_DIF * LINE_DETECT_VALUE_D) + (LINE_DETECT_VALUE_INTE * LINE_DETECT_VALUE_I);

    CH1.writeMicroseconds(1400 + LINE_PID_VALUE * cos(pixy_angle));
    CH2.writeMicroseconds(1400 - (40 * sin(pixy_angle)));
    CH3.writeMicroseconds(1400);
    CH4.writeMicroseconds(1400 - (pixy_angle * LINE_DIRECTION_VALUE));
    CH5.writeMicroseconds(1400);

    Serial.println(cos(pixy_angle));
    Serial.print(1400 + (LINE_PID_VALUE * cos(pixy_angle)));

    Serial.print(F("  "));
    Serial.println(1400 - (40 * sin(pixy_angle)));

    //detect to 0.5s
    if (millis() - right_turn_timer > 2000) {
      right_turn_enable = true;
    } else {
      right_turn_enable = false;
    }

  }

}

void RIGHT_TURN() {
  analogWrite(B, 255);
  analogWrite(R, 0);
  analogWrite(G, 0);
  if ((millis() - break_timer < 500) and BREAK_CHECKER == 0) { //break
    LINE_FOLLOW_BREAK();
    Serial.println("break");
  } else if ((millis() - break_timer > 400) and (millis() - break_timer < 2000) and BREAK_CHECKER == 0) { //break end up and turn
    BREAK_CHECKER = 1;
    CH1.writeMicroseconds(1400 - (middle_x[right_turn_num] - 39) * X_CCC_DETECT_VALUE_P);
    CH2.writeMicroseconds(1400 - (middle_y[right_turn_num] - 26) * X_CCC_DETECT_VALUE_P);
    CH3.writeMicroseconds(1400);
    CH4.writeMicroseconds(1350);
    CH5.writeMicroseconds(1400);
    Serial.println("turn");
  } else if ((millis() - break_timer > 5000) or (abs(pixy_angle) <= 10)) {
    right_turn_enable = 0;
    CH1.writeMicroseconds(1400 - (middle_x[right_turn_num] - 39) * X_CCC_DETECT_VALUE_P);
    CH2.writeMicroseconds(1400 - (middle_y[right_turn_num] - 26) * X_CCC_DETECT_VALUE_P);
    CH3.writeMicroseconds(1400);
    CH4.writeMicroseconds(1350);
    CH5.writeMicroseconds(1400);
    BREAK_CHECKER = 0;
    start_x_count = 0;
    turn_over = 1;
    turn_over_2 += 1;
    turn_timer = millis();
    Serial.println("turn_over");
  } else {
    CH1.writeMicroseconds(1400 - (middle_x[right_turn_num] - 39) * X_CCC_DETECT_VALUE_P);
    CH2.writeMicroseconds(1400 - (middle_y[right_turn_num] - 26) * X_CCC_DETECT_VALUE_P);
    CH3.writeMicroseconds(1400);
    CH4.writeMicroseconds(1350);
    CH5.writeMicroseconds(1400);
  }
}

void LINE_RIGHT_ANGLE_AND_STRAGHT() {
  right_turn_count = constrain(right_turn_count, 0, 100);
  RIGHT_ANGLE_DETECT();

  if (start_x_count > 0) {
    Serial.println("GET");
    right_turn_count += 1;
  } else {
    right_turn_count -= 1;
  }
 
  if (right_turn_count > 70 && (millis() - turn_timer > 3000) ) {
    Serial.println("right turn");
    RIGHT_TURN();
  } else {
    LINE_FOLLOW();
    break_timer = millis();
  }

}

//old fuction block
void LINE_FOLLOW() {
  LINE_DETECT();

  LINE_DETECT_VALUE_DIF = LINE_DETECT_VALUE_PRE - pixy_middle ;
  LINE_DETECT_VALUE_PRE = pixy_middle;

  LINE_DETECT_VALUE_INTE += pixy_middle;

  CH1.writeMicroseconds(1400 + (pixy_middle * LINE_DETECT_VALUE_P) + (LINE_DETECT_VALUE_DIF * LINE_DETECT_VALUE_D) + (LINE_DETECT_VALUE_INTE * LINE_DETECT_VALUE_I));
  CH2.writeMicroseconds(1360);
  CH3.writeMicroseconds(1400);
  CH4.writeMicroseconds(1400 - (pixy_angle * LINE_DIRECTION_VALUE));
  CH5.writeMicroseconds(1400);
}
void LINE_FOLLOW_BREAK() {
  LINE_DETECT();

  LINE_DETECT_VALUE_DIF = LINE_DETECT_VALUE_PRE - pixy_middle ;
  LINE_DETECT_VALUE_PRE = pixy_middle;

  CH1.writeMicroseconds(1400 + (pixy_middle * LINE_DETECT_VALUE_P) + (LINE_DETECT_VALUE_DIF * LINE_DETECT_VALUE_D));
  CH2.writeMicroseconds(1550);
  CH3.writeMicroseconds(1400);
  CH4.writeMicroseconds(1400 - (pixy_angle * LINE_DIRECTION_VALUE));
  CH5.writeMicroseconds(1400);
}
void CCC_WAITING() {
  CCC_DETECT_COLOR_WAITING();
  LINE_DETECT();

  X_CCC_DETECT_VALUE_DIF = X_CCC_DETECT_VALUE_PRE - pixy_ccc_x;
  X_CCC_DETECT_VALUE_PRE = pixy_ccc_x;
  Y_CCC_DETECT_VALUE_DIF = Y_CCC_DETECT_VALUE_PRE - pixy_ccc_y;
  Y_CCC_DETECT_VALUE_PRE = pixy_ccc_y;

  if (abs(pixy_ccc_x - 158) < 30) {
    CH1.writeMicroseconds(1400);
  } else {
    CH1.writeMicroseconds(1400 + (( pixy_ccc_x ) * X_CCC_DETECT_VALUE_P ) + (( X_CCC_DETECT_VALUE_DIF ) * X_CCC_DETECT_VALUE_D ));
  }

  if (abs(pixy_ccc_y - 158) < 20) {
    CH2.writeMicroseconds(1400);
  } else {
    CH2.writeMicroseconds(1400 + (( pixy_ccc_y ) * Y_CCC_DETECT_VALUE_P ) + (( Y_CCC_DETECT_VALUE_DIF ) * Y_CCC_DETECT_VALUE_D ));
  }
  CH3.writeMicroseconds(1400);
  CH4.writeMicroseconds(1400 - (pixy_angle * LINE_DIRECTION_VALUE));
  CH5.writeMicroseconds(1400);

  Serial.print(F("CH1: "));
  Serial.print(1400 + (( pixy_ccc_x ) * X_CCC_DETECT_VALUE_P ) + (( X_CCC_DETECT_VALUE_DIF ) * X_CCC_DETECT_VALUE_D ));
  Serial.print(F("  CH2: "));
  Serial.print(1400 + (( pixy_ccc_y ) * Y_CCC_DETECT_VALUE_P ) + (( Y_CCC_DETECT_VALUE_DIF ) * Y_CCC_DETECT_VALUE_D ));
  Serial.print(F("  CH3: "));
  Serial.println(1400 - (pixy_angle * LINE_DIRECTION_VALUE));
}
void LINE_STABLE_CHECK() {
  LINE_DETECT();

  LINE_DETECT_VALUE_DIF = LINE_DETECT_VALUE_PRE - pixy_middle ;
  LINE_DETECT_VALUE_PRE = pixy_middle;

  LINE_DETECT_VALUE_INTE += pixy_middle;

  CH1.writeMicroseconds(1400 + (pixy_middle * LINE_DETECT_VALUE_P) + (LINE_DETECT_VALUE_DIF * LINE_DETECT_VALUE_D) + (LINE_DETECT_VALUE_INTE * LINE_DETECT_VALUE_I));
  CH2.writeMicroseconds(1400);
  CH3.writeMicroseconds(1400);
  CH4.writeMicroseconds(1400 - (pixy_angle * LINE_DIRECTION_VALUE));
  CH5.writeMicroseconds(1400);
}

//basic function
void THROW() {
  if (throw_timer_enable == 0) {
    throw_timer = millis();
    throw_timer_enable = 1;
  }
  if (millis() - throw_timer < 600 ) {
    THROW_CH.write(10);
  } else {
    Serial.print(F("THROW"));
    THROW_CH.write(180);
    THROW_CHECK = 1;
    turn_over_2 = 0;
  }
}

void stand_open() {
  if (stand_check == 0) {
    stand_timer = millis();
    stand_check = 1;
  }
  if (millis() - stand_timer < 400 ) {
    STAND_CH.write(180);
  } else {
    STAND_CH.write(90);
  }
}
void stand_close() {
  if (stand_check == 1) {
    stand_timer = millis();
    stand_check = 0;
  }
  if (millis() - stand_timer < 400 ) {
    STAND_CH.write(0);
  } else {
    STAND_CH.write(90);
  }
}

void FLY_APM() {
  analogWrite(R, 0);
  analogWrite(G, 0);
  analogWrite(B, 0);
  while (1) {

    stand_open();
    Serial.println(F("APM"));
    Data_receive();
    CH1.writeMicroseconds(receive_data.PITCH);
    CH2.writeMicroseconds(receive_data.ROLL);
    CH3.writeMicroseconds(receive_data.THROTTLE);
    CH4.writeMicroseconds(receive_data.YAW);

    int mode = 0;
    if (receive_data.MODE == 0) {
      mode = 800;
    } else if (receive_data.MODE == 1) {
      mode = 1400;
    }

    if (RECEIVER_CHECK > 1500) {
      mode = 1800;
    }
    CH5.writeMicroseconds(mode);

    if (receive_data.FUNC_1 != 1) {
      break;
    }
  }
}

void Data_receive() {

  if (radio.available()) {
    radio.read(&receive_data, sizeof(receive_data));
    RECEIVER_CHECK = 0;
  } else {
    RECEIVER_CHECK++;
  }

  RECEIVER_CHECK = constrain(RECEIVER_CHECK, 0, 3000);
  if (RECEIVER_CHECK > 1500) {
    receive_data.THROTTLE = 900;
    receive_data.YAW = 1400;
    receive_data.PITCH = 1400;
    receive_data.ROLL = 1400;
    //receive_data.FUNC_1 = 0;
    receive_data.FUNC_2 = 0;
  }
  Serial.print(F("RECEIVE: "));
  Serial.print(receive_data.THROTTLE); Serial.print(F(" , "));
  Serial.print(receive_data.YAW); Serial.print(F(" , "));
  Serial.print(receive_data.PITCH); Serial.print(F(" , "));
  Serial.print(receive_data.ROLL); Serial.print(F(" , "));
  Serial.print(receive_data.FUNC_1); Serial.print(F(" , "));
  Serial.print(receive_data.FUNC_2); Serial.print(F(" , "));
  Serial.print(receive_data.Test); Serial.println(F(" , "));
  //Serial.print(receive_data.I); //Serial.print(" , ");
  //Serial.print(receive_data.D); //Serial.println(" , ");
}
