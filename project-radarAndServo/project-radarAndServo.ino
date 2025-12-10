#include "DFRobot_mmWave_Radar.h"
#include <Servo.h>
#include <Adafruit_Fingerprint.h>
#include "wiring_private.h"

DFRobot_mmWave_Radar sensor(&Serial1); // mmwave radar Pin 13/14

// fingerprint Pin 0/1: RX=1, TX=0
Uart mySerial2 (&sercom3, 1, 0, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM3_Handler() { mySerial2.IrqHandler(); }

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial2);

Servo myServo;
const int SERVO_PIN = 5;
const int LED_PIN = 6;

// ================= parameters =================
const int MAX_SCORE = 20;         
const int TRIGGER_THRESHOLD = 13;  
const int AUTO_CLOSE_DELAY = 30; // countdown  
const float DET_DISTANCE = 1.6;    

// ================= variables =================
int detectionScore = 0;
int lastScore = -100;         
bool isLocked = true;         
unsigned long lastMotionTime = 0; 
unsigned long lastPrintTime = 0;  
bool servoMoving = false;         

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  mySerial2.begin(57600);
  pinPeripheral(1, PIO_SERCOM); 
  pinPeripheral(0, PIO_SERCOM); 

  myServo.attach(SERVO_PIN);
  myServo.write(20); 
  delay(500);

  Serial.println("\n================= Setup Complete =================");
  
  sensor.DetRangeCfg(0, DET_DISTANCE);
  sensor.OutputLatency(0, 0); 
  finger.begin(57600);
}

void loop() {
  if (servoMoving) {
    if (millis() - lastMotionTime < 1500) {
      delay(50); return; 
    }
    servoMoving = false; 
    detectionScore = 0; 
  }

  // fingerprint sensing

  int fingerID = checkFingerprint();
  if (fingerID != -1) {
    // if door locked, opens
    if (isLocked) {
      Serial.print("\n>>> [Fingerprint] Evaluated ID #");
      Serial.println(fingerID);
      performUnlock("Fingerprint Unlock");
      return; // skip radar
    } 
    // if door opens, reset timer
    else {
      Serial.println("\n [FIGERPRINT] Evaluated, Reset Countdown ");
      lastMotionTime = millis(); //reset timer
      digitalWrite(LED_PIN, LOW); delay(100); digitalWrite(LED_PIN, HIGH);
    }
  }

  // score based radar function
  // if theres any movements picked, the score will go up. if nothing, the score will decrease
  // preventing false detection, making it safer as a lock

  int val = sensor.readPresenceDetection();
  if (val == 1) detectionScore++; //score go up
  else detectionScore -= 1; 

  if (detectionScore > MAX_SCORE) detectionScore = MAX_SCORE;
  if (detectionScore < 0) detectionScore = 0;


  // locked - score accumulating
  if (isLocked) {
    if (detectionScore != lastScore) {
      Serial.print("[LISTEN] SCORE: ");
      Serial.print(detectionScore);
      Serial.print(" / ");
      Serial.println(TRIGGER_THRESHOLD);
      lastScore = detectionScore;
    }
    if (detectionScore >= TRIGGER_THRESHOLD) {
      performUnlock("TRIGGER");
    }
  } 
  
  // open - countdown
  else {
    long timePassed = millis() - lastMotionTime;
    long timeLeft = (AUTO_CLOSE_DELAY * 1000) - timePassed;

    if (timeLeft > 0) {
      if (millis() - lastPrintTime > 1000) {
        Serial.print("[Close Countdown] ");
        Serial.print((timeLeft / 1000) + 1);
        Serial.println("s");
        lastPrintTime = millis();
      }
    } 
    else {
      performLock();
    }
  }

  delay(50); 
}

void performUnlock(String reason) {
  Serial.println("\n[OPEN] " + reason + " -> OPEN");
  myServo.write(75);
  digitalWrite(LED_PIN, HIGH);
  isLocked = false;
  servoMoving = true;
  lastMotionTime = millis(); 
  detectionScore = MAX_SCORE; 
}

void performLock() {
  Serial.println("\n<<< [TIME UP] CLOSE! <<<\n");
  myServo.write(0);
  digitalWrite(LED_PIN, LOW);
  isLocked = true;
  servoMoving = true;
  detectionScore = 0; 
  lastScore = 0;
}

int checkFingerprint() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  return finger.fingerID;
}