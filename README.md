# 🕶️ Obstacle Detection Glasses for Visually Impaired

An embedded systems project that uses **three HC-SR04 ultrasonic sensors** mounted on glasses to help visually impaired people detect obstacles in real time — with **directional audio feedback** via dual passive buzzers.

---

## 📸 Project Photo

![Obstacle Detection Glasses](photo.jpg)

---

## 🎯 Problem Statement

Visually impaired individuals face constant risk from obstacles in their environment. Existing solutions like white canes only detect ground-level obstacles. This device provides **360° front-facing obstacle awareness** at head/body level, with directional alerts to guide the user.

---

## ⚙️ How It Works

- Three ultrasonic sensors (Left, Front, Right) continuously measure distances
- The system detects the **closest obstacle** and determines its direction
- Buzzers alert the user with **increasing urgency** as obstacles get closer:
  - 🟢 30cm → Slow, low-tone beep
  - 🟡 20cm → Medium beep
  - 🔴 10cm → Rapid, high-tone urgent beep
- **Left buzzer** activates for left/front obstacles
- **Right buzzer** activates for right/front obstacles
- **Both buzzers** activate when obstacle is directly in front

---

## 🔧 Components Used

| Component | Quantity |
|-----------|----------|
| Arduino Uno | 1 |
| HC-SR04 Ultrasonic Sensor | 3 |
| Passive Buzzer | 2 |
| Breadboard | 1 |
| Jumper Wires | Several |
| Sunglasses frame | 1 |

---

## 🔌 Circuit Connections

| Component | Arduino Pin |
|-----------|-------------|
| Left Sensor - Trig | D2 |
| Left Sensor - Echo | D3 |
| Front Sensor - Trig | D4 |
| Front Sensor - Echo | D5 |
| Right Sensor - Trig | D6 |
| Right Sensor - Echo | D7 |
| Left Buzzer | D10 |
| Right Buzzer | D12 |
| Sensor 2 VCC | A0 (as 5V) |
| Sensor 3 VCC | A1 (as 5V) |

---

## 💡 Key Features

- **Noise reduction** via 5-reading averaging (smoothed distance)
- **Simultaneous dual buzzer** tone generation using custom square wave (avoids Arduino's single-pin tone() limitation)
- **Directional awareness** — user knows which side the obstacle is on
- **3 urgency levels** — tone frequency and beep speed scale with proximity
- **Serial monitor debugging** for real-time distance readout

---

## 🚀 How to Run

1. Clone this repository
2. Open `obstacle_detection_glasses.ino` in Arduino IDE
3. Connect components as per the circuit table above
4. Upload to Arduino Uno
5. Open Serial Monitor (9600 baud) to view live distance readings

---

## 🌍 Real World Application

This project simulates the core obstacle-avoidance logic used in:
- Assistive technology for the visually impaired
- Autonomous rover navigation (similar to ISRO's Chandrayaan rover)
- Collision avoidance systems in robotics

---

## 👨‍💻 Author

**[Your Name]**
B.Tech CSE, [Your College], Telangana
[Your Email] | [LinkedIn URL]

---

## 📄 License

MIT License — free to use, modify, and distribute with attribution.
