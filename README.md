# 🤖 Arduino Robot Car — Pin Reference

## 📌 Complete Pin Map

| Component | Pin | Notes |
|-----------|-----|-------|
| **IR Sensor Left** | A4 | Digital Read |
| **IR Sensor Right** | A5 | Digital Read |
| **Ultrasonic TRIG** | D7 | Output |
| **Ultrasonic ECHO** | D6 | Input |
| **Servo Motor** | D9 | PWM |
| **Buzzer** | D8 | Output |
| **LDR (Light Sensor)** | A0 | Analog Read |
| **LED** | A1 | Output |
| **Motor Driver IN1** | D2 | Output |
| **Motor Driver IN2** | D3 | Output |
| **Motor Driver IN3** | D12 | Output |
| **Motor Driver IN4** | D13 | Output |
| **Motor Driver ENA** | D10 | PWM |
| **Motor Driver ENB** | D11 | PWM |

---

## 🔹 IR Sensor (Line Sensor)

| IR Sensor Pin | Arduino |
|--------------|---------|
| VCC | 5V |
| GND | GND |
| OUT (Left) | A4 |
| OUT (Right) | A5 |

---

## 🔹 Ultrasonic Sensor (HC-SR04)

| Ultrasonic Pin | Arduino |
|---------------|---------|
| VCC | 5V |
| GND | GND |
| TRIG | D7 |
| ECHO | D6 |

---

## 🔹 Servo Motor

| Servo Wire | Arduino |
|-----------|---------|
| Red | 5V |
| Brown | GND |
| Orange (Signal) | D9 |

---

## 🔹 Buzzer

| Buzzer Pin | Arduino |
|-----------|---------|
| + | D8 |
| - | GND |

---

## 🔹 LDR (Light Sensor)

> ⚠️ Requires **10kΩ resistor** (voltage divider)

```
5V ── LDR ──┬── A0 (Arduino)
            │
          10kΩ
            │
           GND
```

---

## 🔹 LED

> ⚠️ Requires **220Ω resistor**

| LED Pin | Arduino |
|--------|---------|
| + (long leg) | A1 |
| - (short leg) | GND (via 220Ω) |

---

## 🔹 Motor Driver (L298N)

| L298N Pin | Arduino |
|----------|---------|
| IN1 | D2 |
| IN2 | D3 |
| IN3 | D12 |
| IN4 | D13 |
| ENA | D10 (PWM) |
| ENB | D11 (PWM) |

---

## 🔹 Power Supply

| Component | Power Source |
|-----------|-------------|
| Arduino | USB / 5V |
| L298N | 12V Battery |
| Sensors & LED | 5V (from Arduino) |

---

## 📦 Components List

- Arduino Uno
- L298N Motor Driver
- 2x IR Sensor (Line Sensor)
- HC-SR04 Ultrasonic Sensor
- Servo Motor (SG90)
- LDR (Light Dependent Resistor)
- LED
- Buzzer
- 10kΩ Resistor (for LDR)
- 220Ω Resistor (for LED)
