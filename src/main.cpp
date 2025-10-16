#include <Arduino.h>
#include <M5Stack.h>
#include <esp_now.h>
#include <WiFi.h>

#define Right_1  1  //#define　プリプロセス命令
#define Right_2  3
#define Left_1  16
#define Left_2  17

int X_POS, Y_POS;

struct ControlData{
  uint8_t x_direction;
  uint8_t y_direction;
};

ControlData pixel_settings = {
  .x_direction = 1,
  .y_direction = 2
};

//OnDataRecv関数がESP-NOWでデータ受信
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  X_POS =makeWord(data[1],data[0]);
  Y_POS =makeWord(data[3],data[2]);
  Serial.print("X_POS:");
  Serial.print(X_POS);
  Serial.print("Y_POS:");
  Serial.print(Y_POS);
}

void setup() {
  M5.begin();
  M5.Lcd.setTextSize(2.0);
  M5.Lcd.setTextColor(WHITE);

  Serial.begin(115200);

  pinMode(Right_1, OUTPUT);
  pinMode(Right_2, OUTPUT);
  pinMode(Left_1, OUTPUT);
  pinMode(Left_2, OUTPUT);
  digitalWrite(Right_1, LOW);  //デジタルピンに高い電圧をかけるか、かけないか
  digitalWrite(Right_2, LOW);
  digitalWrite(Left_1, LOW);
  digitalWrite(Left_2, LOW);

  //ESP-NOWを初期化
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
}

void loop() {

  int val_1 = analogRead(X_POS);
  //前進、後退用プログラム
  if(-20 <= val_1 && val_1 <= 30){
    digitalWrite(Right_1,LOW);
    digitalWrite(Right_2,LOW);
    digitalWrite(Left_1,LOW);
    digitalWrite(Left_2,LOW);
  }else{
    if  (val_1 > 30){
    val_1 = val_1 - 31;
    digitalWrite(Right_1,HIGH);
    digitalWrite(Right_2,LOW);
    digitalWrite(Left_1,LOW);
    digitalWrite(Left_2,HIGH);
  } else {
    val_1 = 30 - val_1;
    digitalWrite(Right_1,HIGH);
    digitalWrite(Right_2,LOW);
    digitalWrite(Left_1,LOW);
    digitalWrite(Left_2,HIGH);
    }
  }

  //右旋回、左旋回用プログラム
  int val_2 = analogRead(Y_POS);
  if(-30 <= val_2 && val_2 <= 30){
    digitalWrite(Right_1,LOW);
    digitalWrite(Right_2,LOW);
    digitalWrite(Left_1,LOW);
    digitalWrite(Left_2,LOW);
  }else{
    if (val_2 > 30){
    val_2 = val_2 - 31;
    digitalWrite(Right_1,HIGH);
    digitalWrite(Right_2,LOW);
    digitalWrite(Left_1,HIGH);
    digitalWrite(Left_2,LOW);
  }else{
    val_2 = 30 - val_2;
    digitalWrite(Right_1,LOW);
    digitalWrite(Right_2,HIGH);
    digitalWrite(Left_1,LOW);
    digitalWrite(Left_2,HIGH);
    }
  }
}