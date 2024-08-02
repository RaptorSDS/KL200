#ifndef XKC_KL200_H
#define XKC_KL200_H

#include <Arduino.h>

class XKC_KL200 {
public:
    XKC_KL200(HardwareSerial& serial);
    void begin(long baudRate = 9600);
    bool restoreFactorySettings();
    bool changeAddress(uint16_t address);
    bool changeBaudRate(uint8_t baudRate);
    bool setUploadMode(bool autoUpload);
    bool setUploadInterval(uint8_t interval);
    bool setLEDMode(uint8_t mode);
    bool setRelayMode(uint8_t mode);
    uint16_t readDistance();
    bool available();
    uint16_t getDistance();
    uint16_t getLastReceivedDistance(); // Neue Funktion

private:
    HardwareSerial& _serial;
    uint16_t _distance;
    uint16_t _lastReceivedDistance; // Speichert den letzten empfangenen Wert
    bool _available;
    void sendCommand(const uint8_t* command, uint8_t length);
    uint8_t calculateChecksum(const uint8_t* data, uint8_t length);
};

#endif
