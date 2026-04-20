#include <Servo.h>

Servo myServo;

// ===== Motor Pins (তোমার confirmed পিন) =====
#define IN1 2
#define IN2 3
#define IN3 12
#define IN4 13
#define ENA 10
#define ENB 11

// ===== Ultrasonic =====
#define trigPin 7
#define echoPin 6

// ===== Sensors =====
#define IRLeft  A4
#define IRRight A5
#define LDR_PIN A0

// ===== Output =====
#define BUZZER 8
#define LED_PIN A1

// ===== Servo Center =====
#define SERVO_CENTER 90
#define SERVO_RIGHT  140
#define SERVO_LEFT   40

// ===== Speed =====
#define SPEED_FAST   200
#define SPEED_NORMAL 160
#define SPEED_SLOW   110
#define SPEED_TURN   140

// ===== Obstacle Threshold =====
#define DIST_STOP    12   // cm — সম্পূর্ণ থামো
#define DIST_WARN    25   // cm — সতর্কতা
#define DIST_CLEAR   30   // cm — পথ পরিষ্কার

// ===== State Machine =====
enum CarState {
  LINE_FOLLOW,
  OBSTACLE_WAIT,
  OBSTACLE_BYPASS,
  RETURNING_LINE
};

CarState currentState = LINE_FOLLOW;

unsigned long waitStartTime = 0;
#define WAIT_DURATION 2500  // 2.5 সেকেন্ড অপেক্ষা

// ===========================
// Ultrasonic — Median of 3
// ===========================
int getDistance() {
  int r[3];
  for (int i = 0; i < 3; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(4);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long dur = pulseIn(echoPin, HIGH, 25000);
    r[i] = (dur == 0) ? 999 : (int)(dur * 0.034 / 2);
    delay(5);
  }
  // Bubble sort → median
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2 - i; j++)
      if (r[j] > r[j + 1]) { int t = r[j]; r[j] = r[j+1]; r[j+1] = t; }
  return r[1];
}

// Servo দিয়ে বাম/ডান দূরত্ব মাপো
int lookAndMeasure(int angle) {
  myServo.write(angle);
  delay(400);
  int d = getDistance();
  delay(100);
  return d;
}

// ===========================
// Motor Functions
// Motor B (Right) REVERSED
// ===========================
void moveForward(int sp) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);   // Motor A
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);  // Motor B (reversed)
  analogWrite(ENA, sp);
  analogWrite(ENB, sp);
}

void moveBackward(int sp) {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, sp);
  analogWrite(ENB, sp);
}

void turnLeft(int sp) {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, sp);
  analogWrite(ENB, sp);
}

void turnRight(int sp) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, sp);
  analogWrite(ENB, sp);
}

void stopCar() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// ===========================
// Setup
// ===========================
void setup() {
  myServo.attach(9);
  myServo.write(SERVO_CENTER);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(IRLeft,  INPUT);
  pinMode(IRRight, INPUT);
  pinMode(BUZZER,  OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(BUZZER, LOW);
  digitalWrite(LED_PIN, LOW);

  // Startup beep
  digitalWrite(BUZZER, HIGH); delay(100);
  digitalWrite(BUZZER, LOW);

  Serial.begin(9600);
  Serial.println("=== Smart Line Car Ready ===");
  delay(1000);
}

// ===========================
// Bypass Obstacle Function
// লাইন ছেড়ে পাশ কাটিয়ে ফিরবে
// ===========================
void bypassObstacle() {
  Serial.println(">> Bypass Start");

  // সার্ভো দিয়ে বাম-ডান দেখো
  int distR = lookAndMeasure(SERVO_RIGHT);
  int distL = lookAndMeasure(SERVO_LEFT);
  myServo.write(SERVO_CENTER);
  delay(200);

  Serial.print("L="); Serial.print(distL);
  Serial.print(" R="); Serial.println(distR);

  // বেশি জায়গা যেদিকে সেদিকে যাও
  if (distL >= distR) {
    // বাম দিক দিয়ে bypass
    turnLeft(SPEED_TURN);  delay(450);   // লাইন থেকে বের হও
    moveForward(SPEED_NORMAL); delay(600); // সামনে যাও
    turnRight(SPEED_TURN); delay(450);   // অবজেক্টের পাশ দিয়ে
    moveForward(SPEED_NORMAL); delay(700); // অবজেক্ট পার করো
    turnRight(SPEED_TURN); delay(400);   // লাইনের দিকে ফেরো
    moveForward(SPEED_NORMAL); delay(500);
    turnLeft(SPEED_TURN);  delay(400);   // লাইনে ফিরে আসো
  } else {
    // ডান দিক দিয়ে bypass
    turnRight(SPEED_TURN); delay(450);
    moveForward(SPEED_NORMAL); delay(600);
    turnLeft(SPEED_TURN);  delay(450);
    moveForward(SPEED_NORMAL); delay(700);
    turnLeft(SPEED_TURN);  delay(400);
    moveForward(SPEED_NORMAL); delay(500);
    turnRight(SPEED_TURN); delay(400);
  }

  stopCar();
  Serial.println(">> Bypass Done");
  currentState = LINE_FOLLOW;
}

// ===========================
// Main Loop
// ===========================
void loop() {

  int d        = getDistance();
  int leftIR   = digitalRead(IRLeft);   // কালো লাইন = 1, সাদা = 0
  int rightIR  = digitalRead(IRRight);
  int lightVal = analogRead(LDR_PIN);

  // ===== LDR → LED =====
  digitalWrite(LED_PIN, lightVal < 300 ? HIGH : LOW);

  // ===== Debug =====
  Serial.print("State:"); Serial.print(currentState);
  Serial.print(" D:");    Serial.print(d);
  Serial.print(" L:");    Serial.print(leftIR);
  Serial.print(" R:");    Serial.print(rightIR);
  Serial.print(" LDR:");  Serial.println(lightVal);

  // ===========================
  // STATE MACHINE
  // ===========================
  switch (currentState) {

    // ---------------------------
    case LINE_FOLLOW:
    // ---------------------------
      digitalWrite(BUZZER, LOW);

      if (d <= DIST_STOP) {
        // অবজেক্ট কাছে — থামো এবং wait শুরু করো
        stopCar();
        digitalWrite(BUZZER, HIGH);
        waitStartTime = millis();
        currentState = OBSTACLE_WAIT;
        Serial.println(">> Obstacle! Waiting...");
        break;
      }

      // কালো লাইন ফলো করো (1 = কালো লাইন)
      if      (leftIR == 1 && rightIR == 1) moveForward(SPEED_FAST);   // দুটোই লাইনে
      else if (leftIR == 1 && rightIR == 0) turnLeft(SPEED_TURN);      // বামে লাইন
      else if (leftIR == 0 && rightIR == 1) turnRight(SPEED_TURN);     // ডানে লাইন
      else                                  moveForward(SPEED_SLOW);   // লাইনের বাইরে
      break;

    // ---------------------------
    case OBSTACLE_WAIT:
    // ---------------------------
      stopCar();
      // বাজার দিতে থাকো
      if ((millis() / 300) % 2 == 0)
        digitalWrite(BUZZER, HIGH);
      else
        digitalWrite(BUZZER, LOW);

      // 2.5 সেকেন্ড পরে দেখো অবজেক্ট সরেছে কিনা
      if (millis() - waitStartTime >= WAIT_DURATION) {
        int newDist = getDistance();
        if (newDist > DIST_CLEAR) {
          // অবজেক্ট সরে গেছে — আবার চলো
          digitalWrite(BUZZER, LOW);
          currentState = LINE_FOLLOW;
          Serial.println(">> Obstacle cleared!");
        } else {
          // অবজেক্ট এখনো আছে — bypass করো
          digitalWrite(BUZZER, LOW);
          currentState = OBSTACLE_BYPASS;
        }
      }
      break;

    // ---------------------------
    case OBSTACLE_BYPASS:
    // ---------------------------
      bypassObstacle();
      break;
  }

  delay(10);
}
