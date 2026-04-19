#include <Servo.h>

Servo myServo;

// Motor Pins
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
#define ENA 10
#define ENB 11

// Ultrasonic
#define trigPin 7
#define echoPin 6

// Sensors
#define IRSensorLeft A4
#define IRSensorRight A5
#define LDR A0

// Output
#define buzzer 8
#define led 13

long duration;
int distance;

int speedNormal = 180;
int speedSlow = 120;

int servoPos = 0;
bool dir = true;

// Distance Function
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);
  if(duration == 0) return 999;

  return duration * 0.034 / 2;
}

// Motor Functions
void moveForward(int sp){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, sp);
  analogWrite(ENB, sp);
}

void turnLeft(int sp){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, sp);
  analogWrite(ENB, sp);
}

void turnRight(int sp){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, sp);
  analogWrite(ENB, sp);
}

void stopCar(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Setup
void setup() {
  myServo.attach(9);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(IRSensorLeft, INPUT);
  pinMode(IRSensorRight, INPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);

  Serial.begin(9600);
}

// Main Loop
void loop() {

  // ===== Servo Sweep (0–90° ONLY) =====
  if(dir){
    servoPos++;
    if(servoPos >= 90) dir = false;
  } else {
    servoPos--;
    if(servoPos <= 0) dir = true;
  }
  myServo.write(servoPos);

  // Sensor Read
  int d = getDistance();
  int leftVal = digitalRead(IRSensorLeft);
  int rightVal = digitalRead(IRSensorRight);
  int lightVal = analogRead(LDR);

  // ===== LDR → LED ONLY =====
  if(lightVal < 300){
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }

  // ===== Obstacle =====
  if(d < 15){
    stopCar();
    digitalWrite(buzzer, HIGH);
  }
  else if(d < 25){
    digitalWrite(buzzer, HIGH);

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
    digitalWrite(buzzer, LOW);

    // ===== Line Follow =====
    if(leftVal == LOW && rightVal == HIGH){
      turnLeft(speedNormal);
    }
    else if(leftVal == HIGH && rightVal == LOW){
      turnRight(speedNormal);
    }
    else if(leftVal == HIGH && rightVal == HIGH){
      moveForward(speedNormal);
    }
    else {
      moveForward(speedSlow);
    }
  }

  delay(15);
}
