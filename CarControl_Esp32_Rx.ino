#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h> 
#include "ESC.h" // RC_ESP library installed by Library Manager

// ===========================
// Motor && Servo
// ===========================
const static uint8_t PIN_SERVO = 12;
const static uint8_t PIN_ESC= 13;

Servo esc;
Servo servo;

// Message definition
struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t signalEsc;
    uint8_t signalServo;
};
RadioPacket _radioData;

// Peer to peer communication
esp_now_peer_info_t peerInfo;
uint8_t broadcastAddress[] = {0xB0, 0xA7, 0x32, 0xDD, 0x62, 0xA4};

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
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  Serial.println("Received message");
  memcpy(&_radioData, incomingData, sizeof(_radioData));
  
  Serial.println(len);
  Serial.println("ESC signal: " + String(_radioData.signalEsc));
  Serial.println("Servo signal: " + String(_radioData.signalServo));
  esc.write(_radioData.signalEsc);
  servo.write(_radioData.signalServo);
}


void setup()
{
  Serial.begin(115200);

  // Peer to peer communication settings
  WiFi.mode(WIFI_STA);
  Serial.println("Initializing ESP-NOW...");
  while (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    delay(1000);
  }
  Serial.println("Init success");
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
  esp_now_register_recv_cb(OnDataRecv);
  
  servo.attach(PIN_SERVO); 
  esc.attach(PIN_ESC, 880, 2000); 
}

void loop()
{}