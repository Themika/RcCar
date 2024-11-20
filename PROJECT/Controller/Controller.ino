#include <SoftwareSerial.h>

SoftwareSerial Bluetooth(2, 3); // RX, TX for HC-05
int joystick1X = A0; // Analog pin for Joystick 1 X-axis
int joystick1Y = A1; // Analog pin for Joystick 1 Y-axis
int joystick2X = A2; // Analog pin for Joystick 2 X-axis
int joystick2Y = A3; // Analog pin for Joystick 2 Y-axis

void setup() {
  Bluetooth.begin(9600); // Bluetooth communication
  Serial.begin(38400);     // Debugging on Serial Monitor
  Serial.println("Master is ready to send data.");
}

void loop() {
  // Read joystick values
  int x1 = analogRead(joystick1X);
  int y1 = analogRead(joystick1Y);
  int x2 = analogRead(joystick2X);
  int y2 = analogRead(joystick2Y);

  // Format data as a string
  String data = String(x1) + "," + String(y1) + "," + String(x2) + "," + String(y2) + "\n";

  // Send data to slave
  Bluetooth.print(data);
  Serial.print("Sent: "); // Debug output
  Serial.println(data);

  delay(10); // Delay for stability
}
