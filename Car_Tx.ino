#include <VirtualWire.h>

#define INDICATOR_LED 3
#define VX_PIN A2
#define VY_PIN A1
#define SW_PIN A0

/***** Message Definition *****/
struct Packet {
  int vx;
  int vy;
};

#define MESS_LEN sizeof(Packet)

/******************************/
int readJoystick(int pin) {
  return analogRead(pin) - 512;
}

void blink(int ms) {
  digitalWrite(INDICATOR_LED, HIGH);
  delay(ms);
  digitalWrite(INDICATOR_LED, LOW);
  delay(ms);
}

/*********************************/
Packet* mess;

void setup()
{
  pinMode(INDICATOR_LED, OUTPUT);
  pinMode(VX_PIN, INPUT);
  pinMode(VY_PIN, INPUT);

  // Serial.begin(9600);// giao tiếp Serial với baudrate 9600
  // Serial.println("Ready.........");
  vw_set_ptt_inverted(true);// yêu cầu cho RF link modules
  vw_setup(1024);// cài đặt tốc độ truyền tín hiệu
  vw_set_tx_pin(2);// cài đặt chân digital để phát tín hiệu

  mess = (Packet*)malloc(MESS_LEN);
}

void loop()
{
  mess->vx = readJoystick(VX_PIN);
  mess->vy = readJoystick(VY_PIN);

  // Serial.println("vx = " + String(mess->vx));
  // Serial.println("vy = " + String(mess->vy));

  vw_send((uint8_t*)mess, 2*MESS_LEN);// gửi tín hiệu đi
  vw_wait_tx();
  blink(100);
}
