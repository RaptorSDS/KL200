#ifndef XKC_KL200_H
#define XKC_KL200_H

#include "Arduino.h"

/*
 * Protokoll laut Datenblatt XKC-KL200-2M-UART (V10, 28.03.2023):
 * Jeder Frame hat 9 Byte, 9600 8N1 (Standard)
 *
 *   [0] Header   0x62 ('b') bzw. 0x61 ('a') bei Line-/Trailing-Mode
 *   [1] Befehl   z.B. 0x33 = Distanz
 *   [2] Laenge   immer 0x09
 *   [3] Adresse High   (0xFFFF = Broadcast)
 *   [4] Adresse Low
 *   [5] Daten High
 *   [6] Daten Low
 *   [7] Antwort  0x66 = OK (in Antworten), sonst 0x00
 *   [8] XOR ueber Byte 0..7
 */
#define XKC_FRAME_LENGTH      9
#define XKC_HEADER_B          0x62
#define XKC_HEADER_A          0x61
#define XKC_ACK               0x66
#define XKC_BROADCAST_ADDRESS 0xFFFF

#define XKC_CMD_BAUDRATE      0x30   // b0
#define XKC_CMD_LINE_MODE     0x30   // a0 (Header 0x61)
#define XKC_CMD_ADDRESS       0x32   // b2
#define XKC_CMD_TRAILING_MODE 0x32   // a2 (Header 0x61)
#define XKC_CMD_DISTANCE      0x33   // b3
#define XKC_CMD_UPLOAD_MODE   0x34   // b4
#define XKC_CMD_INTERVAL      0x35   // b5
#define XKC_CMD_LED_MODE      0x37   // b7
#define XKC_CMD_RELAY_MODE    0x38   // b8
#define XKC_CMD_RESET         0x39   // b9

#define XKC_DEFAULT_TIMEOUT   1000

/**
 * @brief Fehlercodes für die XKC_KL200 Bibliothek
 */
enum XKC_KL200_Error {
    XKC_SUCCESS = 0,           // Operation erfolgreich
    XKC_INVALID_PARAMETER = 1, // Ungültiger Parameter
    XKC_TIMEOUT = 2,           // Timeout bei der Kommunikation
    XKC_CHECKSUM_ERROR = 3,    // Prüfsummenfehler
    XKC_RESPONSE_ERROR = 4,    // Ungültige Antwort vom Sensor
    XKC_OUT_OF_RANGE = 5       // Messwert ausserhalb des gültigen Bereichs
};

class XKC_KL200 {
public:
    /**
     * @brief Konstruktor für Hardware-Serial (begin() setzt die Baudrate)
     * @param serial  Hardware-Serial-Objekt, z.B. Serial1 / Serial2
     * @param address Sensoradresse, Standard 0xFFFF = Broadcast (jeder Sensor antwortet)
     */
    XKC_KL200(HardwareSerial& serial, uint16_t address = XKC_BROADCAST_ADDRESS);

    /**
     * @brief Konstruktor für beliebige Streams, z.B. SoftwareSerial.
     *        Der Stream muss vorher selbst mit begin(9600) gestartet werden.
     */
    XKC_KL200(Stream& serial, uint16_t address = XKC_BROADCAST_ADDRESS);

    /**
     * @brief Initialisiert die Kommunikation mit dem Sensor
     * @param baudRate Baudrate für die serielle Kommunikation (Standard: 9600)
     */
    void begin(long baudRate = 9600);

    /**
     * @brief Setzt den Sensor auf Werkseinstellungen (Xingkechuang default, FE) zurück
     */
    uint8_t hardReset();

    /**
     * @brief Setzt den Sensor auf Benutzereinstellungen (FD) zurück
     */
    uint8_t softReset();

    /**
     * @brief Ändert die Adresse des Sensors (0-65534). Bei Erfolg wird die neue
     *        Adresse auch von der Bibliothek verwendet.
     */
    uint8_t changeAddress(uint16_t address);

    /**
     * @brief Ändert die Baudrate des Sensors
     * @param rate 0=2400 1=4800 2=9600 3=14400 4=19200 5=38400 6=56000 7=57600 8=115200 9=128000
     *        Bei Hardware-Serial wird die eigene Schnittstelle automatisch umgestellt.
     */
    uint8_t changeBaudRate(uint8_t rate);

    /**
     * @brief Setzt den Upload-Modus des Sensors
     * @param autoUpload true = automatischer Upload, false = manuelle Abfrage
     */
    uint8_t setUploadMode(bool autoUpload);

    /**
     * @brief Setzt das Upload-Intervall im automatischen Modus
     * @param interval 1-100 (entspricht 100ms-10s)
     */
    uint8_t setUploadInterval(uint8_t interval);

    /**
     * @brief Konfiguriert die LED
     * @param mode 0=an bei Erkennung, 1=aus bei Erkennung, 2=immer aus, 3=immer an
     */
    uint8_t setLEDMode(uint8_t mode);

    /**
     * @brief Konfiguriert den Relais-/Schaltausgang
     * @param mode 0=aktiv bei Erkennung, 1=inaktiv bei Erkennung
     */
    uint8_t setRelayMode(uint8_t mode);

    /**
     * @brief Setzt den Kommunikationsmodus (Line mode)
     * @param mode 0=Relais-Modus, 1=UART-Modus
     */
    uint8_t setCommunicationMode(uint8_t mode);

    /**
     * @brief Setzt den Trailing-Modus (Filter / Ansprechverhalten)
     * @param mode 0=hohe Präzision, 1=normal, 2=hohe Stabilität
     */
    uint8_t setTrailingMode(uint8_t mode);

    /**
     * @brief Gültiger Messbereich. Werte ausserhalb werden verworfen.
     *        Standard: 1..65535 mm (0 wird verworfen, wie "keine Messung")
     */
    void setDistanceRange(uint16_t minDistance, uint16_t maxDistance);

    /**
     * @brief Fragt die Distanz im manuellen Modus ab (blockierend bis timeout)
     *        Im Auto-Modus wird stattdessen auf den nächsten Frame gewartet.
     * @return Distanz in mm oder 0 bei Fehler (Details: getLastError())
     */
    uint16_t readDistance(unsigned long timeout = XKC_DEFAULT_TIMEOUT);

    /**
     * @brief Verarbeitet empfangene Bytes (nicht blockierend) und meldet,
     *        ob eine neue, noch nicht abgeholte Distanz vorliegt.
     */
    bool available();

    /**
     * @brief Wie available(), aus Kompatibilitätsgründen erhalten
     * @return true wenn eine neue Distanz vorliegt
     */
    bool processAutoData();

    /**
     * @brief Gibt die zuletzt gemessene Distanz zurück und setzt available() zurück
     */
    uint16_t getDistance();

    /**
     * @brief Gibt die zuletzt empfangene Distanz zurück (ohne available() zurückzusetzen)
     */
    uint16_t getLastReceivedDistance();

    /**
     * @brief Rohdaten (Daten High/Low) des letzten Distanz-Frames, z.B. für Hex-Ausgabe
     */
    uint16_t getLastRawValue();

    /**
     * @brief Fehlercode der letzten Operation (XKC_KL200_Error)
     */
    uint8_t getLastError();

    /**
     * @brief Anzahl verworfener Frames mit falscher Prüfsumme (Diagnose)
     */
    uint16_t getChecksumErrorCount();

    /**
     * @brief Berechnet die XOR-Prüfsumme
     */
    static uint8_t calculateChecksum(const uint8_t* data, uint8_t length);

private:
    Stream& _serial;
    HardwareSerial* _hwSerial;     // nur gesetzt, wenn Baudrate von uns gesteuert werden kann
    uint16_t _address;
    bool _available;
    bool _autoMode;
    uint16_t _distance;
    uint16_t _lastReceivedDistance;
    uint16_t _lastRawValue;
    uint16_t _minDistance;
    uint16_t _maxDistance;
    uint8_t _lastError;
    uint16_t _checksumErrors;

    uint8_t _rxBuffer[XKC_FRAME_LENGTH];
    uint8_t _rxIndex;

    void sendCommand(uint8_t header, uint8_t command, uint16_t data, uint8_t answer = 0x00);
    uint8_t sendAndWait(uint8_t header, uint8_t command, uint16_t data, uint8_t answer = 0x00);
    void clearInput();

    /**
     * @brief Liest verfügbare Bytes und synchronisiert auf den Frame-Anfang.
     *        Nicht blockierend; gibt true zurück, sobald ein gültiger Frame in _rxBuffer liegt.
     */
    bool pollFrame();

    /**
     * @brief Verwirft den Pufferanfang bis zum nächsten möglichen Header
     */
    void resync();

    /**
     * @brief Wartet bis zu timeout ms auf einen Frame mit dem gewünschten Befehl.
     *        Dazwischen eintreffende Distanz-Frames werden gespeichert.
     */
    uint8_t waitForFrame(uint8_t expectedCmd, unsigned long timeout);

    /**
     * @brief Wertet einen Distanz-Frame aus
     * @return true wenn die Distanz gültig war
     */
    bool handleDistanceFrame(const uint8_t* frame);
};

#endif
