#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_BMP085_U.h>
#include <DHT.h>

// Sensor Pins
#define MQ5_PIN A0
#define MQ3_PIN A1
#define DHT_PIN 2
#define BUZZER_PIN 3
#define LED_PIN 4  // Define LED pin

// Thresholds
#define MQ5_THRESHOLD 100
#define MQ3_THRESHOLD 300
#define DHT11_TEMP_THRESHOLD 40
#define DHT11_HUMIDITY_THRESHOLD 90
#define BMP180_PRESSURE_THRESHOLD 1100
#define FALL_THRESHOLD_X -10
#define FALL_THRESHOLD_Y -1.55
#define FALL_THRESHOLD_Z 50

// Buzzer frequencies for different alerts
#define BUZZER_TEMP 500
#define BUZZER_HUMIDITY 600
#define BUZZER_PRESSURE 700
#define BUZZER_MQ5 800
#define BUZZER_MQ3 900
#define BUZZER_FALL 1000

// Create sensor objects
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
DHT dht(DHT_PIN, DHT11);

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);  // Initialize LED pin

  // Initialize ADXL345
  if(!accel.begin()) {
    Serial.println("No ADXL345 detected ... Check your wiring!");
    while(1);
  }
  accel.setRange(ADXL345_RANGE_16_G);
  
  // Initialize BMP180
  if(!bmp.begin()) {
    Serial.print("No BMP180 detected ... Check your wiring!");
    while(1);
  }
  
  // Initialize DHT11
  dht.begin();
  
  Serial.println("Sensors initialized!");
}

void loop() {
  String data = "";
  bool alert = false;

  // Read DHT11 temperature and humidity
  float dhtTemperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  if (!isnan(dhtTemperature) && !isnan(humidity)) {
    data += "DHT11_Temperature," + String(dhtTemperature);
    data += ",DHT11_Humidity," + String(humidity);
    if (dhtTemperature > DHT11_TEMP_THRESHOLD) {
      tone(BUZZER_PIN, BUZZER_TEMP, 500);
      alert = true;
    }
    if (humidity > DHT11_HUMIDITY_THRESHOLD) {
      tone(BUZZER_PIN, BUZZER_HUMIDITY, 500);
      alert = true;
    }
  } else {
    Serial.println("Failed to read from DHT sensor!");
  }

  // Read BMP180 pressure
  sensors_event_t pressureEvent;
  bmp.getEvent(&pressureEvent);
  if (pressureEvent.pressure) {
    data += ",BMP180_Pressure," + String(pressureEvent.pressure);
    if (pressureEvent.pressure > BMP180_PRESSURE_THRESHOLD) {
      tone(BUZZER_PIN, BUZZER_PRESSURE, 500);
      alert = true;
    }
  }

  // Read MQ5
  int mq5Value = analogRead(MQ5_PIN);
  data += ",MQ5," + String(mq5Value);
  if (mq5Value > MQ5_THRESHOLD) {
    tone(BUZZER_PIN, BUZZER_MQ5, 500);
    alert = true;
  }

  // Read MQ3
  int mq3Value = analogRead(MQ3_PIN);
  data += ",MQ3," + String(mq3Value);
  if (mq3Value > MQ3_THRESHOLD) {
    tone(BUZZER_PIN, BUZZER_MQ3, 500);
    alert = true;
  }

  // Read ADXL345 for fall detection and get x, y, z values
  sensors_event_t event;
  accel.getEvent(&event);
  
  float x = event.acceleration.x;
  float y = event.acceleration.y;
  float z = event.acceleration.z;

  // Check for fall detection
  if (x > FALL_THRESHOLD_X || y > FALL_THRESHOLD_Y || z > FALL_THRESHOLD_Z) {
    data += ",Fall Detected";
    tone(BUZZER_PIN, BUZZER_FALL, 500);
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    alert = true;
  }

  // If no alert was triggered, turn off the buzzer and LED
  if (!alert) {
    noTone(BUZZER_PIN);
    digitalWrite(LED_PIN, LOW);  // Turn off LED
  }

  Serial.println(data);
  delay(2000);
}
