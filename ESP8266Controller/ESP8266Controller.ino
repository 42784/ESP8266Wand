#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WiFi credentials
const char* ssid = "Xiaomi_D2B3";
const char* password = "abc849849";

const char* serverUrl = "http://192.168.31.161:4278/isOpen";
void setup() {
  Serial.begin(115200);
  pinMode(D3, OUTPUT);
  digitalWrite(D3, LOW); // 初始设置为低电平

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
void closeLight(){
    if(isOpenTheLight){
        isOpenTheLight = false;
        digitalWrite(D3, LOW); // 关灯，低电平
    }
}
void openLight(){
    if(!isOpenTheLight){
        isOpenTheLight = true;
        digitalWrite(D3, HIGH); // 开灯，高电平
        delay(200);
    }
}
void loop() {
    HTTPClient http;
    http.begin(serverUrl); // 启动HTTP连接
    int httpCode = http.GET(); // 发送GET请求
    if (httpCode > 0) {
        String payload = http.getString();
        if (payload == "开灯") {
            openLight();
            delay(100);
        } else {
            closeLight();
        }
    } else { //httpCode < 0 服务器异常
        closeLight();
        delay(2000);
    }
    http.end(); // 关闭HTTP连接
    delay(300);
}
