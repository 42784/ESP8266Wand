#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WiFi credentials
const char* ssid = "Xiaomi_D2B3";
const char* password = "abc849849";

const char* serverUrl = "http://192.168.31.206:8080/isOpen"; 
void setup() {
  Serial.begin(115200);
  pinMode(D2, OUTPUT);
  digitalWrite(D2, LOW); // 初始设置为低电平

  ESP.wdtDisable();
//  // Re-enable watchdog timer with a timeout of 8 seconds
//  ESP.wdtEnable(8000); // 8000ms = 8s

  // 连接Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
bool isOpenTheLight = false;
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl); // 启动HTTP连接
    int httpCode = http.GET(); // 发送GET请求

    if (httpCode > 0) {
      String payload = http.getString();
      // Serial.println("HTTP Response code: ");
      // Serial.println(httpCode);
      // Serial.println("Payload: ");
      // Serial.println(payload);
      
       delay(100);
      if (payload == "开灯") {
        if(!isOpenTheLight){
          isOpenTheLight = true;
          digitalWrite(D2, HIGH); // 开灯，高电平
        }
      } else {
        if(isOpenTheLight){
          isOpenTheLight = false;
        digitalWrite(D2, LOW); // 关灯，低电平
        }
      }
    } else {
        isOpenTheLight = false;
      Serial.println("Error on HTTP request");
      digitalWrite(D2, LOW); // 请求失败，低电平
    }

    http.end(); // 关闭HTTP连接
  } else {
     delay(150);
    Serial.println("WiFi not connected");
    if(isOpenTheLight){
      isOpenTheLight = false;
        digitalWrite(D2, LOW); // 关灯，低电平
    }
  }

  ESP.wdtDisable();
  delay(300);
  
}
