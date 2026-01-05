#include "DFRobot_mmWave_Radar.h"
#include <Servo.h>
#include <Adafruit_Fingerprint.h>
#include "wiring_private.h"

// ================= Hardware Abstraction =================

// Radar Pins 13/14
DFRobot_mmWave_Radar sensor(&Serial1); 

// Fingerprint Module
// RX=1, TX=0
Uart mySerial2 (&sercom3, 1, 0, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM3_Handler() { mySerial2.IrqHandler(); }

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial2);

Servo myServo;
const int SERVO_PIN = 5; 
const int LED_PIN = 6;

// ================= Servo Settings =================
const int SERVO_NEUTRAL = 20;     // Idle position
const int SERVO_PRESS_OPEN = 85;  // Unlock
const int SERVO_PRESS_CLOSE = 85;  // Lock

// ================= Thresholds =================
const int MAX_SCORE = 20;         // Radar maximum score
const int TRIGGER_THRESHOLD = 13; // Radar activation threshold
const int AUTO_CLOSE_DELAY = 10;  // 10s auto-lock timer
const float DET_DISTANCE = 1.6;   // Radar detection meters

// ================= System State =================
int detectionScore = 0;           // Start Score
int lastScore = -100;             
bool isLocked = true;             
unsigned long lastMotionTime = 0; // Timers
unsigned long lastPrintTime = 0; 
bool servoMoving = false;  

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  // Initialize fingerprint sensor pins
  mySerial2.begin(57600);
  pinPeripheral(1, PIO_SERCOM); 
  pinPeripheral(0, PIO_SERCOM); 

  // Initialize servo
  myServo.attach(SERVO_PIN);
  myServo.write(SERVO_NEUTRAL);
  delay(500);

  Serial.println("\n>>> System Boot <<<");
  
  // Initialize servo radar
  sensor.DetRangeCfg(0, DET_DISTANCE);
  sensor.OutputLatency(0, 0); // 0 latency
  finger.begin(57600);
}

void loop() {
  // Stop sensor input when servo moving to filter vibrations
  if (servoMoving) {
    if (millis() - lastMotionTime < 2000) { 
      delay(50); return; 
    }
    servoMoving = false; 
    detectionScore = 0; // Reset score after stabilization
  }

  // Fingerprint - Highest Priority
  int fingerID = checkFingerprint();
  if (fingerID != -1) {
    if (isLocked) {
      // Valid entry: Trigger unlock sequence
      Serial.print("\n[FINGERPRINT] ID #"); Serial.print(fingerID); Serial.println(" verified.");
      performUnlock("Fingerprint Unlock");
      return; 
    } 
    else {
      // timer
      Serial.println("\n[FINGERPRINT] Verified, Reset Timer.");
      lastMotionTime = millis(); 
      digitalWrite(LED_PIN, LOW); delay(100); digitalWrite(LED_PIN, HIGH);
    }
  }

  // Radar
  int val = sensor.readPresenceDetection();
  if (val == 1) detectionScore++; //detected something, score go up
  else detectionScore -= 1; // nothing detected, score go down

  if (detectionScore > MAX_SCORE) detectionScore = MAX_SCORE;
  if (detectionScore < 0) detectionScore = 0;

  // Logic

  if (isLocked) {
    // Mode: Monitoring
    if (detectionScore != lastScore) {
      Serial.print("[RADAR] Score: ");
      Serial.print(detectionScore); Serial.print(" / "); Serial.println(TRIGGER_THRESHOLD);
      lastScore = detectionScore;
    }
    // Threshold crossed -> Trigger unlock
    if (detectionScore >= TRIGGER_THRESHOLD) {
      performUnlock("Radar Unlock");
    }
  } 
  else {
    // Mode: Waiting
    long timePassed = millis() - lastMotionTime;
    long timeLeft = (AUTO_CLOSE_DELAY * 1000) - timePassed;

    if (timeLeft > 0) {
      if (millis() - lastPrintTime > 1000) {
        Serial.print("[TIMER] Auto-lock in: ");
        Serial.print((timeLeft / 1000) + 1); Serial.println("s");
        lastPrintTime = millis();
      }
    } 
    else {
      performLock();
    }
  }

  delay(50); // Sampling rate ~20Hz
}

// Servo Manipulation

void performUnlock(String reason) {
  Serial.println("\n Unlocking: " + reason);
  
  // Press
  myServo.write(SERVO_PRESS_OPEN); 
  digitalWrite(LED_PIN, HIGH);
  delay(1200); // Hold duration
  
  // Release
  myServo.write(SERVO_NEUTRAL);    

  // State Update
  isLocked = false;
  servoMoving = true;
  lastMotionTime = millis(); 
  detectionScore = MAX_SCORE; // Prevent instant lock
}

void performLock() {
  Serial.println("\n Lock (Timeout)");
  
  // Press
  myServo.write(SERVO_PRESS_CLOSE); 
  digitalWrite(LED_PIN, LOW);
  delay(1200); // Hold duration
  
  // Release
  myServo.write(SERVO_NEUTRAL);     

  // State Update
  isLocked = true;
  servoMoving = true;
  detectionScore = 0; // Reset integrator
  lastScore = 0;
}

// fingerprint detector
int checkFingerprint() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  return finger.fingerID;
}