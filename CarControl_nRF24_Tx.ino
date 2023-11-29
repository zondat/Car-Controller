
#include "SPI.h"
#include "NRFLite.h"

// Joystick
#define PIN_VX A2
#define PIN_VY A1
#define SW_PIN A0
#define ADC_MAX 1023

// nRF24L01
const static uint8_t RADIO_ID = 1;             // Our radio's id.
const static uint8_t DESTINATION_RADIO_ID = 0; // Id of the radio we will transmit to.
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 53;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    int16_t vx;
    int16_t vy;
};

NRFLite _radio;
RadioPacket _radioData;

int offsetX = 0;
int offsetY = 0;
int nbSamples = 100;
void calib() {
  for (int i=0; i<nbSamples; i++) {
    offsetX += analogRead(PIN_VX) - ADC_MAX/2;
    offsetY += analogRead(PIN_VY) - ADC_MAX/2;
  }
  offsetX = offsetX / nbSamples;
  offsetY = offsetY / nbSamples;
}

void setup()
{
  Serial.begin(115200);
  while (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
  {
      Serial.println("Cannot communicate with radio");
      delay (1000); // Wait here forever.
  }
  Serial.println("Connected...");

  // Joystick
  pinMode(PIN_VX, INPUT);
  pinMode(PIN_VY, INPUT);

  // calib
  calib();
}

void loop()
{
    int16_t vx = analogRead(PIN_VX) - offsetX - ADC_MAX/2;
    int16_t vy = analogRead(PIN_VY) - offsetY - ADC_MAX/2;

    Serial.println("vx: " + String(vx));
    Serial.println("vy: " + String(vy));

    _radioData.vx = vx;
    _radioData.vy = vy;
  
    if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))) Serial.println("Message sent");

    delay(1000);
}
