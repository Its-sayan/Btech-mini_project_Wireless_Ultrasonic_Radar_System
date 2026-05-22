#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

// Assign pin layouts for the 16x2 LCD module
const int lcd_rs = 12, lcd_en = 11, lcd_d4 = 5, lcd_d5 = 4, lcd_d6 = 3, lcd_d7 = 2;
LiquidCrystal lcd(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7);

// Hardware mapping for HC-SR04 Ultrasonic sensor
const int trigPin = 9;
const int echoPin = 10;

// Setup a dedicated software serial link for ESP8266 communication (Rx, Tx)
SoftwareSerial espSerial(6, 7); 

// Network Credentials configuration parameters
const String WIFI_SSID = "YOUR_WIFI_SSID";
const String WIFI_PASS = "YOUR_WIFI_PASSWORD";

// Forward declarations for networking functions
void sendATCommand(String command, const int timeout);

void setup() {
  // Initialize standard hardware diagnostics console
  Serial.begin(9600);
  // Initialize communications bus over software-defined serial pins for ESP8266
  espSerial.begin(115200); 

  // Configure hardware pin directions for the ultrasonic telemetry system
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize display layout canvas
  lcd.begin(16, 2);
  lcd.print("Initializing...");
  delay(1000);

  // Issue low-level AT commands to establish network pairing profile
  lcd.clear();
  lcd.print("Connecting WiFi");
  
  sendATCommand("AT+RST\r\n", 2000);                          // Reset module firmware state
  sendATCommand("AT+CWMODE=1\r\n", 1000);                     // Force standard station mode profile
  sendATCommand("AT+CWJAP=\"" + WIFI_SSID + "\",\"" + WIFI_PASS + "\"\r\n", 5000); // Pair to network AP
  sendATCommand("AT+CIPMUX=1\r\n", 1000);                     // Allow concurrent multi-socket instances
  sendATCommand("AT+CIPSERVER=1,80\r\n", 1000);               // Initialize persistent HTTP listener at port 80

  lcd.clear();
  lcd.print("Radar System Ready");
  delay(1500);
}

void loop() {
  long duration;
  int distance;

  // Execute a precise 10-microsecond high pulse to trigger ultrasonic wave emissions
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Compute wave return delay period
  duration = pulseIn(echoPin, HIGH);
  
  // Convert time-of-flight latency to physical depth values (Speed of Sound = ~343 m/s)
  distance = duration * 0.0343 / 2;

  // Update physical UI layer metrics display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Radar Tracking:");
  lcd.setCursor(0, 1);
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print(" cm");

  // Network Layer Listener - Handle incoming HTTP requests
  if (espSerial.available()) {
    if (espSerial.find("+IPD,")) {
      delay(500); // Allow buffer queue to fill sequentially
      int connectionId = espSerial.read() - 48; // Convert ASCII value character offset to raw integer ID
      
      // Construct responsive raw payload strings container 
      String webpage = "<h1>Radar Telemetry Broadcast</h1>";
      webpage += "<p>Current Range Reading: <strong>" + String(distance) + " cm</strong></p>";
      
      // Format proper data transmission packet commands
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += webpage.length();
      cipSend += "\r\n";
      
      // Dispatch content across wireless medium
      sendATCommand(cipSend, 1000);
      espSerial.print(webpage);
      
      // Explicitly terminate communication sockets to clear memory structures
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId;
      closeCommand += "\r\n";
      sendATCommand(closeCommand, 1000);
    }
  }
  
  delay(200); // System cycle stabilizing delay
}

void sendATCommand(String command, const int timeout) {
  espSerial.print(command); 
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (espSerial.available()) {
      char c = espSerial.read(); 
      Serial.write(c); // Mirror diagnostic logs directly out to the main USB serial monitor
    }
  }
}
