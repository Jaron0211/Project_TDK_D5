
int EX_DATA[24] = {1,0,1,0,1,0,0,0,0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,1};//m7~m0,m15~m8,m23~m16
String OUTLINE_DATA,SUM_LAST_HEX;

void setup() {
  Serial.begin(9600);
  Serial1.begin(19200,SERIAL_7E1);
  // put your setup code here, to run once:

}



void loop() {
  // put your main code here, to run repeatedly:
  ARDUINO_TO_PLC(EX_DATA);
  delay(100);
  //Serial.println(OUTLINE_DATA);
}

void ARDUINO_TO_PLC(int DATA[24]){

  int to_plc[20] = {2,48,48,54,49,48,48,56,48,48,51,0,0,0,0,0,0,3,0,0};

  //int to_plc[16] = {2,30,30,36,31,30,30,38,30,30,31,0,0,3,0,0};
  
  byte TO_PLC_1;
  for(int i = 0;i<=7;i++){
    byte BIT = DATA[i];
    TO_PLC_1 = TO_PLC_1 << 1;
    TO_PLC_1 |= BIT;
  }
  
  OUTLINE_DATA = String(int(TO_PLC_1),HEX);
  OUTLINE_DATA.toUpperCase();
  Serial.println(OUTLINE_DATA);
  if(OUTLINE_DATA.length() < 2){
    int OUTLINE_DATA_len = OUTLINE_DATA.length()+1;
    char OUTLINE_DATA_CHAR[OUTLINE_DATA_len];
    OUTLINE_DATA.toCharArray(OUTLINE_DATA_CHAR,OUTLINE_DATA_len);  
    to_plc[11] = 48;
    to_plc[12] = OUTLINE_DATA_CHAR[0];
  }else{
    int OUTLINE_DATA_len = OUTLINE_DATA.length()+1;
    char OUTLINE_DATA_CHAR[OUTLINE_DATA_len];
    OUTLINE_DATA.toCharArray(OUTLINE_DATA_CHAR,OUTLINE_DATA_len);  
    to_plc[11] = OUTLINE_DATA_CHAR[0];
    to_plc[12] = OUTLINE_DATA_CHAR[1];
  }

  byte TO_PLC_2;
  for(int i = 8;i<=15;i++){
    byte BIT = DATA[i];
    TO_PLC_2 = TO_PLC_2 << 1;
    TO_PLC_2 |=  BIT;
  }
  
  OUTLINE_DATA = String(int(TO_PLC_2),HEX);
  OUTLINE_DATA.toUpperCase();
  Serial.println(OUTLINE_DATA);
  if(OUTLINE_DATA.length() < 2){
    int OUTLINE_DATA_len = OUTLINE_DATA.length()+1;
    char OUTLINE_DATA_CHAR[OUTLINE_DATA_len];
    OUTLINE_DATA.toCharArray(OUTLINE_DATA_CHAR,OUTLINE_DATA_len);  
    to_plc[13] = 48;
    to_plc[14] = OUTLINE_DATA_CHAR[0];
  }else{
    int OUTLINE_DATA_len = OUTLINE_DATA.length()+1;
    char OUTLINE_DATA_CHAR[OUTLINE_DATA_len];
    OUTLINE_DATA.toCharArray(OUTLINE_DATA_CHAR,OUTLINE_DATA_len);  
    to_plc[13] = OUTLINE_DATA_CHAR[0];
    to_plc[14] = OUTLINE_DATA_CHAR[1];
  }

  byte TO_PLC_3;
  for(int i = 16;i<=23;i++){
    byte BIT = DATA[i];
    TO_PLC_3 = TO_PLC_3 << 1;
    TO_PLC_3 |=  BIT;
  }
  
  OUTLINE_DATA = String(int(TO_PLC_3),HEX);
  OUTLINE_DATA.toUpperCase();
  Serial.println(OUTLINE_DATA);
  if(OUTLINE_DATA.length() < 2){
    int OUTLINE_DATA_len = OUTLINE_DATA.length()+1;
    char OUTLINE_DATA_CHAR[OUTLINE_DATA_len];
    OUTLINE_DATA.toCharArray(OUTLINE_DATA_CHAR,OUTLINE_DATA_len);  
    to_plc[15] = 48;
    to_plc[16] = OUTLINE_DATA_CHAR[0];
  }else{
    int OUTLINE_DATA_len = OUTLINE_DATA.length()+1;
    char OUTLINE_DATA_CHAR[OUTLINE_DATA_len];
    OUTLINE_DATA.toCharArray(OUTLINE_DATA_CHAR,OUTLINE_DATA_len);  
    to_plc[15] = OUTLINE_DATA_CHAR[0];
    to_plc[16] = OUTLINE_DATA_CHAR[1];
  }
  
  

  int SUM_INT = 0;
  for(int i = 1;i<20;i++){
    SUM_INT += to_plc[i];
  }
  String SUM = String(SUM_INT,HEX);
  SUM.toUpperCase();
  int SUM_len = SUM.length()+1;
  char SUM_CHAR[SUM.length()+1];
  SUM.toCharArray(SUM_CHAR,SUM_len);  
  to_plc[18] = SUM_CHAR[SUM.length()-2];
  to_plc[19] = SUM_CHAR[SUM.length()-1];
  

  for(int i =0;i<20;i++){
    Serial.print(to_plc[i]);
    Serial.print(",");
  }
  Serial.println();
  char send_plc[20];
  for(int i =0;i<20;i++){
    send_plc[i] = (to_plc[i]);
  }
  Serial1.write(send_plc);  
}

int HEX_TO_DEC(int HEX_IN){
  int ten = HEX_IN/10*16;
  int one = HEX_IN%10;

  return(ten+one);
}
