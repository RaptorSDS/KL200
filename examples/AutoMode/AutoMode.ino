/*
 * XKC-KL200 Auto-Modus
 * Der Sensor sendet selbstständig alle 100ms..10s einen Distanz-Frame.
 *
 * Verdrahtung: Braun=VCC (5-24V), Blau=GND, Gelb=Sensor-TX -> RX vom Board
 *              (ca. 1k Pull-Up gegen MCU-VCC), Schwarz=Sensor-RX <- TX vom Board
 *
 * Board mit zweiter Hardware-Serial nötig (Mega, ESP32, Leonardo, ...).
 * Für Arduino Uno/Nano siehe Beispiel "SoftwareSerial".
 */
#include <XKC_KL200.h>

#define SENSOR_SERIAL Serial1

XKC_KL200 sensor(SENSOR_SERIAL);

void printResult(const char* text, uint8_t result) {
    Serial.print(text);
    if (result == XKC_SUCCESS) {
        Serial.println(": OK");
    } else {
        Serial.print(": Fehler ");
        Serial.println(result);
    }
}

void setup() {
    Serial.begin(115200);              // Debug-Ausgabe
    Serial.println("XKC-KL200 Auto Mode Example");

    sensor.begin(9600);                // Standard-Baudrate des Sensors
    sensor.setDistanceRange(1, 2200);  // unplausible Werte verwerfen (Messbereich 10-2000mm)

    printResult("UART-Modus", sensor.setCommunicationMode(1));
    printResult("Intervall 500ms", sensor.setUploadInterval(5));
    printResult("LED immer an", sensor.setLEDMode(3));
    printResult("Auto-Upload", sensor.setUploadMode(true));
}

void loop() {
    // available() liest die empfangenen Bytes, nicht blockierend
    if (sensor.available()) {
        uint16_t distance = sensor.getDistance();
        Serial.print("Distanz: ");
        Serial.print(distance);
        Serial.print(" mm  (raw 0x");
        Serial.print(sensor.getLastRawValue(), HEX);
        Serial.println(")");
    }

    // Andere Aufgaben können hier parallel ausgeführt werden
}
