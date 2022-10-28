unsigned long timer = 0;

void setup() {
  //Serial.begin(2000000);
  pinMode(2,OUTPUT);
  pinMode(3,INPUT);
  pinMode(5,INPUT);
  pinMode(6,INPUT);
  pinMode(7,INPUT);
  pinMode(9,INPUT);

  timer = millis();
  // put your setup code here, to run once:

}

void loop() {
  /*
  int CH1 = pulseIn(9,1);
  int CH2 = pulseIn(7,1);
  int CH3 = pulseIn(6,1);
  int CH4 = pulseIn(5,1);
  int CH5 = pulseIn(3,1);
*/
 
  //Serial.println(micros()-timer); 
  if(micros() - timer <= 300){
    digitalWrite(2,1);
    Serial.println("1");
  }else if(300 < micros() - timer and micros() - timer <= 1500){
    digitalWrite(2,0);
    Serial.println("0");
  }else if( (1500) < micros() - timer and micros() - timer < 22500){
    digitalWrite(2,0);
    Serial.println("0");
  }else{
    //Serial.println();
    timer = micros();
  }
  
/*
  Serial.print("CH1:");Serial.print(CH1);Serial.print(",");
  Serial.print("CH2:");Serial.print(CH2);Serial.print(",");
  Serial.print("CH3:");Serial.print(CH3);Serial.print(",");
  Serial.print("CH4:");Serial.print(CH4);Serial.print(",");
  Serial.print("CH5:");Serial.print(CH5);Serial.println();
  */
  
  // put your main code here, to run repeatedly:

}
