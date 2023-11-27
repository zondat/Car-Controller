
#include "SPI.h"
#include "NRFLite.h"

// Joystick
#define VX_PIN A2
#define VY_PIN A1
#define SW_PIN A0

int readJoystick(int pin) {
  return analogRead(pin) - 512;
}

double toDegree(double radian) {
  return radian/PI*180;
}

// nRF24L01
const static uint8_t RADIO_ID = 1;             // Our radio's id.
const static uint8_t DESTINATION_RADIO_ID = 0; // Id of the radio we will transmit to.
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 53;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t signalEsc;
    uint8_t signalServo;
};

NRFLite _radio;
RadioPacket _radioData;

void setup()
{
    Serial.begin(115200);
    
    // By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
    // Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
    // You can run the 'ChannelScanner' example to help select the best channel for your environment.
    // You can assign a different bitrate and channel as shown below.
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100) // THE DEFAULT
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)
    
    while (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        Serial.println("Cannot communicate with radio");
        delay (1000); // Wait here forever.
    }
    Serial.println("Connected...");
    
    // _radioData.FromRadioId = RADIO_ID;

    // Joystick
    pinMode(VX_PIN, INPUT);
    pinMode(VY_PIN, INPUT);
}

void loop()
{
    int32_t vx = readJoystick(VX_PIN);
    int32_t vy = readJoystick(VY_PIN);

    Serial.println("vx: " + String(vx));
    Serial.println("vy: " + String(vy));
    Serial.println("sqrt(vx*vx + vy*vy): " + String(sqrt(vx*vx + vy*vy)));
    uint8_t speed = map(sqrt(vx*vx + vy*vy), 0, 722, 0, 180);
    uint8_t angle = map(toDegree(atan(vx/vy)), 0, 512, -90, 90);

    _radioData.signalEsc = speed;
    _radioData.signalServo = angle;

    Serial.println("Speed: " + String(speed));
    Serial.println("Angle: " + String(angle));   
    if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))) Serial.println("Message sent");

    delay(100);
}
