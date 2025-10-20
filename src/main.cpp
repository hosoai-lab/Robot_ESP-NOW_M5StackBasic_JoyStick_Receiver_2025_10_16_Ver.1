#include <Arduino.h>
#include <M5Stack.h>
#include <esp_now.h>
#include <WiFi.h>

#define Right_1  1  //#define　プリプロセス命令
#define Right_2  3
#define Left_1  16
#define Left_2  17

//ジョイスティックデータ
int X_POSITION, Y_POSITION;

typedef struct {
  int x_direction;
  int y_direction;
} joystick_data;

joystick_data joyData;

//入力値vの絶対値がデッドゾーンの値dzより小さいかを判断している。
//もし小さければ、値0を返し、そうでなければ入力値を返す。
static inline int Deadzone(int v, int dz){
  if (abs(v) < dz) return 0;
  return v;
}

//モーター設定を目的とした関数
void setMotor(int Right, int Left){
  //constrain関数は数値を特定の範囲内に収めるために使用する。
  //constrain関数　constrain(数値、最小値、最大値);
  Right = constrain(Right, -255, 255);
  Left = constrain(Left, -255, 255);

  //右モーター
  if (Right >= 0){
    ledcWrite(0, Right);
    ledcWrite(1, 0);
  }else {
    ledcWrite(0, 0);
    ledcWrite(1, -Right);
  }

  //左モーター
  if (Left >= 0){
    ledcWrite(2, Left);
    ledcWrite(3, 0);
  }else {
    ledcWrite(2, 0);
    ledcWrite(3, -Left);
  }
}

//OnDataRecv関数がESP-NOWでデータ受信
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  memcpy(&joyData, data, sizeof(joyData));

  //map関数を用いて0から4095相当から-255から255相当へ
  int xSpeed = map(joyData.x_direction, 0, 4095, -255, 255);
  int ySpeed = map(joyData.y_direction, 0, 4095, -255, 255);

  xSpeed = Deadzone(xSpeed, 20);
  ySpeed = Deadzone(xSpeed, 20);

  int rightMotor = ySpeed - xSpeed;
  int leftMotor = ySpeed + xSpeed;

  rightMotor = constrain(rightMotor, -255, 255);
  leftMotor = constrain(leftMotor, -255, 255);

  setMotor(rightMotor, leftMotor);
}

void setup() {
  M5.begin();
  M5.Lcd.setTextSize(2.0);
  M5.Lcd.setTextColor(WHITE);

  Serial.begin(115200);

  //モータードライバー_PWM制御セットアップ
  pinMode(Right_1, OUTPUT);
  pinMode(Right_2, OUTPUT);
  pinMode(Left_1, OUTPUT);
  pinMode(Left_2, OUTPUT);

  //ESP-NOWを初期化処理
  WiFi.mode(WIFI_STA);
  if(esp_now_init() != ESP_OK){
    Serial.println("Error initializing ESP-NOW");
    M5.Lcd.setTextSize(2.0);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(0,0);
    M5.Lcd.print("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv); 

  delay(500);
}

void loop() {
  
}