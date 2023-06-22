#include <VirtualWire.h>

#define INDICATOR_UP A2
#define INDICATOR_DOWN A3
#define INDICATOR_LEFT A1
#define INDICATOR_RIGHT A0

#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

#define DATA_PIN 2

#define SIGNIFICANT_MAGNITUDE 18

#define MAX_SPEED 255 //từ 0-255
#define MIN_SPEED 0

/***** Message Definition *****/
#define MOTOR_LEFT 1
#define MOTOR_RIGHT 2
#define FORWARD 1
#define BACKWARD 0
struct Packet {
  int vx;
  int vy;
};

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

/******************************/
#define MESS_LEN sizeof(Packet)
byte msg[MESS_LEN];// biến lưu dữ liệu nhận được
byte msgLen = MESS_LEN;

void setup()
{
  // Serial.begin(9600);
  // Serial.println("Rx ready ...");

  pinMode(INDICATOR_UP, OUTPUT);
  pinMode(INDICATOR_DOWN, OUTPUT);
  pinMode(INDICATOR_LEFT, OUTPUT);
  pinMode(INDICATOR_RIGHT, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  vw_setup(1024);
  vw_set_rx_pin(DATA_PIN);
  vw_rx_start();

  // Serial.println("Begin receiving");

  // moveBackward();
}


void loop()
{
  if (vw_get_message(msg, &msgLen))
  {
    Serial.println("Received");
    Packet *receivedPacket = (Packet*)msg;
    delay(100);

    int vx = receivedPacket->vx;
    int vy = receivedPacket->vy;
    
    // Serial.println("vx = " + String(vx));
    // Serial.println("vy = " + String(vy));

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
