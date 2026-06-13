#define REMOTEXY_MODE__ESP32CORE_BLE
#include <BLEDevice.h>
#define REMOTEXY_BLUETOOTH_NAME "RemoteXY"
#include <RemoteXY.h>
#pragma pack(push, 1)  
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =   // 37 bytes V19 
  { 255,1,0,0,0,30,0,19,0,0,0,0,31,1,106,200,1,1,1,0,
  2,30,40,44,22,0,2,26,31,31,79,78,0,79,70,70,0 };
// this structure defines all the variables and events of your control interface 
struct {
    // input variables
  uint8_t switch_01; // =1 if switch ON and =0 if OFF, from 0 to 1
    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0
} RemoteXY;   
#pragma pack(pop)

//byte redl = ; the lights, unused
//byte greenl = ;
//byte bluel = ;
//bool actflag = 0;

byte input1 = 18; //leftmost
byte input2 = 19;
//byte input3 = ; unused
byte input4 = 22;
byte input5 = 23; //rightmost

byte b = 21; //back wheels
byte fl1 = 26; //IN1
byte fl2 = 25; //IN2
byte fr1 = 33; //IN3
byte fr2 = 32; //IN4
byte spd = 130;
byte spd2 = 65;

void forth(){ //前進
  analogWrite(fl1, spd);
  digitalWrite(fl2, 0);
  analogWrite(fr1, spd);
  digitalWrite(fr2, 0);
  analogWrite(b, spd);
}

/*void back(){ //後退
  digitalWrite(fl1, 0);
  analogWrite(fl2, spd);
  digitalWrite(fr1, 0);
  analogWrite(fr2, spd);
  digitalWrite(b, 0);
}*/

void left(){ //左轉
  digitalWrite(fl1, 0);
  digitalWrite(fl2, 0); 
  digitalWrite(fr2, 0);
  analogWrite(b, spd);
  if(digitalRead(input1)) analogWrite(fr1, spd); //big turn
  else analogWrite(fr1, spd2); //small turn
}

void right(){ //右轉
  digitalWrite(fl2, 0); 
  digitalWrite(fr1, 0);
  digitalWrite(fr2, 0);
  analogWrite(b, spd);
  if(digitalRead(input5)) analogWrite(fl1, spd); //big turn
  else analogWrite(fl1, spd2); //small turn
}

void stop(){
  digitalWrite(fl1, 0);
  digitalWrite(fl2, 0); 
  digitalWrite(fr1, 0);
  digitalWrite(fr2, 0);
  digitalWrite(b, 0);
}

void setup() 
{
  RemoteXY_Init ();  // initialization by macros 
  pinMode(fl1, OUTPUT);
  pinMode(fl2, OUTPUT);
  pinMode(fr1, OUTPUT);
  pinMode(fr2, OUTPUT);
  pinMode(b, OUTPUT);

  pinMode(input1,INPUT);
  pinMode(input2,INPUT);
  //pinMode(input3,INPUT);
  pinMode(input4,INPUT);
  pinMode(input5,INPUT);

  //pinMode(redl, OUTPUT);
  //pinMode(greenl, OUTPUT);
  //pinMode(bluel, OUTPUT);
  Serial.begin(9600);
}

void loop() 
{ 
  RemoteXYEngine.handler ();
  if(RemoteXY.switch_01){ //auto
    if(!(digitalRead(input4) ^ digitalRead(input2))){
      forth();
    }
    else if(digitalRead(input2) || digitalRead(input1)){
      left();
      RemoteXYEngine.delay(80); 
    }
    else{
      right();
      RemoteXYEngine.delay(80);
    }
  }
  else stop(); //stopping
}
