uint32_t now, receiver_input1, receiver_input1_previous;

void setup() {
  Serial.begin(57600);                                               //Start serial port at 57600bps
  attachInterrupt(PB10, receiver_ch1, CHANGE);                       //Connect changing PB10 to routine receiver_ch1
}

void loop() {
  delayMicroseconds(3500);                                           //Wait 3500us to simulate  a 250Hz refresh rate
  Serial.println(receiver_input1);                                   //Print the receiver input to the serial monitor
}

void receiver_ch1() {
  now = micros();                                                    //Store the current micros() value
  if (0B1 & GPIOB_BASE->IDR >> 10 )receiver_input1_previous = now;   //If input PB10 is high start measuring the time
  else receiver_input1 = now - receiver_input1_previous;             //If input PB10 is low calculate the total pulse time
}

