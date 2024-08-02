#include <XKC_KL200.h>

XKC_KL200 sensor(Serial);

void setup() {
    Serial.begin(9600);
    sensor.begin();
    Serial.println("Relay and LED Mode Example");

    // Set communication mode to Relay
    if (sensor.setCommunicationMode(1)) {
        Serial.println("Relay mode set successfully.");
    } else {
        Serial.println("Failed to set Relay mode.");
    }

    // Set relay mode to "start when there is induction"
    if (sensor.setRelayMode(0)) {
        Serial.println("Relay mode set successfully.");
    } else {
        Serial.println("Failed to set relay mode.");
    }

    // Set LED mode to "light always on"
    if (sensor.setLEDMode(1)) {
        Serial.println("LED mode set successfully.");
    } else {
        Serial.println("Failed to set LED mode.");
    }
}

void loop() {
    // No distance data is available in Relay mode.
}
