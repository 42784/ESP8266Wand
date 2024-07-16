#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//#include <ArduinoJson.h>
// #include <tools.cpp>



// WiFi credentials
const char* ssid = "Xiaomi_D2B3";
const char* password = "abc849849";

// Server address
const char* serverIP = "192.168.31.206";
const int serverPort = 8080;


// Buffer for xyz data
const int bufferSize = 100; // Buffer size for xyz data
struct XYZ {
  float x;
  float y;
  float z;
};
XYZ xyzData[bufferSize];
int bufferIndex = 0;



// Function to connect to Wi-Fi
void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to send JSON data
void sendData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String serverPath = String("http://") + serverIP + ":" + String(serverPort) + "/pushData";

    // Specify request destination
    http.begin(serverPath.c_str());
    http.addHeader("Content-Type", "application/json");

    // Serialize JSON to string
//    String jsonStr;
//    serializeJson(jsonData, jsonStr);

    // Create JSON string manually
    String jsonStr = "{\"xyz\":[";
    for (int i = 0; i < bufferIndex; i++) {
      jsonStr += "{\"x\":" + String(xyzData[i].x) + ",\"y\":" + String(xyzData[i].y) + ",\"z\":" + String(xyzData[i].z) + "}";
      if (i < bufferIndex - 1) {
        jsonStr += ",";
      }
    }
    jsonStr += "]}";

    // Send HTTP POST request
    int httpResponseCode = http.POST(jsonStr);

    // Check the returning code
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode); // Print return code
      Serial.println(response); // Print request answer

    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    // Free resources
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

// Function to add xyz data to buffer
void addXyzData(float x, float y, float z) {
  if (bufferIndex < bufferSize) {
    xyzData[bufferIndex] = {x, y, z};
    bufferIndex++;
  } else {
    Serial.println("Buffer full, cannot add more data");
  }
}

// Function to send buffered xyz data
void sendXyzData() {
  if (bufferIndex > 0) {

    sendData();

    // Clear the buffer
    bufferIndex = 0;
  } else {
    Serial.println("No data to send");
  }
}


// 录制的数据点的数量
int REC_POINT_NUMBER = 100;
// 录制时长
int REC_TIME = 1500;

Adafruit_MPU6050 mpu;
float calculateDTW(float seq1[][3], int n, float seq2[][3], int m);
void print2DArray(float arr[][3], int rows);



void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D1);
  pinMode(D3, INPUT);  // 将D3口配置为输入模式
  mpu.begin();

  delay(1000);
  connectToWiFi();

//  ESP.wdtDisable();
//  // Re-enable watchdog timer with a timeout of 8 seconds
//  ESP.wdtEnable(8000); // 8000ms = 8s

  // 设置加速度计的量程范围。量程越小，测量精度越高，适用于测量较小的加速度。量程越大，能测量的最大加速度越高，适用于测量较大的加速度。
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  // 设置陀螺仪的量程范围。量程越小，测量精度越高，适用于测量较小的角速度。量程越大，能测量的最大角速度越高，适用于测量较大的角速度。
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  // 设置数字低通滤波器的带宽为21Hz。带宽越低，滤除的高频噪声越多，数据越平滑，但会引入更多的延迟。带宽越高，滤除的噪声越少，数据越敏感，但噪声也会增加。
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);
}


void loop() {

  int state = digitalRead(D3);  // 读取D3口的电平状态
  if (state == LOW) {
        Serial.println("按下按键");
    while(bufferIndex<bufferSize){
        // Serial.println("bufferIndex"+bufferIndex);
        // Serial.println("bufferSize"+bufferSize);
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        float x = a.acceleration.x;
        float y = a.acceleration.y;
        float z = a.acceleration.z;

//        Serial.println("accX  " + String(x));
//        Serial.println("accY  " + String(y));
//        Serial.println("accZ  " + String(z));
        addXyzData(x, y, z);
        delay(10);
    }
    sendXyzData();

  }
     delay(100);
}