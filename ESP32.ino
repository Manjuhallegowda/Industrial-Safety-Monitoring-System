#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3O4l0ltx9"
#define BLYNK_TEMPLATE_NAME "IMS"
#define BLYNK_AUTH_TOKEN "KuTcp_7XUtKT98lPrhasV8BIDDN6hCM-"
#define RX_PIN 16 // Connect Arduino's TX pin to this RX pin on ESP32
#define TX_PIN 17 // Connect Arduino's RX pin to this TX pin on ESP32
#include <BlynkSimpleEsp32.h>
#include<WiFi.h>

// Set ESP32 Serial pins
#define RX_PIN 16 // Connect Arduino's TX pin to this RX pin on ESP32
#define TX_PIN 17 // Connect Arduino's RX pin to this TX pin on ESP32

char auth[] = "KuTcp_7XUtKT98lPrhasV8BIDDN6hCM-"; // Blynk auth token
char ssid[] = "username"; // WiFi SSID
char pass[] = "**********"; // WiFi password

// Pins for sensor data
#define VIRTUAL_PIN_TEMP 0
#define VIRTUAL_PIN_HUMIDITY 1
#define VIRTUAL_PIN_PRESSURE 2
#define VIRTUAL_PIN_MQ5 3
#define VIRTUAL_PIN_MQ3 4

void setup() {
  Serial.begin(9600);
  Serial.begin(9600, SERIAL_8N1, 16, 17); // Start ESP32 Serial for communication with Arduino
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n'); // Read data from Arduino
    Serial.println(data);
    // Parse the data string and send values to Blynk
    parseData(data);
  }
  Blynk.run();
}

void parseData(String data) {
  // Split the string by commas
  int index = 0;
  String sensorType = "";
  String sensorValue = "";
  while ((index = data.indexOf(',')) != -1) {
    sensorType = data.substring(0, index);
    data = data.substring(index + 1);
    index = data.indexOf(',');
    sensorValue = data.substring(0, index);
    data = data.substring(index + 1);
    
    // Send sensor data to Blynk
    if (sensorType == "DHT11_Temperature") {
      Blynk.virtualWrite(VIRTUAL_PIN_TEMP, sensorValue.toFloat());
    } else if (sensorType == "DHT11_Humidity") {
      Blynk.virtualWrite(VIRTUAL_PIN_HUMIDITY, sensorValue.toFloat());
    } else if (sensorType == "BMP180_Pressure") {
      Blynk.virtualWrite(VIRTUAL_PIN_PRESSURE, sensorValue.toFloat());
    } else if (sensorType == "MQ5") {
      Blynk.virtualWrite(VIRTUAL_PIN_MQ5, sensorValue.toInt());
    } else if (sensorType == "MQ3") {
      Blynk.virtualWrite(VIRTUAL_PIN_MQ3, sensorValue.toInt());
    }
  }
}
