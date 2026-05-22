# 📡 Wireless Ultrasonic Radar System

![Arduino](https://img.shields.io/badge/Arduino-Uno-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![ESP8266](https://img.shields.io/badge/ESP8266-ESP--01-4C4C4C?style=for-the-badge&logo=espressif&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

> A wireless telemetry and spatial radar tracking system built using an Arduino UNO, an HC-SR04 Ultrasonic Sensor, and an ESP8266 Wi-Fi module. The system measures physical distances to target obstacles using acoustic time-of-flight principles, outputs live metrics to an integrated 16x2 physical LCD screen, and simultaneously streams live spatial data via a local HTTP micro-webserver to any connected device on the same network.

---

## 🚀 Key Features

- **Real-Time Edge Processing** – High-accuracy distance tracking using ultrasonic waveform reflection physics
- **Dual-Layer Diagnostics** – Simultaneous local visual readout (16x2 Parallel LCD Canvas) and remote wireless readout
- **Embedded Web Architecture** – Hosts a standalone HTTP server directly on the ESP8266 Wi-Fi stack
- **Robust Synchronous Architecture** – Eliminates blocking loops during local sensor operations

---

## 🛠 Hardware Architecture

### Required Bill of Materials (BOM)

| Component | Quantity | Purpose |
|:---|:---:|:---|
| Arduino UNO R3 | 1 | Main Edge Processing Microcontroller |
| ESP8266 (ESP-01) | 1 | Wireless Local Network Interface |
| HC-SR04 Ultrasonic Sensor | 1 | Acoustic Transceiver |
| 16x2 Character LCD Display | 1 | HD44780 Driver compatible |
| 10kΩ Potentiometer | 1 | LCD Contrast tuning |
| Breadboard & Jumper Wires | 1 set | Interconnections |

### 📐 Circuit Schematic

> **⚠️ CRITICAL WARNING:** The ESP8266 module runs strictly on **3.3V logic**. Connecting its VCC or RX pins directly to 5V can permanently destroy the module. Use a logic level shifter or voltage divider on the RX line (Arduino Pin 7 → ESP8266 RX).

### Wiring Connection Table

| Peripheral | Pin | Arduino Pin | Function |
|:---|:---|:---|:---|
| **HC-SR04** | VCC | 5V | Power |
| | GND | GND | Ground |
| | Trig | Pin 9 | Trigger output |
| | Echo | Pin 10 | Echo input |
| **ESP8266** | VCC | 3.3V | **3.3V only** |
| | CH_PD | 3.3V | Enable |
| | GND | GND | Ground |
| | TX | Pin 6 | SoftwareSerial RX |
| | RX | Pin 7 | SoftwareSerial TX (via divider) |
| **LCD 16x2** | VSS/VDD | GND/5V | Power |
| | VO | Potentiometer | Contrast |
| | RS | Pin 12 | Register select |
| | E | Pin 11 | Enable |
| | D4-D7 | Pins 5,4,3,2 | Data bus |


