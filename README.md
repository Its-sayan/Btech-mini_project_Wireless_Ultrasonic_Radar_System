
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

---

## 💻 Firmware Source Code

Create a file named `WirelessRadar.ino` in your Arduino IDE:

```cpp
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

// LCD pin assignments
const int lcd_rs = 12, lcd_en = 11, lcd_d4 = 5, lcd_d5 = 4, lcd_d6 = 3, lcd_d7 = 2;
LiquidCrystal lcd(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7);

// Ultrasonic pins
const int trigPin = 9;
const int echoPin = 10;

// ESP8266 SoftwareSerial (RX, TX)
SoftwareSerial espSerial(6, 7);

// ⚠️ UPDATE THESE WITH YOUR NETWORK CREDENTIALS
const String WIFI_SSID = "YOUR_WIFI_SSID";
const String WIFI_PASS = "YOUR_WIFI_PASSWORD";

void executeATCommand(String command, const int timeoutDuration);

void setup() {
  Serial.begin(9600);
  espSerial.begin(115200);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  lcd.begin(16, 2);
  lcd.print("System Reboot...");
  delay(1500);
  
  lcd.clear();
  lcd.print("Pairing WiFi...");
  
  // ESP8266 AT commands
  executeATCommand("AT+RST\r\n", 2000);
  executeATCommand("AT+CWMODE=1\r\n", 1500);
  executeATCommand("AT+CWJAP=\"" + WIFI_SSID + "\",\"" + WIFI_PASS + "\"\r\n", 6000);
  executeATCommand("AT+CIPMUX=1\r\n", 1500);
  executeATCommand("AT+CIPSERVER=1,80\r\n", 1500);
  
  lcd.clear();
  lcd.print("Radar Status:");
  lcd.setCursor(0, 1);
  lcd.print("ONLINE & ARMED");
  delay(2000);
}

void loop() {
  long acousticDuration;
  int calculatedDistance;
  
  // Trigger ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  acousticDuration = pulseIn(echoPin, HIGH);
  calculatedDistance = acousticDuration * 0.0343 / 2;
  
  if (calculatedDistance > 400 || calculatedDistance <= 0) {
    calculatedDistance = -1;
  }
  
  // Update LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Target Distance:");
  lcd.setCursor(0, 1);
  if (calculatedDistance == -1) {
    lcd.print("OUT OF BOUNDS");
  } else {
    lcd.print(">> ");
    lcd.print(calculatedDistance);
    lcd.print(" cm");
  }
  
  // Handle web requests
  if (espSerial.available()) {
    if (espSerial.find("+IPD,")) {
      delay(400);
      int connectionChannelId = espSerial.read() - 48;
      
      String htmlPayload = buildHTMLPage(calculatedDistance);
      
      String transmissionCommand = "AT+CIPSEND=" + String(connectionChannelId) + "," + htmlPayload.length() + "\r\n";
      executeATCommand(transmissionCommand, 1000);
      espSerial.print(htmlPayload);
      
      String socketTerminationCommand = "AT+CIPCLOSE=" + String(connectionChannelId) + "\r\n";
      executeATCommand(socketTerminationCommand, 1000);
    }
  }
  
  delay(1500);
}

String buildHTMLPage(int distance) {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta http-equiv='refresh' content='2'>";
  html += "<title>Wireless Radar Telemetry</title>";
  html += "<style>";
  html += "body{font-family:'Segoe UI',Arial,sans-serif;background-color:#0f172a;color:#f8fafc;text-align:center;padding:50px;}";
  html += ".card{background:#1e293b;border-radius:12px;padding:30px;display:inline-block;box-shadow:0 10px 15px -3px rgba(0,0,0,0.5);border:1px solid #334155;}";
  html += "h1{color:#38bdf8;margin-bottom:10px;font-size:2.2rem;}";
  html += ".metric{font-size:3.5rem;font-weight:bold;color:#4ade80;margin:20px 0;font-family:monospace;}";
  html += ".outrange{color:#ef4444;}";
  html += "</style></head>";
  html += "<body><div class='card'><h1>📡 Wireless Radar System</h1><p>Real-Time Spatial Telemetry Dashboard</p><hr>";
  
  if (distance == -1) {
    html += "<div class='metric outrange'>CLEAR</div><p>No obstacle detected within range.</p>";
  } else {
    html += "<div class='metric'>" + String(distance) + " <span style='font-size:1.5rem;'>cm</span></div>";
    html += "<p>Target tracking distance localized.</p>";
  }
  
  html += "<p style='font-size:0.8rem;margin-top:20px;'>Auto-refreshes every 2 seconds</p>";
  html += "</div></body></html>";
  
  return html;
}

void executeATCommand(String command, const int timeoutDuration) {
  espSerial.print(command);
  long int temporalBenchmark = millis();
  while ((temporalBenchmark + timeoutDuration) > millis()) {
    while (espSerial.available()) {
      Serial.write(espSerial.read());
    }
  }
}
