/*
 * XKC-KL200 Relais-Modus
 * Der gelbe Draht arbeitet danach als Schaltausgang, es gibt keine Distanzdaten mehr.
 * Zurück in den UART-Modus mit setCommunicationMode(1).
 */
#include <XKC_KL200.h>

#define SENSOR_SERIAL Serial1
#define SENSOR_OUTPUT_PIN 7   // gelber Draht nach dem Umschalten (mit Pull-Up)

XKC_KL200 sensor(SENSOR_SERIAL);

void setup() {
    Serial.begin(115200);
    Serial.println("XKC-KL200 Relay and LED Mode Example");

    sensor.begin(9600);

    // Erst Relais- und LED-Verhalten setzen, solange UART noch antwortet
    if (sensor.setRelayMode(0) == XKC_SUCCESS) {          // aktiv bei Erkennung
        Serial.println("Relais-Modus gesetzt");
    }
    if (sensor.setLEDMode(0) == XKC_SUCCESS) {            // LED an bei Erkennung
        Serial.println("LED-Modus gesetzt");
    }
    if (sensor.setCommunicationMode(0) == XKC_SUCCESS) {  // Relais-Modus
        Serial.println("Line mode: Relais");
    }

    pinMode(SENSOR_OUTPUT_PIN, INPUT_PULLUP);
}

void loop() {
    Serial.println(digitalRead(SENSOR_OUTPUT_PIN) ? "frei" : "Objekt erkannt");
    delay(500);
}
