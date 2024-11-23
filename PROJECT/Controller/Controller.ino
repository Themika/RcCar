#include <SoftwareSerial.h>

// Set up Bluetooth communication
SoftwareSerial BTSerial(2, 3);  // RX, TX (pins connected to Bluetooth)

// Joystick pins
int joystick1XPin = A0; // X-axis of joystick 1
int joystick1YPin = A1; // Y-axis of joystick 1

// Speaker pin
int speakerPin = 5; // Pin connected to the speaker/buzzer

void setup() {
  // Start Bluetooth and Serial Monitor
  BTSerial.begin(9600);
  Serial.begin(9600);  // For monitoring via Serial Monitor

  // Set the speaker pin as output
  pinMode(speakerPin, OUTPUT);
}

void loop() {
  // Read joystick X and Y values (0-1023)
  int joystick1XValue = analogRead(joystick1XPin);
  int joystick1YValue = analogRead(joystick1YPin);

  // Send joystick data via Bluetooth
  BTSerial.print(joystick1XValue);
  BTSerial.print(",");
  BTSerial.print(joystick1YValue);
  BTSerial.print(",");

  // Debugging via Serial Monitor
  Serial.print("Joystick X: ");
  Serial.print(joystick1XValue);
  Serial.print(", Y: ");
  Serial.println(joystick1YValue);

  // Reverse detection: Play sound if joystick Y-axis is below the center
  if (joystick1YValue < 512) {
    tone(speakerPin, 1000); // Play a 1000 Hz tone
  } 
  else {
    noTone(speakerPin); // Stop the tone
  }

  delay(200);  // Wait before sending again
}
