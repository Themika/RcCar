#include <SoftwareSerial.h>
#include <Pixy2.h>

SoftwareSerial Bluetooth(2, 3); // RX, TX for HC-06
Pixy2 pixy;

int mode = 1; 

String inputString = "";

const int motor1In1 = 5;
const int motor1In2 = 6;
const int motor1Enable = 9;

const int motor2In1 = 7;
const int motor2In2 = 8;
const int motor2Enable = 10;

const int centerThreshold = 20;
const int baseSpeed = 150;

void setup() {
  Serial.begin(31250);
  Bluetooth.begin(9600);
  pixy.init();

  // Initialize motor pins
  pinMode(motor1In1, OUTPUT);
  pinMode(motor1In2, OUTPUT);
  pinMode(motor1Enable, OUTPUT);

  pinMode(motor2In1, OUTPUT);
  pinMode(motor2In2, OUTPUT);
  pinMode(motor2Enable, OUTPUT);

  stopMotor();
  Serial.println("Robot initialized. Set mode to 0 (RC) or 1 (Object-Following).");
}

void loop() {
  if (mode == 0) {
    rcMode(); 
  } else if (mode == 1) {
    objectFollowingMode(); 
  }
}

void rcMode() {
  while (Bluetooth.available()) {
    char c = Bluetooth.read();
    if (c == '\n') {
      processJoystickData(inputString);
      inputString = ""; 
    } else {
      inputString += c;
    }
  }
}

void objectFollowingMode() {
  pixy.ccc.getBlocks();
  
  if (pixy.ccc.numBlocks > 0) {
    for (int i = 0; i < pixy.ccc.numBlocks; i++) {
      if (pixy.ccc.blocks[i].m_signature == 1 && 
          pixy.ccc.blocks[i].m_width > 20 &&  
          pixy.ccc.blocks[i].m_height > 20) { 
          
        int blockX = pixy.ccc.blocks[i].m_x;
        int blockWidth = pixy.ccc.blocks[i].m_width;
        int blockHeight = pixy.ccc.blocks[i].m_height;

        Serial.print("Detected block: X=");
        Serial.print(blockX);
        Serial.print(", Width=");
        Serial.print(blockWidth);
        Serial.print(", Height=");
        Serial.println(blockHeight);

        if (blockX < (pixy.frameWidth / 2 - centerThreshold)) {
          controlMotor(motor1In1, motor1In2, motor1Enable, -baseSpeed);
          controlMotor(motor2In1, motor2In2, motor2Enable, baseSpeed);
        } else if (blockX > (pixy.frameWidth / 2 + centerThreshold)) {
          controlMotor(motor1In1, motor1In2, motor1Enable, baseSpeed);
          controlMotor(motor2In1, motor2In2, motor2Enable, -baseSpeed);
        } else {
          controlMotor(motor1In1, motor1In2, motor1Enable, baseSpeed);
          controlMotor(motor2In1, motor2In2, motor2Enable, baseSpeed);
        }
        return;
      }
    }
  } else {
    stopMotor();
    Serial.println("No block detected.");
  }
}


void processJoystickData(String data) {
  int firstComma = data.indexOf(',');
  int secondComma = data.indexOf(',', firstComma + 1);
  int thirdComma = data.indexOf(',', secondComma + 1);

  if (firstComma > 0 && secondComma > firstComma && thirdComma > secondComma) {
    String x1Value = data.substring(0, firstComma);
    String y1Value = data.substring(firstComma + 1, secondComma);
    String y2Value = data.substring(secondComma + 1);

    int x1 = x1Value.toInt();
    int y1 = y1Value.toInt();
    int throttle = map(y2Value.toInt(), 0, 1023, 0, 255);

    int direction = map(y1, 0, 1023, -255, 255);
    int turning = map(x1, 0, 1023, -255, 255);
        // Print joystick values
    Serial.print("Joystick 1 - X: ");
    Serial.print(x1);
    Serial.print(", Y: ");
    Serial.println(y1);

    Serial.print("Joystick 2 - Speed: ");
    Serial.println(throttle);
    delay(50);

    int baseSpeed = constrain(direction * throttle / 255, -255, 255);
    int motor1Speed = constrain(baseSpeed + turning, -255, 255);
    int motor2Speed = constrain(baseSpeed - turning, -255, 255);

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
