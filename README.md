

## 🔹 IR Sensor (Line Sensor)

| Pin | Arduino                |
| --- | ---------------------- |
| VCC | 5V                     |
| GND | GND                    |
| OUT | A4 (Left) / A5 (Right) |

👉 ✔️ সবসময় 5V + GND লাগাতে হবে

---

## 🔹 Ultrasonic Sensor

| Pin  | Arduino |
| ---- | ------- |
| VCC  | 5V      |
| GND  | GND     |
| TRIG | D7      |
| ECHO | D6      |

---

## 🔹 Servo Motor

| Wire   | Arduino |
| ------ | ------- |
| Red    | 5V      |
| Brown  | GND     |
| Orange | D9      |

---

## 🔹 Buzzer

| Pin | Arduino |
| --- | ------- |
| +   | D8      |
| -   | GND     |

---

## 🔹 LDR (Light Sensor)

👉 LDR একটু আলাদা (এটা voltage divider লাগে)

### Connection:

```text
5V ---- LDR ---- A0 ---- Resistor (10k) ---- GND
```

👉 A0 থেকে reading নেওয়া হবে

---

## 🔹 LED (Light)

👉 এটা আলাদা component (LDR-এ built-in light থাকে না ❌)

| LED          | Arduino                 |
| ------------ | ----------------------- |
| + (long leg) | D13                     |
| -            | GND (via resistor 220Ω) |
---
🔹 Motor Driver (L298N)
Function
Arduino Pin
IN1 D2
IN2 D3
IN3 D4
IN4 D5
ENA D10 (PWM) 
ENB D11 (PWM)
---
