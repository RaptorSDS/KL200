/*
 * XKC-KL200 manueller Modus
 * Die Distanz wird per Befehl 62 33 abgefragt.
 *
 * Board mit zweiter Hardware-Serial nötig (Mega, ESP32, Leonardo, ...).
 * Für Arduino Uno/Nano siehe Beispiel "SoftwareSerial".
 */
#include <XKC_KL200.h>

#define SENSOR_SERIAL Serial1

XKC_KL200 sensor(SENSOR_SERIAL);

void setup() {
    Serial.begin(115200);
    Serial.println("XKC-KL200 Manual Mode Example");

    sensor.begin(9600);

    if (sensor.setCommunicationMode(1) != XKC_SUCCESS) {
        Serial.println("UART-Modus konnte nicht gesetzt werden");
    }
    if (sensor.setUploadMode(false) != XKC_SUCCESS) {
        Serial.println("Manueller Modus konnte nicht gesetzt werden");
    }
    if (sensor.setLEDMode(0) != XKC_SUCCESS) {
        Serial.println("LED-Modus konnte nicht gesetzt werden");
    }
}

void loop() {
    uint16_t distance = sensor.readDistance(); // Abfrage senden und Antwort lesen

    if (distance > 0) {
        Serial.print("Distanz: ");
        Serial.print(distance);
        Serial.println(" mm");
    } else {
        Serial.print("Keine Messung, Fehler ");
        Serial.println(sensor.getLastError());
    }
    delay(500);
}
