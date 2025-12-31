# 🏘️ Smart Community Access and Emergency Management System

## 📌 Project Overview
The **Smart Community Access and Emergency Management System** is an IoT-based embedded solution designed to automate **community entry/exit control, emergency handling, environmental monitoring, and real-time status display**.  
It uses an **ESP32** along with multiple sensors, actuators, and the **Blynk IoT platform** to create a smart, responsive, and safe community model.

---

## 🎯 Key Features
- 🚪 **Automated Entry & Exit Gates**
  - IR sensors for accurate detection
  - Servo-controlled gates with auto-close timing
- 👥 **Real-Time Attendance Monitoring**
  - Live people count on LCD and Blynk app
- 🔥 **Fire Emergency Management**
  - Fire detection sensor
  - Automatic emergency gate opening
  - Buzzer alarm and RGB LED alert
- 🌧️ **Rain-Based Roof Automation**
  - Rain sensor–controlled stepper motor roof
  - Limit switch protection for safe movement
- 💧 **Water Pump Control**
  - Manual switch-based pump activation
- 📱 **IoT Monitoring with Blynk**
  - Attendance count
  - Fire and rain status
  - Roof control enable/disable
- 🕒 **Real-Time Clock**
  - NTP-based date and time (IST)
- 📟 **16x2 I2C LCD Display**
  - Rotating status screens every 10 seconds

---

## 🧰 Hardware Components Used
- ESP32 Development Board  
- IR Sensors (Entry & Exit)  
- Rain Sensor Module  
- Fire Sensor  
- Servo Motors (Gates & Emergency Exits)  
- Stepper Motor + Driver (Roof Control)  
- Limit Switches  
- RGB LED  
- Buzzer  
- Water Pump with Relay  
- 16x2 I2C LCD Display  
- Push Buttons / Switches  

---

## 💻 Software & Libraries
- Arduino IDE  
- ESP32 Board Package  

### Libraries:
- Blynk  
- WiFiManager  
- ESP32Servo  
- LiquidCrystal_I2C  
- NTPClient  
- Wire  

---

## 🔌 System Working
1. **Normal Operation**
   - IR sensors detect entry and exit
   - Attendance is updated on LCD and Blynk
   - Rain sensor controls roof automatically
2. **Emergency Mode**
   - Fire detection triggers emergency mode
   - Emergency gates open automatically
   - Buzzer alarm sounds
   - RGB LED turns RED
3. **Remote Monitoring**
   - Live data visible on Blynk app
   - Roof automation can be enabled or disabled remotely

---

## 📊 Blynk Virtual Pin Mapping
| Feature | Virtual Pin |
|-------|-------------|
| Attendance Count | V26 |
| Fire Status | V27 |
| Rain Status | V28 |
| Score / Message | V29 |
| Roof Control Enable | V30 |

---

## 🛠️ Setup Instructions
1. Clone the repository  
2. Open the `.ino` file in Arduino IDE  
3. Install all required libraries  
4. Update **Blynk Template ID and Auth Token**  
5. Upload the code to ESP32  
6. Power the system and connect to WiFi  

---

## 📌 Applications
- Smart Community Models  
- Smart Campus & Stadium Systems  
- IoT Mini & Major Projects  
- Smart City Demonstrations  
- Embedded Systems Learning Projects  

---

## 🚀 Future Enhancements
- RFID / Fingerprint-based access control  
- Mobile notifications for emergencies  
- Cloud data logging and analytics  
- Camera-based surveillance  
- Solar-powered system integration  

---

## 📜 License
This project is licensed under the **MIT License**.  
You are free to use, modify, and distribute this project.

---
