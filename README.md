
# 📡 Wireless Ultrasonic Radar System

![Arduino](https://img.shields.io/badge/Arduino-Uno-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![ESP8266](https://img.shields.io/badge/ESP8266-ESP--01-4C4C4C?style=for-the-badge&logo=espressif&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

> A wireless telemetry and spatial radar tracking system built using an Arduino UNO, an HC-SR04 Ultrasonic Sensor, and an ESP8266 Wi-Fi module. The system measures physical distances to target obstacles using acoustic time-of-flight principles, outputs live metrics to an integrated 16x2 physical LCD screen, and simultaneously streams live spatial data via a local HTTP micro-webserver to any connected device on the same network.

---

## 🚀 Key Features

* **Real-Time Edge Processing** – High-accuracy distance tracking using ultrasonic waveform reflection physics.
* **Dual-Layer Diagnostics** – Simultaneous local visual readout (16x2 Parallel LCD Canvas) and remote wireless web telemetry.
* **Embedded Web Architecture** – Hosts a standalone HTTP server directly on the ESP8266 Wi-Fi stack serving asynchronous requests.
* **Hardware-Level Protection** – Integrated logic level conversion to safeguard low-voltage 3.3V CMOS components.

---

## 🏗 System Architecture & Data Flow

The system architecture cleanly decouples the physical environment sensing layer from the local and remote presentation layers. The core logic executes on the Arduino Uno, while network tasks are delegated entirely to the hardware TCP/IP stack of the Wi-Fi module.




[ Physical Target ] <--- Acoustic Waves ---> [ HC-SR04 ] ---> [ Arduino Uno ] ---> [ ESP8266 ] ---> [ Remote Client Browser ]
|
+-------------> [ 16x2 LCD Local Screen ]


### Visual Architecture Block Diagram

![Data Flow Diagram](results/Data%20Flow%20Diagram.png)

### Architecture Description
* **Physical Layer:** The system establishes a spatial tracking region by emitting high-frequency acoustic waves. When an obstacle interrupts this field, the reflected compression waves are captured by the receiving transducer.
* **Edge Processing:** The central ATmega328P microcontroller manages precise task scheduling. It acts as the operational bridge, triggering hardware interrupts to measure return pulses, parsing the values into distance arrays, and concurrently outputting data across multiple communication buses.
* **Wireless Presentation Layer:** Data is serialized and transmitted over UART using targeted AT commands to the ESP8266 transceiver. The ESP-01 module acts as an isolated micro-webserver, processing incoming socket requests from networked user devices and serving a live tracking dashboard layout via basic HTTP packets.

---

## 🛠 Hardware Architecture

### Required Bill of Materials (BOM)

| Component | Quantity | Purpose |
|:---|:---:|:---|
| Arduino UNO R3 | 1 | Main Edge Processing Microcontroller |
| ESP8266 (ESP-01) | 1 | Wireless Local Network Interface Module |
| HC-SR04 Ultrasonic Sensor | 1 | Acoustic Transceiver (Radar Head) |
| 16x2 Character LCD Display | 1 | HD44780 Driver compatible display |
| 10kΩ Potentiometer | 1 | LCD Contrast analog tuning |
| Resistors (1kΩ, 2.2kΩ, 220Ω) | 1 ea | Voltage dividing & LED current limiting |
| Breadboard & Jumper Wires | 1 set | Physical component interconnections |

### 📐 Wiring Connection Reference

> **⚠️ CRITICAL WARNING:** The ESP8266 module runs strictly on **3.3V logic**. Connecting its VCC or RX pins directly to a 5V rail/pin will permanently destroy the module. A voltage divider (1kΩ and 2.2kΩ) is implemented on the Arduino TX Line (Pin 7) to drop the signal safely down to ~3.43V.

| Peripheral Module | Physical Pin | Arduino Uno Pin | Wire Color Code | Function Reference |
|:---|:---|:---|:---|:---|
| **HC-SR04** | VCC | 5V | Red | Main Supply |
| | GND | GND | Black | Common Ground |
| | Trig | Pin 9 | Yellow | Trigger Output Pulse |
| | Echo | Pin 10 | Green | Echo Input Capture |
| **ESP8266 (ESP-01)**| VCC / CH_PD | 3.3V | Red / Orange | Core Power & Hardware Enable |
| | GND | GND | Black | Common Ground |
| | TX | Pin 6 | Blue | SoftwareSerial RX Node |
| | RX | Voltage Divider| Purple | SoftwareSerial TX (Safe Level Shifting) |
| **LCD 16x2 Canvas** | VSS / VDD | GND / 5V | Black / Red | Module Power Rails |
| | VO | Potentiometer | Varied | Contrast Adjustment wiper |
| | RS / E | Pin 12 / Pin 11 | White / Gray | Register Select & Enable Strobing |
| | D4, D5, D6, D7| Pins 5, 4, 3, 2 | Brn/Orng/Yel/Grn | 4-Bit Parallel Data Bus |
| | LED+ / LED- | 220Ω to 5V / GND| Red / Black | Backlight Control Circuit |

### Visual Wiring Diagram

![Visual Circuit Diagram](results/Visual%20Circuit%20Diagram.png)

### Circuit Integration Layout Description
* **Power Distribution Buses:** The breadboard utilizes two isolated voltage tracks. The upper bus is assigned to the 3.3V rail supplying the ESP8266 Wi-Fi transceiver, while the lower rail provides 5V to run the high-current components including the parallel LCD backlight and the ultrasonic transducer array.
* **Inline Attenuation Network:** Positioned right before the ESP-01 RX input pin, an asymmetrical series/parallel resistor configuration functions as an active voltage divider. This reduces the raw 5V logic transient spikes originating from the Arduino TX pin down to a safe 3.3V CMOS-compatible signal, averting long-term electrical breakdown.

---

## ⏱ Physical Principles & Timing Dynamics

Distance calculation depends entirely on the Time-of-Flight (ToF) of an ultrasonic pulse traveling through air at the speed of sound ($v \approx 343 \text{ m/s}$ at 20°C). 

### Mathematical Formulation
Because the acoustic burst travels outward to the target and reflects back to the transducer, the singular distance $d$ is derived by dividing the total round-trip time by 2:

$$Distance = \frac{\text{Pulse Width }(\mu\text{s}) \times 0.0343 \text{ cm/}\mu\text{s}}{2}$$

### Digital Waveform & Logic State Analysis

![Timing & Signal Waveform Diagram](results/Timing%20%26%20Signal%20Waveform%20Diagram.png)

### Operational Timing Sequence Description
1. **Trigger Induction:** The master control routine raises the Trigger Pin (Pin 9) from 0V to 5V for a distinct 10-microsecond interval. This baseline window forces the internal sensor ASIC to wake up from idle mode and prep its oscillator circuit.
2. **Sonic Emission & Echo Assertion:** Upon the falling edge of the trigger pulse, the sensor transmits a localized burst consisting of exactly eight distinct 40kHz sonic waves. Simultaneously, the sensor pulls its Echo output line (Pin 10) to a high state (5V). 
3. **Echo Deflection & Conversion:** The Echo pin remains locked high while the sound waves traverse the open physical space. When the acoustic wave encounters an obstacle, it deflects back to the microphone receiver. The moment this reflection is registered, the Echo line drops back to 0V. The total time duration (ToF) of this high pulse is interpreted by the microcontroller to compute and update the runtime variables.

