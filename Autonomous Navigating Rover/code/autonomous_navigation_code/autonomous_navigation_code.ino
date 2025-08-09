#include <Wire.h>
#include <SoftwareSerial.h>
#include <QMC5883LCompass.h>

#define trigObstacle 2
#define echoObstacle 3
#define trigPothole 4
#define echoPothole 5

#define in1 6
#define in2 7
#define in3 8
#define in4 9
#define enA 10
#define enB 11

SoftwareSerial BT(12, 13); // RX, TX
QMC5883LCompass compass;

int heading;

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  pinMode(trigObstacle, OUTPUT);
  pinMode(echoObstacle, INPUT);
  pinMode(trigPothole, OUTPUT);
  pinMode(echoPothole, INPUT);

  pinMode(in1, OUTPUT); pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT); pinMode(in4, OUTPUT);
  pinMode(enA, OUTPUT); pinMode(enB, OUTPUT);

  compass.init();
}

void loop() {
  compass.read();
  heading = compass.getAzimuth();

  if (BT.available()) {
    String command = BT.readStringUntil('\n');
    command.trim();
    if (command == "GO") {
      moveForward();
    } else if (command == "STOP") {
      stopMotors();
    }
  }

  if (detectObstacle() < 25) {
    stopMotors();
    delay(500);
    avoidObstacle();
  }

  if (detectPothole() > 50) {
    stopMotors();
    delay(500);
    avoidPothole();
  }
}

// === Movement Functions ===

void moveForward() {
  analogWrite(enA, 150);
  analogWrite(enB, 150);
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
}

void stopMotors() {
  digitalWrite(in1, LOW); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, LOW);
}

void avoidObstacle() {
  // Turn right slightly
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  delay(400);
  stopMotors();
}

void avoidPothole() {
  // Reverse a bit and turn left
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  delay(300);
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  delay(400);
  stopMotors();
}

// === Sensor Functions ===

long detectObstacle() {
  digitalWrite(trigObstacle, LOW);
  delayMicroseconds(2);
  digitalWrite(trigObstacle, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigObstacle, LOW);
  return pulseIn(echoObstacle, HIGH) * 0.034 / 2;
}

long detectPothole() {
  digitalWrite(trigPothole, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPothole, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPothole, LOW);
  return pulseIn(echoPothole, HIGH) * 0.034 / 2;
}
