#include "XKC_KL200.h"

// Baudraten-Tabelle laut Datenblatt (Index 0-9)
static const uint32_t XKC_BAUD_TABLE[] = {
    2400, 4800, 9600, 14400, 19200, 38400, 56000, 57600, 115200, 128000
};

XKC_KL200::XKC_KL200(HardwareSerial& serial, uint16_t address) :
    _serial(serial),
    _hwSerial(&serial),
    _address(address),
    _available(false),
    _autoMode(false),
    _distance(0),
    _lastReceivedDistance(0),
    _lastRawValue(0),
    _minDistance(1),
    _maxDistance(0xFFFF),
    _lastError(XKC_SUCCESS),
    _checksumErrors(0),
    _rxIndex(0) {}

XKC_KL200::XKC_KL200(Stream& serial, uint16_t address) :
    _serial(serial),
    _hwSerial(nullptr),
    _address(address),
    _available(false),
    _autoMode(false),
    _distance(0),
    _lastReceivedDistance(0),
    _lastRawValue(0),
    _minDistance(1),
    _maxDistance(0xFFFF),
    _lastError(XKC_SUCCESS),
    _checksumErrors(0),
    _rxIndex(0) {}

void XKC_KL200::begin(long baudRate) {
    if (_hwSerial) {
        _hwSerial->begin(baudRate);
    }
    delay(100); // Kurze Pause für die Initialisierung des Sensors
    clearInput();
}

uint8_t XKC_KL200::hardReset() {
    // Datenbits 0xFFFF, Parameter FE im Antwort-Byte: Xingkechuang default
    return sendAndWait(XKC_HEADER_B, XKC_CMD_RESET, 0xFFFF, 0xFE);
}

uint8_t XKC_KL200::softReset() {
    // Parameter FD: Benutzer-Standardkonfiguration
    return sendAndWait(XKC_HEADER_B, XKC_CMD_RESET, 0xFFFF, 0xFD);
}

uint8_t XKC_KL200::changeAddress(uint16_t address) {
    if (address > 0xFFFE) return _lastError = XKC_INVALID_PARAMETER;

    uint8_t result = sendAndWait(XKC_HEADER_B, XKC_CMD_ADDRESS, address);
    // Bei Broadcast-Adresse bleibt die Bibliothek bei 0xFFFF, das funktioniert weiterhin
    if (result == XKC_SUCCESS && _address != XKC_BROADCAST_ADDRESS) {
        _address = address;
    }
    return result;
}

uint8_t XKC_KL200::changeBaudRate(uint8_t baudRate) {
    if (baudRate > 9) return _lastError = XKC_INVALID_PARAMETER;

    uint8_t result = sendAndWait(XKC_HEADER_B, XKC_CMD_BAUDRATE, baudRate);
    if (result == XKC_SUCCESS && _hwSerial) {
        // Sensor spricht ab jetzt mit der neuen Baudrate
        _hwSerial->flush();
        _hwSerial->begin(XKC_BAUD_TABLE[baudRate]);
        clearInput();
    }
    return result;
}

uint8_t XKC_KL200::setUploadMode(bool autoUpload) {
    uint8_t result = sendAndWait(XKC_HEADER_B, XKC_CMD_UPLOAD_MODE, autoUpload ? 1 : 0);
    if (result == XKC_SUCCESS) {
        _autoMode = autoUpload;
    }
    return result;
}

uint8_t XKC_KL200::setUploadInterval(uint8_t interval) {
    if (interval < 1 || interval > 100) return _lastError = XKC_INVALID_PARAMETER;
    return sendAndWait(XKC_HEADER_B, XKC_CMD_INTERVAL, interval);
}

uint8_t XKC_KL200::setLEDMode(uint8_t mode) {
    if (mode > 3) return _lastError = XKC_INVALID_PARAMETER;
    return sendAndWait(XKC_HEADER_B, XKC_CMD_LED_MODE, mode);
}

uint8_t XKC_KL200::setRelayMode(uint8_t mode) {
    if (mode > 1) return _lastError = XKC_INVALID_PARAMETER;
    return sendAndWait(XKC_HEADER_B, XKC_CMD_RELAY_MODE, mode);
}

uint8_t XKC_KL200::setCommunicationMode(uint8_t mode) {
    if (mode > 1) return _lastError = XKC_INVALID_PARAMETER;
    // Senden mit Header 0x61 ('a0'), Antwort kommt mit 0x62 0x30
    return sendAndWait(XKC_HEADER_A, XKC_CMD_LINE_MODE, mode);
}

uint8_t XKC_KL200::setTrailingMode(uint8_t mode) {
    if (mode > 2) return _lastError = XKC_INVALID_PARAMETER;
    // Senden mit Header 0x61 ('a2'), Antwort kommt mit 0x62 0x32
    return sendAndWait(XKC_HEADER_A, XKC_CMD_TRAILING_MODE, mode);
}

void XKC_KL200::setDistanceRange(uint16_t minDistance, uint16_t maxDistance) {
    _minDistance = minDistance;
    _maxDistance = maxDistance;
}

uint16_t XKC_KL200::readDistance(unsigned long timeout) {
    if (!_autoMode) {
        // Alte Bytes verwerfen, damit wir die Antwort auf genau diese Abfrage lesen
        clearInput();
        sendCommand(XKC_HEADER_B, XKC_CMD_DISTANCE, 0x0000);
    }

    _lastError = waitForFrame(XKC_CMD_DISTANCE, timeout);
    if (_lastError != XKC_SUCCESS) {
        return 0;
    }
    if (!handleDistanceFrame(_rxBuffer)) {
        _lastError = XKC_OUT_OF_RANGE;
        return 0;
    }
    _available = false;
    return _distance;
}

bool XKC_KL200::available() {
    while (pollFrame()) {
        if (_rxBuffer[1] == XKC_CMD_DISTANCE) {
            handleDistanceFrame(_rxBuffer);
        }
    }
    return _available;
}

bool XKC_KL200::processAutoData() {
    return available();
}

uint16_t XKC_KL200::getDistance() {
    _available = false;
    return _distance;
}

uint16_t XKC_KL200::getLastReceivedDistance() {
    return _lastReceivedDistance;
}

uint16_t XKC_KL200::getLastRawValue() {
    return _lastRawValue;
}

uint8_t XKC_KL200::getLastError() {
    return _lastError;
}

uint16_t XKC_KL200::getChecksumErrorCount() {
    return _checksumErrors;
}

uint8_t XKC_KL200::calculateChecksum(const uint8_t* data, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; ++i) {
        checksum ^= data[i];
    }
    return checksum;
}

void XKC_KL200::sendCommand(uint8_t header, uint8_t command, uint16_t data, uint8_t answer) {
    uint8_t frame[XKC_FRAME_LENGTH] = {
        header, command, XKC_FRAME_LENGTH,
        (uint8_t)(_address >> 8), (uint8_t)_address,
        (uint8_t)(data >> 8), (uint8_t)data,
        answer, 0x00
    };
    frame[8] = calculateChecksum(frame, 8);
    _serial.write(frame, XKC_FRAME_LENGTH);
    _serial.flush();
}

uint8_t XKC_KL200::sendAndWait(uint8_t header, uint8_t command, uint16_t data, uint8_t answer) {
    sendCommand(header, command, data, answer);
    _lastError = waitForFrame(command, XKC_DEFAULT_TIMEOUT);
    if (_lastError == XKC_SUCCESS && _rxBuffer[7] != XKC_ACK) {
        _lastError = XKC_RESPONSE_ERROR;
    }
    return _lastError;
}

void XKC_KL200::clearInput() {
    while (_serial.available()) {
        _serial.read();
    }
    _rxIndex = 0;
}

bool XKC_KL200::pollFrame() {
    while (_serial.available()) {
        uint8_t b = (uint8_t)_serial.read();

        // Auf Header synchronisieren (Antworten kommen immer mit 0x62)
        if (_rxIndex == 0 && b != XKC_HEADER_B) {
            continue;
        }

        _rxBuffer[_rxIndex++] = b;

        // Laengenbyte muss 0x09 sein, sonst war der Header nur ein Datenbyte
        if (_rxIndex == 3 && _rxBuffer[2] != XKC_FRAME_LENGTH) {
            resync();
            continue;
        }

        if (_rxIndex == XKC_FRAME_LENGTH) {
            if (calculateChecksum(_rxBuffer, 8) == _rxBuffer[8]) {
                _rxIndex = 0;
                return true;
            }
            _checksumErrors++;
            resync();
        }
    }
    return false;
}

void XKC_KL200::resync() {
    // Ab Position 1 nach dem nächsten möglichen Frame-Anfang suchen,
    // damit ein echter Frame hinter einem falschen 0x62 nicht verloren geht
    for (uint8_t start = 1; start < _rxIndex; start++) {
        uint8_t remaining = _rxIndex - start;
        if (_rxBuffer[start] == XKC_HEADER_B &&
            (remaining < 3 || _rxBuffer[start + 2] == XKC_FRAME_LENGTH)) {
            memmove(_rxBuffer, _rxBuffer + start, remaining);
            _rxIndex = remaining;
            return;
        }
    }
    _rxIndex = 0;
}

uint8_t XKC_KL200::waitForFrame(uint8_t expectedCmd, unsigned long timeout) {
    uint16_t checksumErrorsBefore = _checksumErrors;
    unsigned long startTime = millis();

    do {
        while (pollFrame()) {
            if (_rxBuffer[1] == expectedCmd) {
                return XKC_SUCCESS;
            }
            // Im Auto-Modus laufen Distanz-Frames zwischen den Antworten weiter
            if (_rxBuffer[1] == XKC_CMD_DISTANCE) {
                handleDistanceFrame(_rxBuffer);
            }
        }
        yield(); // Für ESP8266/ESP32, um WDT-Resets zu vermeiden
    } while (millis() - startTime < timeout);

    return (_checksumErrors != checksumErrorsBefore) ? XKC_CHECKSUM_ERROR : XKC_TIMEOUT;
}

bool XKC_KL200::handleDistanceFrame(const uint8_t* frame) {
    uint16_t rawDistance = ((uint16_t)frame[5] << 8) | frame[6];
    _lastRawValue = rawDistance;

    if (rawDistance < _minDistance || rawDistance > _maxDistance) {
        return false;
    }

    _distance = rawDistance;
    _lastReceivedDistance = rawDistance;
    _available = true;
    return true;
}
