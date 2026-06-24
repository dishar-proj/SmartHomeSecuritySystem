#define BLYNK_TEMPLATE_ID "TMPL2d5riqVUW"
#define BLYNK_TEMPLATE_NAME "HomeSecurity"
#define BLYNK_AUTH_TOKEN "0XxiV5ulJgdbsCOZ0eNay_a6XSxays_T"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
//connect to wifi
char ssid[] = "TMOBILE-9FD1";
char pass[] = "underrate.recount.agency.abstract";

#define LED_PIN V3 //IoT

//Sensor 1: LDR
const int inputPin = 36;   // ADC1 (GPIO36)
const float thresholdLDR = 1.8; // Point between 0.6V and 2.3V
const float vref = 3.3;   // Nominal reference 
int rawValue;
float voltage;
int ldrDetect;

// Sensor 2: Ultrasonic 
long duration;
int distance;
const int trigPin = 13; // Input pin
const int echoPin = 14; // Output pin
const int detectThreshold = 80; // distance (cm)
int usDetect;

// Sensor 3: FSR
const int fsrPin = 32; // GPIO 32 (ADC1_CH4)
const float thresholdFSR = 1.5;
int rawFSR;
float voltageFSR;
int fsrDetect;

// Kalman Filter
float A = 0.9;      // State transition
float Q = 0.05;     // Process noise
float x_est = 0.0;  // Initial state
float P = 1.0;      // Initial error covariance
float R_LDR = 0.3; 
float R_US = 0.3;
float R_FSR = 0.25;

// Alarm
const float triggerThreshold = 0.45; 
const float clearThreshold = 0.3; 
bool alarmTriggered = false;
const int buzzerPin = 5;

void setup() {
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); // IoT
  pinMode(buzzerPin, OUTPUT); // buzzer

  //Sensor 1 & 3: LDR & FSR
  Serial.begin(115200); //Must match serial monitor
  analogReadResolution(12); // 0–4095 range

  //Sensor 2: Ultrasonic Sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // IoT
  Blynk.run();

  // Sensor 1: LDR
  rawValue = analogRead(inputPin); //read the voltage through ADC
  voltage = (rawValue / 4095.0) * vref; //calculate voltage based on ADC value
 
  if (voltage > thresholdLDR) {
    ldrDetect = 1;
  } else {
    ldrDetect = 0;
  }

  //Sensor 2: Ultrasonic 
  digitalWrite(trigPin, LOW); // make sure input starts at low
  delayMicroseconds(2); // stabilize signal 
  digitalWrite(trigPin, HIGH); // section of code sends 10us pulse
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH); //amount of time echo takes to return
  distance = duration * 0.034/2; //calculate distance
  //correction factor based on calibration
  if (distance <= 10) { 
    distance = distance + 1;
  }

  if (distance > 0 && distance < detectThreshold) { 
    usDetect = 1;
  } else {
    usDetect = 0;
  }

  // Sensor 3: FSR
    rawFSR = analogRead(fsrPin);
    voltageFSR = (rawFSR / 4095.0) * vref;

    if (voltageFSR > thresholdFSR) {
      fsrDetect = 1; 
    } else {
      fsrDetect = 0;
    }

  // Kalman Filter Implementation
  float x_pred = A * x_est;
  float P_pred = A * P * A + Q;

  // Update with LDR
  float K1 = P_pred / (P_pred + R_LDR);
  x_est = x_pred + K1 * (ldrDetect - x_pred);
  P_pred = (1 - K1) * P_pred;

  // Update with Ultrasonic
  float K2 = P_pred / (P_pred + R_US);
  x_est = x_est + K2 * (usDetect - x_est);
  P_pred = (1 - K2) * P_pred;

  // Update with FSR
  float K3 = P_pred / (P_pred + R_FSR);
  x_est = x_est + K3 * (fsrDetect - x_est);
  P = (1 - K3) * P_pred;

  // IoT
  Blynk.virtualWrite(V0, ldrDetect);
  Blynk.virtualWrite(V1, usDetect);
  Blynk.virtualWrite(V2, fsrDetect);  

if (!alarmTriggered && x_est > triggerThreshold) { 
  alarmTriggered = true;
  Serial.println("🚨 Alarm Triggered!");
  Blynk.virtualWrite(LED_PIN, 255);  // LED ON
  Blynk.setProperty(LED_PIN, "color", "#FF0000"); // Red
  Blynk.logEvent("intruder_alert", "Intruder Detected!");
  digitalWrite(buzzerPin, HIGH);  // Turn buzzer ON
  delay(1000);                     // Wait 1 sec
  digitalWrite(buzzerPin, LOW); // Turn buzzer OFF
} else if (alarmTriggered && x_est < clearThreshold) {
  alarmTriggered = false;
  Serial.println("✅ Alarm Cleared.");
  Blynk.virtualWrite(LED_PIN, 255);  // LED ON
  Blynk.setProperty(LED_PIN, "color", "#00FF00"); // Green
}
  delay(50);  
}

