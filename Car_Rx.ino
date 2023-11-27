/*

Demonstrates simple RX and TX operation.
Any of the Basic_TX examples can be used as a transmitter.
Please read through 'NRFLite.h' for a description of all the methods available in the library.

Radio    Arduino
CE    -> 9
CSN   -> 10 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI)
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND

*/

#include "SPI.h"
#include "NRFLite.h"

// nRF24L01 Settings
const static uint8_t RADIO_ID = 0;       // Our radio's id.  The transmitter will send to this id.
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;
const static uint8_t PIN_SERVO = 5;
const static uint8_t PIN_ESC= 6;

NRFLite _radio;

// Message definition
struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t vx;
    uint8_t vy;
};


RadioPacket _radioData;

// L298 Settings
#define INDICATOR_UP A2
#define INDICATOR_DOWN A3
#define INDICATOR_LEFT A1
#define INDICATOR_RIGHT A0

#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

#define SIGNIFICANT_MAGNITUDE 20

#define MAX_SPEED 255 //từ 0-255
#define MIN_SPEED 0

/***** Message Definition *****/
#define MOTOR_LEFT 1
#define MOTOR_RIGHT 2
#define FORWARD 1
#define BACKWARD 0

void rotateMotor(int id, bool dir) {
  if (id == MOTOR_RIGHT) {
    if (dir) {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
    }
    else {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
    }
  }

  if (id == MOTOR_LEFT) {
    if (dir) {
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    }
    else {
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    }    
  }
}

void moveForward() {
  rotateMotor(MOTOR_LEFT, FORWARD);
  rotateMotor(MOTOR_RIGHT, FORWARD);
  digitalWrite(INDICATOR_UP, HIGH);
  digitalWrite(INDICATOR_DOWN, LOW);
  digitalWrite(INDICATOR_LEFT, LOW);
  digitalWrite(INDICATOR_RIGHT, LOW);
}

void moveBackward() {
  rotateMotor(MOTOR_LEFT, BACKWARD);
  rotateMotor(MOTOR_RIGHT, BACKWARD);
  digitalWrite(INDICATOR_DOWN, HIGH);
  digitalWrite(INDICATOR_UP, LOW);
  digitalWrite(INDICATOR_LEFT, LOW);
  digitalWrite(INDICATOR_RIGHT, LOW);
}

void moveLeft() {
  // rotateMotor(MOTOR_LEFT, BACKWARD);
  rotateMotor(MOTOR_RIGHT, FORWARD);
  digitalWrite(INDICATOR_LEFT, HIGH);
  digitalWrite(INDICATOR_DOWN, LOW);
  digitalWrite(INDICATOR_UP, LOW);
  digitalWrite(INDICATOR_RIGHT, LOW);
}

void moveRight() {
  rotateMotor(MOTOR_LEFT, FORWARD);
  // rotateMotor(MOTOR_RIGHT, BACKWARD);
  digitalWrite(INDICATOR_RIGHT, HIGH);
  digitalWrite(INDICATOR_LEFT, LOW);
  digitalWrite(INDICATOR_DOWN, LOW);
  digitalWrite(INDICATOR_UP, LOW);
}

void stop() {
  digitalWrite(INDICATOR_UP, LOW);
  digitalWrite(INDICATOR_DOWN, LOW);
  digitalWrite(INDICATOR_LEFT, LOW);
  digitalWrite(INDICATOR_RIGHT, LOW);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}


void setup()
{
    Serial.begin(115200);
    while (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        Serial.println("Cannot communicate with radio");
        delay(1000);
    }

    // Actuators
    pinMode(INDICATOR_UP, OUTPUT);
    pinMode(INDICATOR_DOWN, OUTPUT);
    pinMode(INDICATOR_LEFT, OUTPUT);
    pinMode(INDICATOR_RIGHT, OUTPUT);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}

void loop()
{
    while (_radio.hasData())
    {
      _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.
      int vx = _radioData.vx;
      int vy = _radioData.vy;

      if (abs(vx) > abs(vy)) {
        if (vx > SIGNIFICANT_MAGNITUDE) {
          moveBackward();
        }
        else if (vx < -SIGNIFICANT_MAGNITUDE) {
          moveForward();
        }
        else {
          stop();
        }
      }
      else  {
        if (vy > SIGNIFICANT_MAGNITUDE) {
          moveLeft();
        }
        else if (vy < -SIGNIFICANT_MAGNITUDE) {
          moveRight();
        }   
        else {
          stop();
        }  
      }   
    }
}
