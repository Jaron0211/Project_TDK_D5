#include <Wire.h>
#define BAUD_RATE 19200
#define CHAR_BUF 128

 void setup() {
   Serial.begin(BAUD_RATE);
   Wire.begin();
   delay(1000); // Give the OpenMV Cam time to bootup.
 }

 void loop() {
   int32_t temp = 0;
   char buff[CHAR_BUF] = {0};

   int MODE = '1';

   Wire.beginTransmission(0x12);
   Wire.write(MODE);
   Wire.endTransmission();
   delay(1);
   
   Wire.requestFrom(0x12, 2);
   if(Wire.available() == 2) { // got length?
     Serial.println("get length");
     temp = Wire.read() | (Wire.read() << 8);
     //delay(1); // Give some setup time...

     Wire.requestFrom(0x12, temp);
     if(Wire.available() == temp) { // got full message?
       Serial.println("get all");
       temp = 0;
       while(Wire.available()) buff[temp++] = Wire.read(); //Serial.println(buff);

     } else {
       while(Wire.available()) Wire.read(); // Toss garbage bytes.
     }
   } else {
     while(Wire.available()) Wire.read(); // Toss garbage bytes.
   }
  
   Serial.print(buff);
   //delay(100); // Don't loop to quickly.
 }
