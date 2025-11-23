#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

#define RED_PIN 8
#define GREEN_PIN 6
#define BUT_IN 3
#define BUT_OUT 2
#define SERVO_PIN 7
#define SS_PIN 10
#define RST_PIN 9

#define SERVO_POS_CLOSE 0
#define SERVO_POS_OPEN 90
#define DELAY_TIME 100
#define DELAY_TIME_LED 1000

unsigned long prevBlink = 0;
unsigned long prevTime = 0;
bool redLedState = false;
bool greenLedState = false;

// UID of valid RFID card
byte validUID[4] = {0x6A, 0x5B, 0xFB, 0x69};

Servo myServo;
MFRC522 mfrc522(SS_PIN, RST_PIN);

enum State {
  DOOR_CLOSED,
  DOOR_OPEN_ALERT_ON,    // open door to go outside, turn on alert
  DOOR_OPEN_ALERT_OFF,   // already touch RFID card, turn off alert
  DOOR_OPEN_NORMAL       // open door to go inside
};

State doorState = DOOR_CLOSED;

bool prevButInState = HIGH;
bool prevButOutState = HIGH;

void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);
  myServo.write(SERVO_POS_CLOSE);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  pinMode(BUT_IN, INPUT_PULLUP);
  pinMode(BUT_OUT, INPUT_PULLUP);

  SPI.begin();
  mfrc522.PCD_Init();

  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
}

bool isButtonPressed(int pin) {
  return digitalRead(pin) == LOW;
}

bool checkUID() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    return false;

  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != validUID[i]) {
      Serial.println(mfrc522.uid.uidByte[i]);
      return false;
    }
  }
  return true;
}

void loop() {
  bool currentButInState = digitalRead(BUT_IN);
  bool currentButOutState = digitalRead(BUT_OUT);

  if (prevButInState == LOW && currentButInState == HIGH) {
    Serial.println("BUT_IN released");
    if (doorState == DOOR_OPEN_NORMAL) {
      myServo.write(SERVO_POS_CLOSE);
      doorState = DOOR_CLOSED;
      delay(DELAY_TIME);
    } else if (doorState == DOOR_CLOSED) {
      myServo.write(SERVO_POS_OPEN);
      doorState = DOOR_OPEN_ALERT_ON;
      delay(DELAY_TIME);
    }
  }

  if (prevButOutState == LOW && currentButOutState == HIGH) {
    Serial.println("BUT_OUT released");
    if (doorState == DOOR_CLOSED) {
      myServo.write(SERVO_POS_OPEN);
      doorState = DOOR_OPEN_NORMAL;
      delay(DELAY_TIME);
    } else if (doorState == DOOR_OPEN_ALERT_OFF) {
      myServo.write(SERVO_POS_CLOSE);
      doorState = DOOR_CLOSED;
      delay(DELAY_TIME);
    }
  }

  prevButInState = currentButInState;
  prevButOutState = currentButOutState;

  switch (doorState) {
    case DOOR_CLOSED:
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      break;

    case DOOR_OPEN_ALERT_ON:
      // blink red led and buzzer
      if (millis() - prevBlink >= 1000) {
        prevBlink = millis();
        redLedState = !redLedState;
        digitalWrite(RED_PIN, redLedState);
      }
      // check RFID card to turn off alert
      if (checkUID()) {
        doorState = DOOR_OPEN_ALERT_OFF;
      }
      break;

    case DOOR_OPEN_ALERT_OFF:
      digitalWrite(RED_PIN, LOW);
      // blink green led to notify valid card
      if (millis() - prevTime >= 1000) {
        prevTime = millis();
        greenLedState = !greenLedState;
        digitalWrite(GREEN_PIN, greenLedState);
      }
      break;

    case DOOR_OPEN_NORMAL:
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      break;
  }
}
