# House-Automation-System
🔑 1. Security & Access Control (RFID & Servo)
Master Card Registration: Pressing a hardware button (BTN_REG) puts the system into a listening state to save the first scanned RFID card as the "Master Card".
Main Door Lock: Scanning the correct Master Card triggers a servo motor (mainDoor) to turn 90 degrees, pauses for 3 seconds to let someone enter, and automatically locks it back at 0 degrees.
Garage Control: The garage gate servo (garageGate) can be explicitly toggled open or closed via Bluetooth commands.
🚨 2. Safety & Threat Monitoring (Gas & Flame)
Constant Background Checks: The system continuously monitors an MQ6 Gas sensor and an analog Flame sensor.
Emergency Lockdown: If gas exceeds safe limits or fire is detected, it freezes all user inputs, rings a buzzing alarm, flashes a red LED, and alerts the user over Bluetooth. Normal operation only resumes when the danger clears.
📱 3. Smart Controls & Environment (Bluetooth & Touch)
Touch-Activated Light: A capacitive touch sensor works as a toggle switch to turn a dedicated green light source (SIMPLE_GREEN) on and off.
Bluetooth App Integration: The system listens for incoming single-character serial commands to trigger actions:
T: Reads and replies with current temperature and humidity data from the DHT11 sensor.
L / l: Manually forces the smart light ON or OFF.
M: Remotely opens the main door for 3 seconds.
G / g: Remotely opens or closes the garage gate.


🧠 Core Controller
1 x Arduino Uno R3 (or Arduino Nano/Mega) – The micro-controller running your code.
🛡️ Access & Motion Components
1 x RC522 RFID Module – For reading keycards and key fobs.
2 x SG90 Micro Servo Motors – One for the mainDoor (locks/unlocks), and one for the garageGate.
⚠️ Safety & Environment Sensors
1 x MQ-6 Gas Sensor Module – To detect LPG, butane, or smoke levels.
1 x Flame Sensor Module (Analog Output) – To detect fire infrastructure infrared signatures.
1 x DHT11 Temperature & Humidity Sensor – To pull environmental data readings.
🎛️ Control & Interface Modules
1 x HC-05 (or HC-06) Bluetooth Module – Connects to TX/RX lines to send and receive commands to/from your phone.
1 x TTP223 Touch Sensor Module – Acts as the capacitive light switch toggle.
1 x Push Button Switch – Connected to BTN_REG to trigger Master Card registration mode.
🔊 Outputs & Indicators
1 x Active 5V Buzzer – Sound alarm indicator for danger warnings and access denied tones.
3 x 5mm LEDs – Visual status lights:
1 x Red LED (System Alert / Danger / Access Denied)
2 x Green LEDs (One for Access Granted, one for the Touch-controlled Light)
🔌 Prototyping Essentials
1 x Full-Sized Solderless Breadboard – To map out your physical test circuit easily.
3 x 220-Ohm Resistors – Current-limiting protection for the LEDs.
Bundle of Jumper Wires – High count of Male-to-Male and Male-to-Female wires to establish pin connections.
1 x USB Cable (Type-A to Type-B) – To upload this codebase from VS Code to your Arduino board.