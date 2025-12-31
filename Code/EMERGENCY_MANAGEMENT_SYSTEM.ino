/******** SMART COMMUNITY ACCESS & EMERGENCY MANAGEMENT SYSTEM ********/

#define BLYNK_TEMPLATE_ID   "TMPL3yRMYNu_K"
#define BLYNK_TEMPLATE_NAME "Smart Community System"
#define BLYNK_AUTH_TOKEN    "WFKLI2YCiouWJuEkXZjqmpto5QBcsRgx"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp32.h>
#include <NTPClient.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/************ PIN CONFIGURATION ************/
// Rain-controlled roof system
#define RAIN_PIN    35
#define SW_CLOSE    34
#define SW_OPEN     39
#define DIR         13
#define STEP        12

// Entry / Exit IR sensors
#define IR_ENTRY    33
#define IR_EXIT     26

// Servo motors
#define SERVO_ENTRY 27
#define SERVO_EXIT  14
#define SERVO_EM1   15
#define SERVO_EM2   2
#define SERVO_EM3   4
#define SERVO_EM4   5

// Fire safety
#define FIRE_PIN    19
#define BUZZER_PIN  23

// Pump (manual)
#define PUMP_SWITCH 18
#define PUMP_PIN    16

// RGB LED (Common Cathode)
#define RGB_RED     32
#define RGB_GREEN  25

/************ SERVO SETTINGS ************/
Servo entryServo, exitServo;
Servo em1, em2, em3, em4;

#define SERVO_CLOSED 0
#define SERVO_OPEN   90

/************ SYSTEM VARIABLES ************/
long occupancyCount = 0;

// Gate timing
bool entryGateActive = false, exitGateActive = false;
unsigned long entryGateTime = 0, exitGateTime = 0;
#define GATE_OPEN_DURATION 2000

// IR debounce
bool lastEntryIR = HIGH, lastExitIR = HIGH;
unsigned long lastEntryMillis = 0, lastExitMillis = 0;
#define IR_COOLDOWN 1200

// Rain & roof
#define RAIN_ON  1800
#define RAIN_OFF 2200
bool rainDetected = false;
bool roofControlEnabled = true;

// Stepper timing
#define STEP_PULSE_US    20
#define STEP_INTERVAL_US 300

// Emergency
bool emergencyActive = false;
unsigned long emergencyStart = 0;
#define EMERGENCY_DURATION 10000
unsigned long buzzerMillis = 0;
bool buzzerState = false;

/************ LCD & TIME ************/
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800);

bool wifiConnected = false;
unsigned long lastLCDUpdate = 0;
int lcdScreen = 0;

/************ BLYNK PINS ************/
#define V_OCCUPANCY V26
#define V_FIRE      V27
#define V_RAIN      V28
#define V_ROOF_EN   V30

/************ WIFI & BLYNK ************/
void setupWiFi() {
  WiFiManager wm;
  wm.setTimeout(30);

  lcd.clear();
  lcd.print("Config WiFi...");
  if (!wm.autoConnect("SmartCommunity_AP")) {
    wifiConnected = false;
    lcd.clear();
    lcd.print("Offline Mode");
    delay(2000);
  } else {
    wifiConnected = true;
    Blynk.config(BLYNK_AUTH_TOKEN);
    Blynk.connect();
  }
}

/************ RGB STATUS ************/
void setSafeMode() {
  digitalWrite(RGB_GREEN, HIGH);
  digitalWrite(RGB_RED, LOW);
}

void setEmergencyMode() {
  digitalWrite(RGB_GREEN, LOW);
  digitalWrite(RGB_RED, HIGH);
}

/************ EMERGENCY HANDLING ************/
void triggerEmergency() {
  emergencyActive = true;
  emergencyStart = millis();

  em1.write(SERVO_OPEN);
  em2.write(SERVO_OPEN);
  em3.write(SERVO_OPEN);
  em4.write(SERVO_OPEN);

  setEmergencyMode();
  if (wifiConnected) Blynk.virtualWrite(V_FIRE, "Fire: Emergency");

  lcd.clear();
  lcd.print("FIRE ALERT!");
}

void handleBuzzer() {
  if (millis() - buzzerMillis > 150) {
    buzzerMillis = millis();
    buzzerState = !buzzerState;
    digitalWrite(BUZZER_PIN, buzzerState);
  }
}

/************ ROOF CONTROL ************/
void stepMotor(bool cw) {
  digitalWrite(DIR, cw);
  digitalWrite(STEP, HIGH);
  delayMicroseconds(STEP_PULSE_US);
  digitalWrite(STEP, LOW);
  delayMicroseconds(STEP_INTERVAL_US);
}

void handleRoof() {
  int rainVal = analogRead(RAIN_PIN);
  rainDetected = (rainVal < RAIN_ON);

  if (wifiConnected)
    Blynk.virtualWrite(V_RAIN, rainDetected ? "Rain: Yes" : "Rain: No");

  if (roofControlEnabled && !emergencyActive) {
    if (rainDetected && digitalRead(SW_CLOSE) == HIGH) stepMotor(true);
    else if (!rainDetected && digitalRead(SW_OPEN) == HIGH) stepMotor(false);
  }
}

/************ ENTRY / EXIT ************/
void handleEntryExit() {
  unsigned long now = millis();

  if (digitalRead(IR_ENTRY) == LOW && lastEntryIR == HIGH && now - lastEntryMillis > IR_COOLDOWN) {
    lastEntryMillis = now;
    occupancyCount++;
    entryServo.write(SERVO_OPEN);
    entryGateActive = true;
    entryGateTime = now;
  }

  if (digitalRead(IR_EXIT) == LOW && lastExitIR == HIGH && now - lastExitMillis > IR_COOLDOWN) {
    lastExitMillis = now;
    if (occupancyCount > 0) occupancyCount--;
    exitServo.write(SERVO_OPEN);
    exitGateActive = true;
    exitGateTime = now;
  }

  lastEntryIR = digitalRead(IR_ENTRY);
  lastExitIR  = digitalRead(IR_EXIT);

  if (wifiConnected) Blynk.virtualWrite(V_OCCUPANCY, occupancyCount);
}

/************ GATE AUTO CLOSE ************/
void closeGates() {
  if (entryGateActive && millis() - entryGateTime > GATE_OPEN_DURATION) {
    entryServo.write(SERVO_CLOSED);
    entryGateActive = false;
  }
  if (exitGateActive && millis() - exitGateTime > GATE_OPEN_DURATION) {
    exitServo.write(SERVO_CLOSED);
    exitGateActive = false;
  }
}

/************ SETUP ************/
void setup() {
  Serial.begin(115200);

  pinMode(IR_ENTRY, INPUT);
  pinMode(IR_EXIT, INPUT);
  pinMode(FIRE_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PUMP_SWITCH, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);

  entryServo.attach(SERVO_ENTRY);
  exitServo.attach(SERVO_EXIT);
  em1.attach(SERVO_EM1);
  em2.attach(SERVO_EM2);
  em3.attach(SERVO_EM3);
  em4.attach(SERVO_EM4);

  lcd.init();
  lcd.backlight();
  lcd.print("Smart Community");

  setupWiFi();
  timeClient.begin();

  setSafeMode();
}

/************ LOOP ************/
void loop() {
  if (wifiConnected) {
    Blynk.run();
    timeClient.update();
  }

  if (digitalRead(FIRE_PIN) == LOW && !emergencyActive)
    triggerEmergency();

  if (emergencyActive) {
    handleBuzzer();
    if (millis() - emergencyStart > EMERGENCY_DURATION) {
      emergencyActive = false;
      digitalWrite(BUZZER_PIN, LOW);
      setSafeMode();
    }
  } else {
    handleEntryExit();
    closeGates();
    handleRoof();
    digitalWrite(PUMP_PIN, digitalRead(PUMP_SWITCH));
  }

  delay(2);
}
