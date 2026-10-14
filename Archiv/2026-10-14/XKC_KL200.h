#ifndef XKC_KL200_H
#define XKC_KL200_H

#include "Arduino.h"

/**
 * @brief Fehlercodes für die XKC_KL200 Bibliothek
 */
enum XKC_KL200_Error {
    XKC_SUCCESS = 0,           // Operation erfolgreich
    XKC_INVALID_PARAMETER = 1, // Ungültiger Parameter
    XKC_TIMEOUT = 2,           // Timeout bei der Kommunikation
    XKC_CHECKSUM_ERROR = 3,    // Prüfsummenfehler
    XKC_RESPONSE_ERROR = 4     // Ungültige Antwort vom Sensor
};

class XKC_KL200 {
public:
    /**
     * @brief Konstruktor für den XKC_KL200 Sensor
     * @param serial Hardware-Serial-Objekt für die Kommunikation
     */
    XKC_KL200(HardwareSerial& serial);

    /**
     * @brief Initialisiert die Kommunikation mit dem Sensor
     * @param baudRate Baudrate für die serielle Kommunikation (Standard: 9600)
     */
    void begin(long baudRate = 9600);

    /**
     * @brief Setzt den Sensor auf Werkseinstellungen zurück
     * @return XKC_SUCCESS bei Erfolg, sonst Fehlercode
     */
    uint8_t hardReset();

    /**
     * @brief Setzt den Sensor auf Benutzereinstellungen zurück
     * @return XKC_SUCCESS bei Erfolg, sonst Fehlercode
     */
    uint8_t softReset();

    /**
     * @brief Ändert die Adresse des Sensors
     * @param address Neue Adresse (0-65534)
     * @return XKC_SUCCESS bei Erfolg, sonst Fehlercode
     */
    uint8_t changeAddress(uint16_t address);

    /**
     * @brief Ändert die Baudrate des Sensors
     * @param rate Baudrate-Index (0-9, siehe Dokumentation)
     * @return XKC_SUCCESS bei Erfolg, sonst Fehlercode
     */
    uint8_t changeBaudRate(uint8_t rate);

    /**
     * @brief Setzt den Upload-Modus des Sensors
     * @param autoUpload true für automatischen Upload, false für manuellen Abfragemodus
     * @return XKC_SUCCESS bei Erfolg, sonst Fehlercode
     */
    uint8_t setUploadMode(bool autoUpload);

    /**
     * @brief Setzt das Upload-Intervall im automatischen Modus
     * @param interval Intervall (1-100, entspricht 100ms-10s)
     * @return XKC_SUCCESS bei Erfolg, sonst Fehlercode
     */
    uint8_t setUploadInterval(uint8_t interval);

    /**
     * @brief Konfiguriert das Verhalten der LED
     * @param mode 0=an bei Erkennung, 1=aus bei Erkennung, 2=immer aus, 3=immer an
     * @return XKC_SUCCESS bei Erfolg, sonst Fehlercode
     */
    uint8_t setLEDMode(uint8_t mode);

    /**
     * @brief Konfiguriert das Verhalten des Relais-Ausgangs
     * @param mode 0=aktiv bei Erkennung, 1=inaktiv bei Erkennung
     * @return XKC_SUCCESS bei Erfolg, sonst Fehlercode
     */
    uint8_t setRelayMode(uint8_t mode);

    /**
     * @brief Setzt den Kommunikationsmodus des Sensors
     * @param mode 0=Relais-Modus, 1=UART-Modus
     * @return XKC_SUCCESS bei Erfolg, sonst Fehlercode
     */
    uint8_t setCommunicationMode(uint8_t mode);

    /**
     * @brief Liest die aktuelle Distanz vom Sensor im manuellen Modus
     * @param timeout Timeout in Millisekunden (Standard: 1000ms)
     * @return Gemessene Distanz in mm oder 0 bei Fehler
     */
    uint16_t readDistance(unsigned long timeout = 1000);

    /**
     * @brief Prüft, ob neue Daten im automatischen Modus verfügbar sind
     * @return true wenn neue Daten verfügbar sind, sonst false
     */
    bool available();

    /**
     * @brief Verarbeitet eingehende Daten im automatischen Modus
     * @return true wenn neue Daten erfolgreich verarbeitet wurden, sonst false
     */
    bool processAutoData();

    /**
     * @brief Gibt die zuletzt gemessene Distanz zurück
     * @return Zuletzt gemessene Distanz in mm
     */
    uint16_t getDistance();

    /**
     * @brief Gibt die zuletzt empfangene Distanz zurück (ohne _available zurückzusetzen)
     * @return Zuletzt empfangene Distanz in mm
     */
    uint16_t getLastReceivedDistance();

private:
    HardwareSerial& _serial;
    bool _available;
    uint16_t _distance;
    uint16_t _lastReceivedDistance;
    bool _autoMode;

    /**
     * @brief Sendet einen Befehl an den Sensor
     * @param command Befehlsarray
     * @param length Länge des Befehls
     */
    void sendCommand(const uint8_t* command, uint8_t length);

    /**
     * @brief Berechnet die XOR-Prüfsumme für einen Befehl
     * @param data Datenarray
     * @param length Länge der Daten
     * @return Berechnete Prüfsumme
     */
    uint8_t calculateChecksum(const uint8_t* data, uint8_t length);

    /**
     * @brief Wartet auf eine Antwort vom Sensor und prüft diese
     * @param expectedCmd Erwarteter Befehlscode in der Antwort
     * @param timeout Timeout in Millisekunden
     * @return XKC_SUCCESS bei Erfolg, sonst Fehlercode
     */
    uint8_t waitForResponse(uint8_t expectedCmd, unsigned long timeout);
};

#endif
