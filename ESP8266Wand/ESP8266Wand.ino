#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <MPU6050.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//#include <ArduinoJson.h>
// #include <tools.cpp>



// 创建MPU6050对象
MPU6050 mpu;
// 定义存储传感器数据的变量
int16_t ax, ay, az;
int16_t gx, gy, gz;

// WiFi credentials
const char* ssid = "Xiaomi_D2B3";
const char* password = "abc849849";

// Server address
const char* serverIP = "192.168.31.206";
const int serverPort = 4278;


// Buffer for xyz data
const int bufferSize = 200; // Buffer size for xyz data


int bufferIndex = 0;

struct XYZ {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
};
XYZ xyzData[bufferSize];



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

    http.begin(serverPath.c_str());
    http.addHeader("Content-Type", "application/json");


    // Create JSON string manually
    String jsonStr = "{\"xyz\":[";
    for (int i = 0; i < bufferIndex; i++) {

      jsonStr += "{\"ax\":" + String(xyzData[i].ax) + ",\"ay\":" + String(xyzData[i].ay) + ",\"az\":" + String(xyzData[i].az)
       + ",\"gx\":" + String(xyzData[i].gx)  + ",\"gy\":" + String(xyzData[i].gy)  + ",\"gz\":" + String(xyzData[i].gz)
      + "}";
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
void addXyzData(int16_t ax, int16_t ay, int16_t az,
                 int16_t gx, int16_t gy, int16_t gz) {
  if (bufferIndex < bufferSize) {
    xyzData[bufferIndex] = {ax, ay, az,
                             gx, gy, gz };
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



//Adafruit_MPU6050 mpu;
//float calculateDTW(float seq1[][3], int n, float seq2[][3], int m);
//void print2DArray(float arr[][3], int rows);
//


void setup() {
  Serial.begin(115200);
  pinMode(D3, INPUT);  // 将D口配置为输入模式
  connectToWiFi();


  // 初始化I2C通信
  Wire.begin(D6, D5); // SDA, SCL

  // 初始化MPU6050
  mpu.initialize();

  // 检查MPU6050是否正常工作
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connection successful");
  } else {
    Serial.println("MPU6050 connection failed");
  }

/*  Wire.begin(D6, D5);
  mpu.begin();
  delay(100);
//  ESP.wdtDisable();
//  // Re-enable watchdog timer with a timeout of 8 seconds
//  ESP.wdtEnable(8000); // 8000ms = 8s
  // 设置加速度计的量程范围。量程越小，测量精度越高，适用于测量较小的加速度。量程越大，能测量的最大加速度越高，适用于测量较大的加速度。
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  // 设置陀螺仪的量程范围。量程越小，测量精度越高，适用于测量较小的角速度。量程越大，能测量的最大角速度越高，适用于测量较大的角速度。
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  // 设置数字低通滤波器的带宽为21Hz。带宽越低，滤除的高频噪声越多，数据越平滑，但会引入更多的延迟。带宽越高，滤除的噪声越少，数据越敏感，但噪声也会增加。
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);*/


  delay(100);
}


void loop() {


  if (digitalRead(D3) == LOW) {
        Serial.println("按下按键");
        while(bufferIndex<bufferSize){
             mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
            addXyzData(ax, ay, az,
                        gx, gy, gz );
            delay(2);

        }
    sendXyzData();

  }


     delay(100);
}