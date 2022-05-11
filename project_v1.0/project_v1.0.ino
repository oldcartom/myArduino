#include <SoftwareSerial.h> 

SoftwareSerial BlueTooth(4,5);

/*beacon data 크기*/
#define MAXDATA 78

/* 차량신호등 LED 핀번호 정의*/
#define pinCarG 13 
#define pinCarY 12 
#define pinCarR 11 

/* 보행자신호등 LED 핀번호 정의*/
#define pinHumanG 10 
#define pinHumanR 9

#define TIME_USE_HUMAN 3
#define BLINK_COUNT 5 

char data[500]; /* <- pv char data[1000]; */
char userId[500]; /* <- pv char userId[1000]; */
char c;
bool current_state; // 
unsigned long prevTime;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello!");
  BlueTooth.begin(9600);

  pinMode(pinCarG, OUTPUT); 
  pinMode(pinCarY, OUTPUT); 
  pinMode(pinCarR, OUTPUT); 
  
  pinMode(pinHumanG, OUTPUT); 
  pinMode(pinHumanR, OUTPUT);

  initLED(); //신호등 초기화[황색등 모드]
  
  BlueTooth.println("AT+IMM1"); // 장치 검색 시작 AT커맨드 명령어
  BlueTooth.println("AT+ROLE1");
  BlueTooth.println("AT+DISI?"); 
  /*
   * AT+DISI?

   ⇒OK+DISCS

   ⇒OK+DISC: [para1]:[para2]:[para3]:[para4]:[para5] = 78(크기)

   ⇒OK+DISCE
   */

  current_state=false;
  delay(100);
  
}

void loop() {

  int checkNum=0;
  int cnt=0;
  int idcnt=0;

  /*beacon data 저장*/
  if(current_state==false){ 
    
    if(BlueTooth.available()){ //장치 검색 결과 data가 버퍼안에 있다면
      while(BlueTooth.available())
      {
        c=BlueTooth.read(); //read 1byte of beacon data in buffer
        BlueTooth.print(c); //print 1byte of beacon data 
        
        if(c==':') // if read ":"
        {
          checkNum++;
        }
        
        if(checkNum>=1||checkNum<5)
        {
          data[cnt]=c;
          cnt++;
        }
        if(checkNum==5)
        {
          for(int i=0;1<5;i++)
          {
            data[cnt]=c;
            cnt++; 
            c=BlueTooth.read();         
          }
          checkNum++;
        }
      }       
   }
   
    BlueTooth.println(" ");
    current_state=true; 
  }
  
  /*if new value input, check whether it is user ID*/
  if(BlueTooth.available() && current_state){ 
    while(BlueTooth.available()){
    userId[idcnt]=BlueTooth.read();
    idcnt++;
    }
    /*userID[] array to string*/
    String str="";
    
    for(char b=0;b<5;b++){
    str.setCharAt(b, userId[b]); 
  }
    /*ID check function start*/
    if(str.length()< MAXDATA){
      if(ID_Check(str)){
        ToPhone();

        delay(3000); // 모드 변경 [안전황색등 모드]
        digitalWrite(pinCarY, LOW);
     
        // 차량 신호등 적색등 점등
        digitalWrite(pinCarR, HIGH);
    
        //횡단보도 신호등 녹색등 점등
        delay(2000);
        digitalWrite(pinHumanG, HIGH);
        delay(TIME_USE_HUMAN * 1000);
  
        digitalWrite(pinHumanG, HIGH); 
        delay(3000);

        //횡단 녹색신호 점멸
        for(int i = 0; i < BLINK_COUNT; i++) { 
        digitalWrite(pinHumanG, LOW);
        delay(500);
        digitalWrite(pinHumanG, HIGH);
        delay(500);

        //횡단 녹색신호 점멸 연장
        if (BlueTooth.available()) { //스마트폰으로부터 연장신호를 받은 경우      
          int ext = BlueTooth.read();
          if(ext==1){
          Serial.write("연장신호 확인");
          Serial.write(data);  // 시리얼 모니터에 수신한 데이터를 출력한다

          blinkLED();
          }
          else{
            break;
          }
        }        
      }
    }    
  }       
 }
}


/* -------함수정의------*/  

/*신호등 초기화 함수 정의*/
void initLED() { 
      digitalWrite(pinCarG, LOW); 
      digitalWrite(pinCarY, HIGH);
      digitalWrite(pinCarR, LOW); 
      
      digitalWrite(pinHumanG, LOW); 
      digitalWrite(pinHumanR, HIGH); 

      
      prevTime = millis(); 
  }
  
/*사용자 아이디 확인 함수 정의*/
bool ID_Check(String ID){
  ID=="daeun0630";
  BlueTooth.println("ID Check Hello!");
  return;
  }
/*
void clearBuffer(){
  memset(list,'\0',MAXDATA);
}
*/

/* 아두이노 -> 스마트폰 ACK전송 함수*/
void ToPhone(){
  BlueTooth.println("Recive user ACK");
  BlueTooth.println("AT+MINO0x1234");
}

/*녹색신호 점멸 연장 함수*/
void blinkLED(){
    for(int i = 0; i < BLINK_COUNT; i++) { 
          digitalWrite(pinHumanG, LOW);
          delay(500);
          digitalWrite(pinHumanG, HIGH);
          delay(500);

  }
}
/*
int startPointCheck(){
  for(int i=1;i=7:i++){
    if(c==check_start[i])
    {
      c=BlueTooth.read();
      result=1;
    }
    else{
      result=0;
      break;
    }    
  }
  return result;
}
*/
