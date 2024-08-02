#ifndef XKC_KL200_H
#define XKC_KL200_H

#include "Arduino.h"

class XKC_KL200 {
public:
    XKC_KL200(HardwareSerial& serial);

    void begin();
    bool setUploadMode(bool mode);
    bool setLEDMode(uint8_t mode);
    bool changeBaudRate(uint8_t rate);
    bool setUploadInterval(uint8_t interval);
    bool setRelayMode(uint8_t mode);
    uint16_t readDistance();
    bool available();
    uint16_t getDistance();
    uint16_t getLastReceivedDistance();
    bool HardReset(uint8_t type);
    bool SoftReset(uint8_t type);
    bool setCommunicationMode(uint8_t mode);

private:
    HardwareSerial& _serial;
    uint16_t _lastReceivedDistance;
    bool _autoMode;
};

#endif
