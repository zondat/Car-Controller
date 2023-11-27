/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/communication-between-two-esp32
 */

// ESP32: TCP CLIENT + A BUTTON/SWITCH
#include <esp_now.h>
#include <WiFi.h>

/************** Joystick Setting ****************/
#define PIN_VX 34
#define PIN_VY 35

#define DIV_FACT 100

#define ADC_MAX 4095
#define ADC_MIN 0

double toDegree(double radian) {
  return radian/PI*180;
}

/************** Wifi Setting ****************/
const char* ssid = "Thanh Dat";
const char* password = "12345678";

/************* Radio data ****************/
struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t signalEsc;
    uint8_t signalServo;
} _radioData;

esp_now_peer_info_t peerInfo;
uint8_t broadcastAddress[] = {0xA8, 0x42, 0xE3, 0x49, 0x95, 0xC0};

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    Serial.println("Delivery Success");
  }
  else{
    Serial.println("Delivery Fail");
  }
}

// Callback when data is received
// void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
//   memcpy(&_radioData, incomingData, sizeof(_radioData));
//   Serial.print("Bytes received: ");
//   Serial.println(len);
//   Serial.println("ESC signal: " + String(_radioData.signalEsc));
//   Serial.println("Servo signal: " + String(_radioData.signalServo));
// }

// Setup
void setup() {
  // Wifi settings
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  while (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    delay(1000);
  }

  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  // esp_now_register_recv_cb(OnDataRecv);
  
  // ADC Settings
  pinMode(PIN_VX, INPUT);
  pinMode(PIN_VY, INPUT);
}

void loop() {
  // Prepare data
  int8_t vx = (analogRead(PIN_VX) - ADC_MAX/2) / DIV_FACT;
  int8_t vy = (analogRead(PIN_VY) - ADC_MAX/2) / DIV_FACT;
  Serial.println("Vx: " + String(vx));
  Serial.println("Vy: " + String(vy));  

  uint8_t speed = map(vx, - ADC_MAX / (2 * DIV_FACT), ADC_MAX / (2 * DIV_FACT), 0, 180);
  // double arctanVal = vx/vy;
  // double angle = toDegree(atan(arctanVal)) - 45;
  uint8_t angle = map(vy, - ADC_MAX / (2 * DIV_FACT), ADC_MAX / (2 * DIV_FACT), 0, 90) + 45;

  Serial.println("speed: " + String(speed));
  Serial.println("angle: " + String(angle));
  _radioData.signalEsc = speed;
  _radioData.signalServo = angle;
  
  // Send data
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &_radioData, sizeof(_radioData));
  if (result == ESP_OK) Serial.println("Sent with success");
  else Serial.println("Error sending the data");
  delay(500);
}
