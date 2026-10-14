#include <XKC_KL200.h>

XKC_KL200 sensor(Serial);

void setup() {
    Serial.begin(9600);
    sensor.begin();
    Serial.println("Auto Mode Example");

    // Set communication mode to UART
    if (sensor.setCommunicationMode(1)) {
        Serial.println("UART mode set successfully.");
    } else {
        Serial.println("Failed to set UART mode.");
    }
    
    // Enable automatic upload mode
    if (sensor.setUploadMode(true)) {
        Serial.println("Automatic upload mode set successfully.");
    } else {
        Serial.println("Failed to set automatic upload mode.");
    }
        // Set LED mode to "light on "
    if (sensor.setLEDMode(3)) {
        Serial.println("LED mode set successfully.");
    } else {
        Serial.println("Failed to set LED mode.");
    }

    // Hinweis auf mögliche Probleme beim Konfigurieren im automatischen Modus
    Serial.println("Note: Configuring settings while in automatic mode may cause issues due to overlapping data.");
}

void loop() {
    if (sensor.available()) {
        sensor.readDistance();
        uint16_t lastDistance = sensor.getLastReceivedDistance();
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" mm");
    } else {
        // keine Werte        
        Serial.print("No Distance");
    }
}
