#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <DHT.h>

// PIN DEFINITIONS
#define SS_PIN 10
#define RST_PIN 9
#define BTN_REG 2      
#define BUZZER 6
#define RED_LED 7
#define ACCESS_GREEN 8
#define SIMPLE_GREEN A3
#define TOUCH_SENSE 4
#define DHT_PIN 5
#define MQ6_PIN A0
#define FLAME_PIN A1

MFRC522 rfid(SS_PIN, RST_PIN);
DHT dht(DHT_PIN, DHT11);
Servo mainDoor;
Servo garageGate;

// LOGIC VARIABLES
byte masterUID[4];
bool hasMaster = false;
bool lastTouchState = LOW;
bool simpleLightState = LOW;
bool dangerActive = false;

void setup() {
  Serial.begin(9600); 
  pinMode(BTN_REG, INPUT_PULLUP);
  pinMode(TOUCH_SENSE, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(ACCESS_GREEN, OUTPUT);
  pinMode(SIMPLE_GREEN, OUTPUT);
  
  SPI.begin();
  rfid.PCD_Init();
  mainDoor.attach(3);
  garageGate.attach(A2); 
  mainDoor.write(0);
  garageGate.write(0);
  dht.begin();
  
  Serial.println("BT_MSG: System Online. Ready.");
}

void loop() {
  // 1. CRITICAL SENSOR CHECK (ADC)
  int gasLevel = analogRead(MQ6_PIN);
  int flameLevel = analogRead(FLAME_PIN);

  if (gasLevel > 400 || flameLevel < 500) {
    if (!dangerActive) {
      Serial.println("BT_MSG: ! DANGER DETECTED !");
      dangerActive = true;
    }
    handleDanger(); 
    return; // Block everything else during danger
  } else if (dangerActive) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);
    Serial.println("BT_MSG: Threat Cleared. System Safe.");
    dangerActive = false;
  }

  // 2. REGISTRATION BUTTON (Blocking Store Logic)
  if (digitalRead(BTN_REG) == LOW) {
    Serial.println("BT_MSG: BUTTON DETECTED.");
    Serial.println("BT_MSG: Mode: STORE. Scan card now...");
    
    bool cardFound = false;
    while (!cardFound) {
      if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        memcpy(masterUID, rfid.uid.uidByte, 4);
        hasMaster = true;
        cardFound = true;
        Serial.println("BT_MSG: SUCCESS! Master Card Saved.");
        blinkLED(ACCESS_GREEN, 3);
      }
      if (analogRead(MQ6_PIN) > 400) break; 
    }
    rfid.PICC_HaltA();
    delay(500);
  }

  // 3. BLUETOOTH COMMANDS
  checkBluetooth();

  // 4. TOUCH SENSOR TOGGLE
  handleTouchToggle();

  // 5. NORMAL SCANNING MODE
  if (hasMaster) {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
      if (memcmp(rfid.uid.uidByte, masterUID, 4) == 0) {
        accessGranted();
      } else {
        accessDenied();
      }
      rfid.PICC_HaltA();
    }
  }
}

// --- HELPER FUNCTIONS ---

void handleTouchToggle() {
  bool currentTouch = digitalRead(TOUCH_SENSE);
  if (currentTouch == HIGH && lastTouchState == LOW) {
    simpleLightState = !simpleLightState;
    digitalWrite(SIMPLE_GREEN, simpleLightState);
    Serial.print("BT_MSG: Light "); Serial.println(simpleLightState ? "ON" : "OFF");
    delay(200);
  }
  lastTouchState = currentTouch;
}

void checkBluetooth() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    if (cmd == 'T') sendEnvironmentData();
    else if (cmd == 'L') digitalWrite(SIMPLE_GREEN, HIGH);
    else if (cmd == 'l') digitalWrite(SIMPLE_GREEN, LOW);
    
    // Main Door: Still automatic (Open -> Wait -> Close)
    else if (cmd == 'M') { 
      mainDoor.write(90); 
      delay(3000); 
      mainDoor.write(0); 
    }
    
    
    else if (cmd == 'G') { 
      garageGate.write(90); 
      Serial.println("BT_MSG: Garage Gate OPEN");
    }
    else if (cmd == 'g') { 
      garageGate.write(0); 
      Serial.println("BT_MSG: Garage Gate CLOSED");
    }
  }
}

void sendEnvironmentData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.print("BT_MSG: TEMP: "); Serial.print(t);
  Serial.print("C | HUM: "); Serial.print(h); Serial.println("%");
}

void handleDanger() {
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(100);
  digitalWrite(BUZZER, LOW);
  delay(100);
}

void accessGranted() {
  Serial.println("BT_MSG: Access Granted.");
  digitalWrite(ACCESS_GREEN, HIGH);
  mainDoor.write(90);
  delay(3000);
  mainDoor.write(0);
  digitalWrite(ACCESS_GREEN, LOW);
}

void accessDenied() {
  Serial.println("BT_MSG: Access Denied!");
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(1000);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);
}

void blinkLED(int pin, int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(200);
    digitalWrite(pin, LOW);
    delay(200);
  }
}