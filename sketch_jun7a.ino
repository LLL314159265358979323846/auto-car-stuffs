#define REMOTEXY_MODE__ESP32CORE_BLE
#include <BLEDevice.h>
#define REMOTEXY_BLUETOOTH_NAME "RemoteXY"
#include <RemoteXY.h>

#pragma pack(push, 1)  
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =   // 37 bytes V19 
  { 255,1,0,0,0,30,0,19,0,0,0,0,31,1,106,200,1,1,1,0,
  2,30,40,44,22,0,2,26,31,31,79,78,0,79,70,70,0 };

struct {
  // 輸入變數 (雖然藍牙不用了，但保留 RemoteXY 結構避免編譯報錯)
  uint8_t switch_01; 
  uint8_t connect_flag;  
} RemoteXY;   
#pragma pack(pop)

// === 實體開關腳位定義 ===
const byte startSwitch = 15; // 開關接在 GPIO 15 與 GND 之間

// === 原本的循跡感測器腳位 ===
byte input1 = 18; // 最左
byte input2 = 19;
byte input4 = 22;
byte input5 = 23; // 最右

// === 原本的馬達驅動腳位 ===
byte fl1 = 26; // IN1
byte fl2 = 25; // IN2
byte fr1 = 33; // IN3
byte fr2 = 32; // IN4
byte lpwm = 27;
byte rpwm = 14;
byte spd = 130;
byte spd2 = 65;

void forth(){ // 前進
  digitalWrite(fl1, 1);
  digitalWrite(fl2, 0);
  digitalWrite(fr1, 1);
  digitalWrite(fr2, 0);
  analogWrite(lpwm, spd);
  analogWrite(rpwm, spd);
}

void left(){ // 左轉
  digitalWrite(fr1, 1);
  digitalWrite(fr2, 0);
  analogWrite(lpwm, 0);
  if(digitalRead(input1)) analogWrite(rpwm, spd); // 大轉
  else analogWrite(rpwm, spd2); // 小轉
}

void right(){ // 右轉
  digitalWrite(fl1, 1); 
  digitalWrite(fl2, 0); 
  analogWrite(rpwm, 0);
  if(digitalRead(input5)) analogWrite(lpwm, spd); // 大轉
  else analogWrite(lpwm, spd2); // 小轉
}

void stop(){
  digitalWrite(fl1, 0);
  digitalWrite(fl2, 0);
  digitalWrite(fr1, 0);
  digitalWrite(fr2, 0);
  analogWrite(lpwm, 0);
  analogWrite(rpwm, 0);
}

void setup() 
{
  RemoteXY_Init();  
  
  // 設定實體開關為 INPUT_PULLUP (內建上拉電阻)
  pinMode(startSwitch, INPUT_PULLUP);

  pinMode(fl1, OUTPUT);
  pinMode(fl2, OUTPUT);
  pinMode(fr1, OUTPUT);
  pinMode(fr2, OUTPUT);
  pinMode(lpwm, OUTPUT);
  pinMode(rpwm, OUTPUT);

  pinMode(input1, INPUT);
  pinMode(input2, INPUT);
  pinMode(input4, INPUT);
  pinMode(input5, INPUT);

  Serial.begin(9600);
  stop(); // 初始狀態先停車
}

void loop() 
{ 
  RemoteXYEngine.handler(); // 雖然不用藍牙，但保留它維持底層運行避免報錯

  // 讀取實體開關狀態。因為是上拉電阻，當開關導通(ON)接到GND時會是 LOW
  if (digitalRead(startSwitch) == LOW) { 
    
    // === 進入你原本的自動循跡邏輯 ===
    if (digitalRead(input2) || digitalRead(input1)) {
      left();
      RemoteXYEngine.delay(80); 
    }
    else if (digitalRead(input4) || digitalRead(input5)) {
      right();
      RemoteXYEngine.delay(80);
    }
    else {
      forth();
    }
    
  } 
  else { 
    // 開關切到 OFF 時，強制停車
    stop(); 
  }
}
