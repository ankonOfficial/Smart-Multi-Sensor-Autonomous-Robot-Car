#include <Servo.h>

Servo myServo;

// ===== Motor Pins =====
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
#define IRSensorLeft  A4
#define IRSensorRight A5
#define LDR A0

// ===== Output =====
#define buzzer 8
#define led A1

// ===== Speed Settings =====
int speedNormal = 200;
int speedSlow   = 120;
int speedTurn   = 150;

// ===== Servo =====
int servoPos = 45;
bool dir = true;

// ===========================
// Distance — Multiple Reading
// ===========================
int getDistance() {
  int readings[3];

  for(int i = 0; i < 3; i++){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(4);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long dur = pulseIn(echoPin, HIGH, 25000);
    if(dur == 0) readings[i] = 999;
    else readings[i] = dur * 0.034 / 2;

    delay(5);
  }

  // মাঝের মানটা নাও (sort করে median)
  // Simple bubble sort for 3 values
  for(int i = 0; i < 2; i++)
    for(int j = 0; j < 2-i; j++)
      if(readings[j] > readings[j+1]){
        int tmp = readings[j];
        readings[j] = readings[j+1];
        readings[j+1] = tmp;
      }

  return readings[1]; // median value
}

// ===========================
// Motor Functions
// ===========================
void moveForward(int sp){
  // Motor A (Left) — Normal
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  // Motor B (Right) — REVERSED fix
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, sp);
  analogWrite(ENB, sp);
}

void turnLeft(int sp){
  // Left motor পিছনে, Right motor সামনে
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, sp);
  analogWrite(ENB, sp);
}

void turnRight(int sp){
  // Left motor সামনে, Right motor পিছনে
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, sp);
  analogWrite(ENB, sp);
}

void stopCar(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// ===========================
// Setup
// ===========================
void setup() {
  myServo.attach(9);
  myServo.write(45); // মাঝামাঝি position থেকে শুরু

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(IRSensorLeft,  INPUT);
  pinMode(IRSensorRight, INPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);

  digitalWrite(buzzer, LOW);
  digitalWrite(led, LOW);

  Serial.begin(9600);
  Serial.println("System Ready...");
  delay(1000); // শুরুতে ১ সেকেন্ড অপেক্ষা
}

// ===========================
// Main Loop
// ===========================
void loop() {

  // ===== Servo Sweep 0–90° =====
  if(dir){
    servoPos++;
    if(servoPos >= 90) dir = false;
  } else {
    servoPos--;
    if(servoPos <= 0) dir = true;
  }
  myServo.write(servoPos);

  // ===== Sensor Read =====
  int d        = getDistance();
  int leftVal  = digitalRead(IRSensorLeft);   // BLACK line = LOW
  int rightVal = digitalRead(IRSensorRight);  // BLACK line = LOW
  int lightVal = analogRead(LDR);

  // ===== LDR → LED =====
  // অন্ধকার হলে LED জ্বলবে
  if(lightVal < 300){
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }

  // ===== Serial Monitor Debug =====
  Serial.print("Dist: "); Serial.print(d);
  Serial.print(" | L: ");  Serial.print(leftVal);
  Serial.print(" | R: ");  Serial.print(rightVal);
  Serial.print(" | LDR: ");Serial.println(lightVal);

  // ===========================
  // OBSTACLE DETECTION
  // ===========================
  if(d < 12){
    // খুব কাছে — সম্পূর্ণ থামো
    stopCar();
    digitalWrite(buzzer, HIGH);
    delay(300);
    digitalWrite(buzzer, LOW);
    delay(200);
    return; // বাকি কোড skip
  }
  else if(d < 25){
    // কাছে — slow down + buzzer
    digitalWrite(buzzer, HIGH);

    // IR দিয়ে মোড় নাও
    if(leftVal == LOW && rightVal == HIGH){
      turnLeft(speedSlow);
    }
    else if(leftVal == HIGH && rightVal == LOW){
      turnRight(speedSlow);
    }
    else{
      moveForward(speedSlow);
    }
  }
  else {
    // ===========================
    // LINE FOLLOWING
    // সাদা background, কালো line
    // IR sensor কালো লাইনে LOW দেয়
    // ===========================
    digitalWrite(buzzer, LOW);

    if(leftVal == LOW && rightVal == LOW){
      // দুটোই লাইনে — সোজা যাও (fast)
      moveForward(speedNormal);
    }
    else if(leftVal == LOW && rightVal == HIGH){
      // শুধু বাম লাইনে — বামে ঘোরো
      turnLeft(speedTurn);
    }
    else if(leftVal == HIGH && rightVal == LOW){
      // শুধু ডানে লাইনে — ডানে ঘোরো
      turnRight(speedTurn);
    }
    else {
      // দুটোই লাইনের বাইরে — slow forward
      moveForward(speedSlow);
    }
  }

  delay(10);
}
