#include <libmaple/scb.h>

uint32_t now, systick_counter, receiver_input1, receiver_input1_previous;
uint8_t int_flag;

void setup() {
  Serial.begin(57600);                                                    //Start serial port at 57600bps
  attachInterrupt(PB10, receiver_ch1, CHANGE);                            //Connect changing PB10 to routine receiver_ch1
}

void loop() {
  delayMicroseconds(3500);                                                //Wait 3500us to simulate  a 250Hz refresh rate
  Serial.println(receiver_input1);                                        //Print the receiver input to the serial monitor
}

void receiver_ch1() {
  systick_counter = SYSTICK_BASE->CNT;                                    //Read SysTick counter
  if (0b1 & SCB_BASE->ICSR >> 26) {                                       //If SysTick interrupt pending flag is set
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


