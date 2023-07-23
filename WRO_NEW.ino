#include <Servo.h>

const int FUechopin = A2;
const int FUpingpin = 8;
long FUV=0;

const int BUechopin = A0;
const int BUpingpin = 13;
long BUV=0;
long initialBUV = 0;

const int RUechopin = A4;
const int RUpingpin = 7;
long RUV=0;
long initialRUV = 0;

const int LUechopin = A1;
const int LUpingpin = 12;
long LUV=0;

Servo  servo1;
int servopin=11;
int currentAngle = 100;

const int motorB=0;
const int motorF=1;                                                                                                            
const int enb=10;

const int S0=3;
const int S1=5;
const int S2=6;
const int S3=9;
const int sensorOut=2;

int count = 0;

bool getColor(){ 
  
  int currentR=0;
  int currentB=0;
  int currentG=0;
  
   // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  currentR = pulseIn(sensorOut, LOW);
  
  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  currentG = pulseIn(sensorOut, LOW);

   // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  currentB = pulseIn(sensorOut, LOW);
//  Serial.println("R : " + String(currentR) + " | G : " + String(currentG) + " | B : " + String(currentB) );
  if( currentR > 35 ) return true;
  return false;
}

void Move( int directions , int speeds){

  switch ( directions ){
    case 2: currentAngle = 140;
            break;
    case 3: currentAngle = 75;
            break;
    default: if ( currentAngle > 110 )currentAngle = 95;
             else currentAngle = 110;
  }
  
  servo1.write(currentAngle);

  switch ( directions ){
    case 4: digitalWrite(motorF,LOW);
            digitalWrite(motorB,HIGH);
            break;
    default: digitalWrite(motorF,HIGH);
             digitalWrite(motorB,LOW);
  }
  
  analogWrite(enb,speeds);
}

long readUltrasonic(int echopin,int pingpin){
  
  long duration;
  long distance;
  digitalWrite(pingpin,LOW);
  delayMicroseconds(2);
  digitalWrite(pingpin,HIGH);
  delayMicroseconds(10);
  digitalWrite(pingpin,LOW);
  duration = pulseIn(echopin,HIGH);
  distance = duration/29/2;
  return distance;
}

bool centralised(int RUV, int LUV){
  if (( RUV - LUV > 20 ) || ( RUV - LUV < -20 )) return false;
  return true;
}

void Fix(int RUV,int LUV) {
  if( RUV - LUV > 20 ) Move(2,255);
  else Move(3,255);
}

void setup() {

//  Serial.begin(9600);
  pinMode(FUpingpin,OUTPUT);
  pinMode(FUechopin,INPUT);
  pinMode(BUpingpin,OUTPUT);
  pinMode(BUechopin,INPUT);
  pinMode(LUpingpin,OUTPUT);
  pinMode(LUechopin,INPUT);
  pinMode(RUpingpin,OUTPUT);
  pinMode(RUechopin,INPUT);
  
  servo1.attach(servopin);

  pinMode(motorF,OUTPUT);
  pinMode(motorB,OUTPUT);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  initialBUV = readUltrasonic(BUechopin,BUpingpin);
  initialRUV = readUltrasonic(RUechopin,RUpingpin);
}

void loop() {
  
  FUV = readUltrasonic(FUechopin,FUpingpin);
  BUV = readUltrasonic(BUechopin,BUpingpin);
  LUV = readUltrasonic(LUechopin,LUpingpin);
  RUV = readUltrasonic(RUechopin,RUpingpin);

  
  if( getColor() == true ){
    count++;
    delay(500);
  }

  if ( count < 12 ) {
    if ( FUV > 40){
      if( centralised(RUV,LUV) ) Move(1,255);
      else Fix(RUV,LUV);
    }
    else{
      if ( currentAngle > 110 ) Move(3,255);
      else Move(2,255);
    }
  } else{
    if ( (RUV - initialRUV > 10 || RUV - initialRUV < -10) || (BUV - initialBUV > 10 || BUV - initialBUV < -10) ){
      while(RUV - initialRUV > 10){
        Move(2,255);
        RUV = readUltrasonic(RUechopin,RUpingpin);
      }
      while(RUV - initialRUV < -10){
        Move(3,255);
        RUV = readUltrasonic(RUechopin,RUpingpin);
      }
      while(BUV - initialBUV > 10){
        Move(4,255);
        BUV = readUltrasonic(BUechopin,BUpingpin);
      }
      while(BUV - initialBUV < -10){
        Move(1,255);
        BUV = readUltrasonic(BUechopin,BUpingpin);
      }
    }
    else{
      digitalWrite(motorF,LOW);
      digitalWrite(motorB,LOW);
    }
  }

//  Serial.println(count);
  delay(200);
}
