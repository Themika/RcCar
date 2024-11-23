#include <SoftwareSerial.h>

// Set up Bluetooth communication
SoftwareSerial BTSerial(2, 3); // RX, TX (pins connected to Bluetooth)

// L298N Motor control pins
int motor1In1 = 5;   // Motor 1 Direction pin 1
int motor1In2 = 6;   // Motor 1 Direction pin 2
int motor2In3 = 9;   // Motor 2 Direction pin 1
int motor2In4 = 10;  // Motor 2 Direction pin 2

// LED pins
int forwardLedPin = 4; // LED for flashing during forward movement
int backwardLedPin = 7; // LED for flashing during backward movement

// Variables for joystick values
int joystickXValue = 0;
int joystickYValue = 0;

// Dead zone range
const int deadZone = 50;

void setup() {
  // Start Bluetooth and Serial Monitor
  BTSerial.begin(9600);
  Serial.begin(9600); // For monitoring via Serial Monitor

  // Set motor control pins as outputs
  pinMode(motor1In1, OUTPUT);
  pinMode(motor1In2, OUTPUT);
  pinMode(motor2In3, OUTPUT);
  pinMode(motor2In4, OUTPUT);

  // Set LED pins as outputs
  pinMode(forwardLedPin, OUTPUT);
  pinMode(backwardLedPin, OUTPUT);
}

void loop() {
  // Check if Bluetooth data is available
  if (BTSerial.available()) {
    // Read joystick data
    joystickXValue = BTSerial.parseInt();
    BTSerial.read(); // Consume comma
    joystickYValue = BTSerial.parseInt();
    BTSerial.read(); // Consume any extra data

    // Debugging via Serial Monitor
    Serial.print("Joystick X: ");
    Serial.print(joystickXValue);
    Serial.print(", Y: ");
    Serial.println(joystickYValue);

    // Dead zone handling
    if (abs(joystickXValue - 512) < deadZone) joystickXValue = 512;
    if (abs(joystickYValue - 512) < deadZone) joystickYValue = 512;

    // Stop motors and LEDs if joystick is in the center
    if (joystickXValue == 512 && joystickYValue == 512) {
      digitalWrite(motor1In1, LOW);
      digitalWrite(motor1In2, LOW);
      digitalWrite(motor2In3, LOW);
      digitalWrite(motor2In4, LOW);
      digitalWrite(forwardLedPin, LOW);   // Turn off forward LED
      digitalWrite(backwardLedPin, LOW); // Turn off backward LED
      return;
    }

    // Map joystick values to motor speed
    int motorSpeed = map(abs(joystickYValue - 512), 0, 511, 0, 255); // Speed based on Y-axis

    // Forward and backward movement based on Y-axis
    if (joystickYValue > 512) {
      // Forward movement
      analogWrite(motor1In1, motorSpeed);
      digitalWrite(motor1In2, LOW);
      analogWrite(motor2In3, motorSpeed);
      digitalWrite(motor2In4, LOW);

      // Flash the forward LED
      static unsigned long lastForwardToggleTime = 0;
      unsigned long currentTime = millis();
      if (currentTime - lastForwardToggleTime >= 250) { // Toggle every 250 ms
        lastForwardToggleTime = currentTime;
        digitalWrite(forwardLedPin, !digitalRead(forwardLedPin)); // Toggle forward LED state
      }

      digitalWrite(backwardLedPin, LOW); // Turn off backward LED
    } else if (joystickYValue < 512) {
      // Backward movement
      digitalWrite(motor1In1, LOW);
      analogWrite(motor1In2, motorSpeed);
      digitalWrite(motor2In3, LOW);
      analogWrite(motor2In4, motorSpeed);

      // Flash the backward LED
      static unsigned long lastBackwardToggleTime = 0;
      unsigned long currentTime = millis();
      if (currentTime - lastBackwardToggleTime >= 250) { // Toggle every 250 ms
        lastBackwardToggleTime = currentTime;
        digitalWrite(backwardLedPin, !digitalRead(backwardLedPin)); // Toggle backward LED state
      }

      digitalWrite(forwardLedPin, LOW); // Turn off forward LED
    }

    // Left and right turning based on X-axis
    int turnSpeed = map(abs(joystickXValue - 512), 0, 511, 0, 255); // Speed based on X-axis
    if (joystickXValue < 512) {
      // Turn right: Left motor forward, right motor backward
      analogWrite(motor1In1, turnSpeed);
      digitalWrite(motor1In2, LOW);
      digitalWrite(motor2In3, LOW);
      analogWrite(motor2In4, turnSpeed);
    } else if (joystickXValue > 512) {
      // Turn left: Left motor backward, right motor forward
      digitalWrite(motor1In1, LOW);
      analogWrite(motor1In2, turnSpeed);
      analogWrite(motor2In3, turnSpeed);
      digitalWrite(motor2In4, LOW);
    }
  }
}
