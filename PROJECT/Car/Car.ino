#include <SoftwareSerial.h>

// Use pins 2 (RX) and 3 (TX) for Bluetooth communication
SoftwareSerial Bluetooth(2, 3); // RX, TX for HC-06
String inputString = "";

// Motor pins
const int motor1In1 = 5; // IN1 for motor 1
const int motor1In2 = 6; // IN2 for motor 1
const int motor1Enable = 9; // EN for motor 1

const int motor2In1 = 7; // IN1 for motor 2
const int motor2In2 = 8; // IN2 for motor 2
const int motor2Enable = 10; // EN for motor 2

void setup() {
  Bluetooth.begin(9600); // Bluetooth communication
  Serial.begin(38400);   // Debugging on Serial Monitor
  Serial.println("Slave is ready to receive data.");

  // Initialize motor pins
  pinMode(motor1In1, OUTPUT);
  pinMode(motor1In2, OUTPUT);
  pinMode(motor1Enable, OUTPUT);

  pinMode(motor2In1, OUTPUT);
  pinMode(motor2In2, OUTPUT);
  pinMode(motor2Enable, OUTPUT);

  // Stop motors initially
  stopMotor();
}

void loop() {
  // Check if data is available from Bluetooth
  while (Bluetooth.available()) {
    char c = Bluetooth.read(); // Read a character
    if (c == '\n') {
      // End of message; process the string
      processJoystickData(inputString);
      inputString = ""; // Clear the string
    } else {
      inputString += c;
    }
  }
}

void processJoystickData(String data) {
  // Split the string into components (x1, y1, x2, y2)
  int firstComma = data.indexOf(',');
  int secondComma = data.indexOf(',', firstComma + 1);
  int thirdComma = data.indexOf(',', secondComma + 1);

  if (firstComma > 0 && secondComma > firstComma && thirdComma > secondComma) {
    String x1Value = data.substring(0, firstComma);
    String y1Value = data.substring(firstComma + 1, secondComma);
    String x2Value = data.substring(secondComma + 1, thirdComma);
    String y2Value = data.substring(thirdComma + 1);

    // Convert strings to integers
    int x1 = x1Value.toInt(); // Joystick 1 X-axis (turning)
    int y1 = y1Value.toInt(); // Joystick 1 Y-axis (direction)
    int y2 = y2Value.toInt(); // Joystick 2 Y-axis (speed scaling)

    // // Print joystick values
    // Serial.print("Joystick 1 - X: ");
    // Serial.print(x1);
    // Serial.print(", Y: ");
    // Serial.println(y1);

    Serial.print("Joystick 2 - Speed: ");
    Serial.println(y2);
    delay(50);

    // Map joystick values
    int direction = map(y1, 0, 1023, -255, 255); // Forward/Backward direction
    int turning = map(x1, 0, 1023, -255, 255);   // Turning factor
    int throttle = map(y2, 0, 1023, 0, 255);     // Speed scaling factor (0-255)

    // Calculate motor speeds
    int baseSpeed = constrain(direction * throttle / 255, -255, 255);
    int motor1Speed = constrain(baseSpeed + turning, -255, 255); // Left motor
    int motor2Speed = constrain(baseSpeed - turning, -255, 255); // Right motor

    // // Print only Motor 1 speed for debugging
    // Serial.print("Motor 1 Speed: ");
    // Serial.println(motor1Speed);

    // Control motors
    controlMotor(motor1In1, motor1In2, motor1Enable, motor1Speed);
    controlMotor(motor2In1, motor2In2, motor2Enable, motor2Speed);
  } else {
    Serial.println("Invalid data received.");
  }
}

void controlMotor(int in1, int in2, int enable, int speed) {
  if (speed > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(enable, speed);
  } else if (speed < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(enable, -speed);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(enable, 0);
  }
}

void stopMotor() {
  digitalWrite(motor1In1, LOW);
  digitalWrite(motor1In2, LOW);
  analogWrite(motor1Enable, 0);

  digitalWrite(motor2In1, LOW);
  digitalWrite(motor2In2, LOW);
  analogWrite(motor2Enable, 0);
}
