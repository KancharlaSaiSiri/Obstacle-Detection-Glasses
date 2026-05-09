/*
 * ============================================================
 *   OBSTACLE DETECTION GLASSES FOR VISUALLY IMPAIRED
 * ============================================================
 * Author      : [Your Name]
 * College     : [Your College Name]
 * Date        : May 2026
 * Description : A wearable assistive device that uses three
 *               HC-SR04 ultrasonic sensors (Left, Front, Right)
 *               mounted on glasses to detect obstacles and alert
 *               the user via directional buzzer feedback.
 *
 * How it works:
 *   - Sensors continuously measure distance on all three sides
 *   - If an obstacle is detected within threshold range,
 *     buzzers beep with increasing urgency (tone + speed)
 *   - Left buzzer alerts for left/front obstacles
 *   - Right buzzer alerts for right/front obstacles
 *   - Both buzzers alert when obstacle is directly in front
 * ============================================================
 */

// ============================================================
// 1. PIN DEFINITIONS
// ============================================================

// Ultrasonic Sensor Pins (Trig & Echo for Left, Front, Right)
const int trigPinL = 2;
const int echoPinL = 3;
const int trigPinF = 4;
const int echoPinF = 5;
const int trigPinR = 6;
const int echoPinR = 7;

// Passive Buzzer Pins (Left and Right side of glasses)
const int buzzerLeft  = 10;
const int buzzerRight = 12;

// Analog pins used as 5V VCC source to power extra sensors
// (Arduino analog pins can supply ~40mA when set as OUTPUT HIGH)
const int VCC_S2_PIN = A0;
const int VCC_S3_PIN = A1;

// ============================================================
// 2. DISTANCE THRESHOLDS (in centimeters)
// ============================================================
const int THRESHOLD_SLOW   = 30;  // Obstacle within 30cm → slow alert
const int THRESHOLD_MEDIUM = 20;  // Obstacle within 20cm → medium alert
const int THRESHOLD_FAST   = 10;  // Obstacle within 10cm → urgent alert

// ============================================================
// 3. BUZZER TONE FREQUENCIES (in Hz)
// ============================================================
const int TONE_HIGH = 1200;  // Urgent beep (closest obstacle)
const int TONE_MID  = 800;   // Medium beep
const int TONE_LOW  = 400;   // Gentle beep (farthest threshold)

// ============================================================
// 4. BEEP PACE (ON/OFF duration in milliseconds)
// ============================================================
const int PACE_FAST   = 50;   // Rapid beeping for close obstacle
const int PACE_MEDIUM = 100;  // Moderate beeping
const int PACE_SLOW   = 250;  // Slow beeping for distant obstacle

// ============================================================
// 5. GLOBAL VARIABLES
// ============================================================
long duration;                    // Raw pulse duration from sensor
int distanceL, distanceF, distanceR;  // Distances from each sensor (cm)
const int NUM_READINGS = 5;       // Number of readings to average (smoothing)


// ============================================================
// FUNCTION: calculateSmoothedDistance
// ============================================================
// Takes multiple readings from a sensor and returns the average
// distance to reduce noise/false readings.
// Returns 500 (out of range) if reading is invalid.
// ============================================================
int calculateSmoothedDistance(int trigPin, int echoPin) {
  long sumDuration = 0;

  for (int i = 0; i < NUM_READINGS; i++) {
    // Trigger pulse: LOW → HIGH (10µs) → LOW
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Measure echo pulse duration (timeout at 30ms = ~5m range)
    duration = pulseIn(echoPin, HIGH, 30000);
    sumDuration += duration;
    delay(5);  // Small delay between readings
  }

  // Convert average duration to distance in cm
  // Formula: distance = (duration × speed of sound) / 2
  // Speed of sound ≈ 0.034 cm/µs
  long avgDuration = sumDuration / NUM_READINGS;
  int dist = avgDuration * 0.034 / 2;

  // Validate: return 500 (safe "no obstacle" value) if out of range
  return (dist > 0 && dist < 400) ? dist : 500;
}


// ============================================================
// FUNCTION: playPulse
// ============================================================
// Manually generates a square wave on buzzer pins to produce
// a tone at the given frequency. Supports simultaneous dual
// buzzer activation (standard tone() only works on one pin).
//
// Parameters:
//   onMillis  - Duration buzzer stays ON (ms)
//   offMillis - Duration buzzer stays OFF after beep (ms)
//   freq      - Tone frequency in Hz
//   leftOn    - Whether left buzzer should sound
//   rightOn   - Whether right buzzer should sound
// ============================================================
void playPulse(unsigned long onMillis, unsigned long offMillis, int freq, bool leftOn, bool rightOn) {
  
  // If no frequency or no buzzer selected, just wait silently
  if (freq <= 0 || (!leftOn && !rightOn)) {
    digitalWrite(buzzerLeft, LOW);
    digitalWrite(buzzerRight, LOW);
    delay(onMillis + offMillis);
    return;
  }

  unsigned long endTime = millis() + onMillis;
  unsigned long halfPeriodMicros = (1000000UL / freq) / 2UL;  // Half period in µs
  unsigned long lastToggle = micros();
  bool state = false;

  // ON phase: toggle pins rapidly to generate square wave at 'freq'
  while (millis() < endTime) {
    unsigned long now = micros();
    if (now - lastToggle >= halfPeriodMicros) {
      lastToggle = now;
      state = !state;

      // Activate only the relevant buzzer(s)
      digitalWrite(buzzerLeft,  leftOn  ? (state ? HIGH : LOW) : LOW);
      digitalWrite(buzzerRight, rightOn ? (state ? HIGH : LOW) : LOW);
    }
  }

  // OFF phase: silence both buzzers
  digitalWrite(buzzerLeft, LOW);
  digitalWrite(buzzerRight, LOW);
  delay(offMillis);
}


// ============================================================
// SETUP
// ============================================================
void setup() {
  // Use analog pins as 5V power source for extra sensors
  pinMode(VCC_S2_PIN, OUTPUT);
  digitalWrite(VCC_S2_PIN, HIGH);
  pinMode(VCC_S3_PIN, OUTPUT);
  digitalWrite(VCC_S3_PIN, HIGH);

  // Configure sensor pins
  pinMode(trigPinL, OUTPUT); pinMode(echoPinL, INPUT);
  pinMode(trigPinF, OUTPUT); pinMode(echoPinF, INPUT);
  pinMode(trigPinR, OUTPUT); pinMode(echoPinR, INPUT);

  // Configure buzzer pins
  pinMode(buzzerLeft, OUTPUT);
  pinMode(buzzerRight, OUTPUT);
  digitalWrite(buzzerLeft, LOW);
  digitalWrite(buzzerRight, LOW);

  // Start serial monitor for debugging
  Serial.begin(9600);
  Serial.println("=== Obstacle Detection Glasses: System Ready ===");
}


// ============================================================
// MAIN LOOP
// ============================================================
void loop() {

  // Step 1: Read smoothed distances from all three sensors
  distanceL = calculateSmoothedDistance(trigPinL, echoPinL);
  distanceF = calculateSmoothedDistance(trigPinF, echoPinF);
  distanceR = calculateSmoothedDistance(trigPinR, echoPinR);

  // Step 2: Find the closest obstacle across all directions
  int minDistance = min(distanceL, min(distanceF, distanceR));

  // Step 3: Determine alert level based on closest distance
  int currentTone = 0;
  int currentPace = 0;

  if (minDistance <= THRESHOLD_FAST) {
    currentTone = TONE_HIGH;    // High urgency
    currentPace = PACE_FAST;
  } else if (minDistance <= THRESHOLD_MEDIUM) {
    currentTone = TONE_MID;     // Medium urgency
    currentPace = PACE_MEDIUM;
  } else if (minDistance <= THRESHOLD_SLOW) {
    currentTone = TONE_LOW;     // Low urgency
    currentPace = PACE_SLOW;
  }

  // Step 4: Determine which buzzer(s) to activate (directional alert)
  bool alertLeft  = false;
  bool alertRight = false;

  if (currentTone > 0) {
    // Alert left side if left or front sensor triggered
    alertLeft  = (distanceL <= minDistance + 1) || (distanceF <= minDistance + 1);
    // Alert right side if right or front sensor triggered
    alertRight = (distanceR <= minDistance + 1) || (distanceF <= minDistance + 1);

    // Special case: obstacle only in front → alert BOTH sides
    if (distanceF <= THRESHOLD_SLOW && distanceL > THRESHOLD_SLOW && distanceR > THRESHOLD_SLOW) {
      alertLeft  = true;
      alertRight = true;
    }

    // Step 5: Play the directional alert
    playPulse(currentPace, currentPace, currentTone, alertLeft, alertRight);

  } else {
    // No obstacle detected — keep buzzers silent
    digitalWrite(buzzerLeft, LOW);
    digitalWrite(buzzerRight, LOW);
    delay(50);
  }

  // Step 6: Print sensor data to Serial Monitor for debugging
  Serial.print("L: "); Serial.print(distanceL);
  Serial.print(" cm | F: "); Serial.print(distanceF);
  Serial.print(" cm | R: "); Serial.print(distanceR);
  Serial.print(" cm | Min: "); Serial.print(minDistance);
  Serial.println(" cm");
}
