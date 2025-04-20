#include "XKC_KL200.h"

XKC_KL200::XKC_KL200(HardwareSerial& serial) : 
    _serial(serial), 
    _available(false), 
    _distance(0), 
    _lastReceivedDistance(0),
    _autoMode(false) {}

void XKC_KL200::begin(long baudRate) {
    _serial.begin(baudRate);
    delay(100); // Kurze Pause für die Initialisierung des Sensors
}

uint8_t XKC_KL200::hardReset() {
    // Befehl zum Zurücksetzen auf Werkseinstellungen (Xingkechuang default)
    uint8_t command[] = {0x62, 0x39, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return waitForResponse(0x39, 1000);
}

uint8_t XKC_KL200::softReset() {
    // Befehl zum Zurücksetzen auf Benutzereinstellungen
    uint8_t command[] = {0x62, 0x39, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return waitForResponse(0x39, 1000);
}

uint8_t XKC_KL200::changeAddress(uint16_t address) {
    if (address > 0xFFFE) return XKC_INVALID_PARAMETER;
    
    // Befehl zum Ändern der Adresse
    uint8_t command[] = {0x62, 0x32, 0x09, 0xFF, 0xFF, (uint8_t)(address >> 8), (uint8_t)address, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return waitForResponse(0x32, 1000);
}

uint8_t XKC_KL200::changeBaudRate(uint8_t baudRate) {
    if (baudRate > 9) return XKC_INVALID_PARAMETER;
    
    // Befehl zum Ändern der Baudrate
    // Laut Dokumentation: DataHigh=0x00, DataLow=baudRate
    uint8_t command[] = {0x62, 0x30, 0x09, 0xFF, 0xFF, 0x00, baudRate, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return waitForResponse(0x30, 1000);
}

uint8_t XKC_KL200::setUploadMode(bool autoUpload) {
    _autoMode = autoUpload;
    uint8_t mode = autoUpload ? 1 : 0;
    
    // Befehl zum Setzen des Upload-Modus
    // Laut Dokumentation: DataHigh=0x00, DataLow=mode
    uint8_t command[] = {0x62, 0x34, 0x09, 0xFF, 0xFF, 0x00, mode, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return waitForResponse(0x34, 1000);
}

uint8_t XKC_KL200::setUploadInterval(uint8_t interval) {
    if (interval < 1 || interval > 100) return XKC_INVALID_PARAMETER;
    
    // Befehl zum Setzen des Upload-Intervalls
    // Laut Dokumentation: DataHigh=0x00, DataLow=interval
    uint8_t command[] = {0x62, 0x35, 0x09, 0xFF, 0xFF, 0x00, interval, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return waitForResponse(0x35, 1000);
}

uint8_t XKC_KL200::setLEDMode(uint8_t mode) {
    if (mode > 3) return XKC_INVALID_PARAMETER;
    
    // Befehl zum Setzen des LED-Modus
    // Laut Dokumentation: DataHigh=0x00, DataLow=mode
    uint8_t command[] = {0x62, 0x37, 0x09, 0xFF, 0xFF, 0x00, mode, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return waitForResponse(0x37, 1000);
}

uint8_t XKC_KL200::setRelayMode(uint8_t mode) {
    if (mode > 1) return XKC_INVALID_PARAMETER;
    
    // Befehl zum Setzen des Relais-Modus
    // Laut Dokumentation: DataHigh=0x00, DataLow=mode
    uint8_t command[] = {0x62, 0x38, 0x09, 0xFF, 0xFF, 0x00, mode, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return waitForResponse(0x38, 1000);
}

uint8_t XKC_KL200::setCommunicationMode(uint8_t mode) {
    if (mode > 1) return XKC_INVALID_PARAMETER;
    
    // Befehl zum Setzen des Kommunikationsmodus
    // Laut Dokumentation: Command=0x61, 0x30 (nicht 0x62, 0x31)
    // DataHigh=0x00, DataLow=mode
    uint8_t command[] = {0x61, 0x30, 0x09, 0xFF, 0xFF, 0x00, mode, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);
    return waitForResponse(0x30, 1000);
}

uint16_t XKC_KL200::readDistance(unsigned long timeout) {
    if (_autoMode) {
        // Im automatischen Modus sollte processAutoData() verwendet werden
        return _lastReceivedDistance;
    }
    
    // Befehl zum Lesen der Distanz
    uint8_t command[] = {0x62, 0x33, 0x09, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
    command[8] = calculateChecksum(command, 8);
    sendCommand(command, 9);

    // Auf Antwort warten mit Timeout
    unsigned long startTime = millis();
    while (_serial.available() < 9) {
        if (millis() - startTime > timeout) {
            return _lastReceivedDistance; // Timeout, letzte bekannte Distanz zurückgeben
        }
        yield(); // Für ESP8266/ESP32, um WDT-Resets zu vermeiden
    }
    
    // Antwort lesen und verarbeiten
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
            return _distance;
        }
    }
    
    // Bei Fehler letzte bekannte Distanz zurückgeben
    return _lastReceivedDistance;
}

bool XKC_KL200::available() {
    // Prüfen, ob neue Daten im automatischen Modus verfügbar sind
    if (_autoMode && _serial.available() >= 9) {
        return true;
    }
    return _available;
}

bool XKC_KL200::processAutoData() {
    if (!_autoMode || _serial.available() < 9) {
        return false;
    }

    // Daten aus dem seriellen Buffer lesen
    uint8_t response[9];
    _serial.readBytes(response, 9);

    // Prüfen, ob es sich um eine gültige Distanzmeldung handelt
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
            return true;
        }
    }

    // Ungültige Daten aus dem Buffer entfernen
    _serial.read();
    return false;
}

bool XKC_KL200::available() {
    // Prüfen, ob neue Daten im automatischen Modus verfügbar sind
    if (_autoMode && _serial.available() >= 9) {
        return true;
    }
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
    // Befehl senden und warten, bis alle Daten übertragen sind
    _serial.write(command, length);
    _serial.flush();
}

uint8_t XKC_KL200::calculateChecksum(const uint8_t* data, uint8_t length) {
    // XOR-Prüfsumme berechnen
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; ++i) {
        checksum ^= data[i];
    }
    return checksum;
}

uint8_t XKC_KL200::waitForResponse(uint8_t expectedCmd, unsigned long timeout) {
    // Auf Antwort vom Sensor warten mit Timeout
    unsigned long startTime = millis();
    while (_serial.available() < 9) {
        if (millis() - startTime > timeout) {
            return XKC_TIMEOUT;
        }
        yield(); // Für ESP8266/ESP32, um WDT-Resets zu vermeiden
    }
    
    // Antwort lesen und prüfen
    uint8_t response[9];
    _serial.readBytes(response, 9);
    
    // Prüfen, ob die Antwort zum gesendeten Befehl passt
    if (response[0] == 0x62 && response[1] == expectedCmd) {
        uint8_t checksum = response[8];
        uint8_t calcChecksum = calculateChecksum(response, 8);
        
        if (checksum == calcChecksum) {
            // Prüfen, ob die Antwort eine erfolgreiche Ausführung anzeigt (0x66)
            if (response[7] == 0x66) {
                return XKC_SUCCESS;
            }
        } else {
            return XKC_CHECKSUM_ERROR;
        }
    }
    
    return XKC_RESPONSE_ERROR;
}

