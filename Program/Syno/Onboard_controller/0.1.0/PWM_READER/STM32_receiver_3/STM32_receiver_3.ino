#include <libmaple/scb.h>

uint32_t now, systick_counter;
uint32_t receiver_input1, receiver_input1_previous;
uint32_t receiver_input2, receiver_input2_previous;
uint32_t receiver_input3, receiver_input3_previous;
uint32_t receiver_input4, receiver_input4_previous;
uint32_t receiver_input5, receiver_input5_previous;
uint32_t receiver_input6, receiver_input6_previous;
uint8_t int_flag;

void setup() {
  pinMode(PB5, OUTPUT);                                                   //Set port B5 as output
  Serial.begin(57600);                                                    //Start serial port at 57600bps
  attachInterrupt(PB10, receiver_ch1, CHANGE);                            //Connect changing PB10 to routine receiver_ch1
  attachInterrupt(PB11, receiver_ch2, CHANGE);                            //Connect changing PB10 to routine receiver_ch1
  attachInterrupt(PB12, receiver_ch3, CHANGE);                            //Connect changing PB10 to routine receiver_ch1
  attachInterrupt(PB13, receiver_ch4, CHANGE);                            //Connect changing PB10 to routine receiver_ch1
  attachInterrupt(PB14, receiver_ch5, CHANGE);                            //Connect changing PB10 to routine receiver_ch1
  attachInterrupt(PB15, receiver_ch6, CHANGE);                            //Connect changing PB10 to routine receiver_ch1
}

void loop() {
  delay(100);                                                             //Wait 100 milliseconds
  Serial.print("1:");
  Serial.print(receiver_input1);                                          //Print the receiver input 1 to the serial monitor
  Serial.print(" 2:");
  Serial.print(receiver_input2);                                          //Print the receiver input 2 to the serial monitor
  Serial.print(" 3:");
  Serial.print(receiver_input3);                                          //Print the receiver input 3 to the serial monitor
  Serial.print(" 4:");
  Serial.print(receiver_input4);                                          //Print the receiver input 4 to the serial monitor
  Serial.print(" 5:");
  Serial.print(receiver_input5);                                          //Print the receiver input 5 to the serial monitor
  Serial.print(" 6:");
  Serial.println(receiver_input6);                                        //Print the receiver input 6 to the serial monitor  
}

void receiver_ch1() {
  systick_counter = SYSTICK_BASE->CNT;                                    //Read SysTick counter
  if (0b1 & SCB_BASE->ICSR >> 26) {                                       //If SysTick interrupt flag is set
    int_flag = 1;                                                         //Set interrupt flag
    systick_counter = SYSTICK_BASE->CNT;                                  //Re-read the SysTick counter
  }
  else int_flag = 0;                                                      //SysTick interrupt flag is not set during reading

  now = (systick_uptime_millis * 1000) + 
    (SYSTICK_RELOAD_VAL + 1 - systick_counter) / CYCLES_PER_MICROSECOND;  //Calculate the total microseconds
  if (int_flag)now += 1000;                                               //If the SysTick interrupt is set 1000us have to added to get the correct microseconds result

  if (0B1 & GPIOB_BASE->IDR >> 10 )receiver_input1_previous = now;        //If input PB10 is high start measuring the time
  else receiver_input1 = now - receiver_input1_previous;                  //If input PB10 is low calculate the total pulse time
}

void receiver_ch2() {
  systick_counter = SYSTICK_BASE->CNT;                                    //Read SysTick counter
  if (0b1 & SCB_BASE->ICSR >> 26) {                                       //If SysTick interrupt flag is set
    int_flag = 1;                                                         //Set interrupt flag
    systick_counter = SYSTICK_BASE->CNT;                                  //Re-read the SysTick counter
  }
  else int_flag = 0;                                                      //SysTick interrupt flag is not set during reading

  now = (systick_uptime_millis * 1000) + 
    (SYSTICK_RELOAD_VAL + 1 - systick_counter) / CYCLES_PER_MICROSECOND;  //Calculate the total microseconds
  if (int_flag)now += 1000;                                               //If the SysTick interrupt is set 1000us have to added to get the correct microseconds result

  if (0B1 & GPIOB_BASE->IDR >> 11 )receiver_input2_previous = now;        //If input PB11 is high start measuring the time
  else receiver_input2 = now - receiver_input2_previous;                  //If input PB11 is low calculate the total pulse time
}

void receiver_ch3() {
  systick_counter = SYSTICK_BASE->CNT;                                    //Read SysTick counter
  if (0b1 & SCB_BASE->ICSR >> 26) {                                       //If SysTick interrupt flag is set
    int_flag = 1;                                                         //Set interrupt flag
    systick_counter = SYSTICK_BASE->CNT;                                  //Re-read the SysTick counter
  }
  else int_flag = 0;                                                      //SysTick interrupt flag is not set during reading

  now = (systick_uptime_millis * 1000) + 
    (SYSTICK_RELOAD_VAL + 1 - systick_counter) / CYCLES_PER_MICROSECOND;  //Calculate the total microseconds
  if (int_flag)now += 1000;                                               //If the SysTick interrupt is set 1000us have to added to get the correct microseconds result

  if (0B1 & GPIOB_BASE->IDR >> 12 )receiver_input3_previous = now;        //If input PB12 is high start measuring the time
  else receiver_input3 = now - receiver_input3_previous;                  //If input PB12 is low calculate the total pulse time
}

void receiver_ch4() {
  systick_counter = SYSTICK_BASE->CNT;                                    //Read SysTick counter
  if (0b1 & SCB_BASE->ICSR >> 26) {                                       //If SysTick interrupt flag is set
    int_flag = 1;                                                         //Set interrupt flag
    systick_counter = SYSTICK_BASE->CNT;                                  //Re-read the SysTick counter
  }
  else int_flag = 0;                                                      //SysTick interrupt flag is not set during reading

  now = (systick_uptime_millis * 1000) + 
    (SYSTICK_RELOAD_VAL + 1 - systick_counter) / CYCLES_PER_MICROSECOND;  //Calculate the total microseconds
  if (int_flag)now += 1000;                                               //If the SysTick interrupt is set 1000us have to added to get the correct microseconds result

  if (0B1 & GPIOB_BASE->IDR >> 13 )receiver_input4_previous = now;        //If input PB13 is high start measuring the time
  else receiver_input4 = now - receiver_input4_previous;                  //If input PB13 is low calculate the total pulse time
}

void receiver_ch5() {
  systick_counter = SYSTICK_BASE->CNT;                                    //Read SysTick counter
  if (0b1 & SCB_BASE->ICSR >> 26) {                                       //If SysTick interrupt flag is set
    int_flag = 1;                                                         //Set interrupt flag
    systick_counter = SYSTICK_BASE->CNT;                                  //Re-read the SysTick counter
  }
  else int_flag = 0;                                                      //SysTick interrupt flag is not set during reading

  now = (systick_uptime_millis * 1000) + 
    (SYSTICK_RELOAD_VAL + 1 - systick_counter) / CYCLES_PER_MICROSECOND;  //Calculate the total microseconds
  if (int_flag)now += 1000;                                               //If the SysTick interrupt is set 1000us have to added to get the correct microseconds result

  if (0B1 & GPIOB_BASE->IDR >> 14 )receiver_input5_previous = now;        //If input PB14 is high start measuring the time
  else receiver_input5 = now - receiver_input5_previous;                  //If input PB14 is low calculate the total pulse time
}

void receiver_ch6() {
  systick_counter = SYSTICK_BASE->CNT;                                    //Read SysTick counter
  if (0b1 & SCB_BASE->ICSR >> 26) {                                       //If SysTick interrupt flag is set
    int_flag = 1;                                                         //Set interrupt flag
    systick_counter = SYSTICK_BASE->CNT;                                  //Re-read the SysTick counter
  }
  else int_flag = 0;                                                      //SysTick interrupt flag is not set during reading

  now = (systick_uptime_millis * 1000) + 
    (SYSTICK_RELOAD_VAL + 1 - systick_counter) / CYCLES_PER_MICROSECOND;  //Calculate the total microseconds
  if (int_flag)now += 1000;                                               //If the SysTick interrupt is set 1000us have to added to get the correct microseconds result

  if (0B1 & GPIOB_BASE->IDR >> 15 )receiver_input6_previous = now;        //If input PB15 is high start measuring the time
  else receiver_input6 = now - receiver_input6_previous;                  //If input PB15 is low calculate the total pulse time
}

