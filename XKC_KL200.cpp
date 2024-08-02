#include "XKC_KL200.h"

XKC_KL200::XKC_KL200(HardwareSerial& serial) : _serial(serial), _available(false), _distance(0), _lastReceivedDistance(0) {}

void XKC_KL200::begin(long baudRate) {
    _serial.begin(baudRate);
}

bool XKC_KL200::restoreFactorySettings() {
    uint8_t command[] = {0x62, 0x39, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return true;
}

bool XKC_KL200::changeAddress(uint16_t address) {
    if (address > 0xFFFE) return false;
    uint8_t command[] = {0x62, 0x32, 0x09, 0xFF, 0xFF, (uint8_t)(address >> 8), (uint8_t)address, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return true;
}

bool XKC_KL200::changeBaudRate(uint8_t baudRate) {
    if (baudRate > 9) return false;
    uint8_t command[] = {0x62, 0x30, 0x09, 0xFF, 0xFF, baudRate, 0x00, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return true;
}

bool XKC_KL200::setUploadMode(bool autoUpload) {
    uint8_t mode = autoUpload ? 1 : 0;
    uint8_t command[] = {0x62, 0x34, 0x09, 0xFF, 0xFF, mode, 0x00, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return true;
}

bool XKC_KL200::setUploadInterval(uint8_t interval) {
    if (interval < 1 || interval > 100) return false;
    uint8_t command[] = {0x62, 0x35, 0x09, 0xFF, 0xFF, interval, 0x00, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return true;
}

bool XKC_KL200::setLEDMode(uint8_t mode) {
    if (mode > 3) return false;
    uint8_t command[] = {0x62, 0x37, 0x09, 0xFF, 0xFF, mode, 0x00, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return true;
}

bool XKC_KL200::setRelayMode(uint8_t mode) {
    if (mode > 1) return false;
    uint8_t command[] = {0x62, 0x38, 0x09, 0xFF, 0xFF, mode, 0x00, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return true;
}

uint16_t XKC_KL200::readDistance() {
    uint8_t command[] = {0x62, 0x33, 0x09, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);

    while (_serial.available() < 9);
    uint8_t response[9];
    _serial.readBytes(response, 9);

    if (response[0] == 0x62 && response[1] == 0x33) {
        uint8_t length = response[2];
        uint16_t address = (response[3] << 8) | response[4];
        uint16_t rawDistance = (response[5] << 8) | response[6];
        uint8_t checksum = response[8];
        uint8_t calcChecksum = calculateChecksum(response, 8);

        if (checksum == calcChecksum) {
            _distance = rawDistance;
            _lastReceivedDistance = rawDistance;
            _available = true;
        }
    }
    return _distance;
}

bool XKC_KL200::available() {
    return _available;
}

uint16_t XKC_KL200::getDistance() {
    _available = false;
    return _distance;
}

uint16_t XKC_KL200::getLastReceivedDistance() {
    return _lastReceivedDistance;
}

void XKC_KL200::sendCommand(const uint8_t* command, uint8_t length) {
    _serial.write(command, length);
}

uint8_t XKC_KL200::calculateChecksum(const uint8_t* data, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; ++i) {
        checksum ^= data[i];
    }
    return checksum;
}
