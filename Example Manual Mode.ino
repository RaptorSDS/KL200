#include <XKC_KL200.h>

XKC_KL200 sensor(Serial);

void setup() {
    Serial.begin(9600);
    sensor.begin();
    Serial.println("Manual Mode Example");

     if (sensor.setCommunicationMode(0)) {
        Serial.println("UART mode set successfully.");
    } else {
        Serial.println("Failed to set UART mode.");
    }
    // Disable automatic upload mode
    if (sensor.setUploadMode(false)) {
        Serial.println("Manual upload mode set successfully.");
    } else {
        Serial.println("Failed to set manual upload mode.");
    }

    // Set LED mode to "light on when there is induction"
    if (sensor.setLEDMode(0)) {
        Serial.println("LED mode set successfully.");
    } else {
        Serial.println("Failed to set LED mode.");
    }
    
}

void loop() {
    uint16_t distance = sensor.readDistance(); //send request and get responce in mm
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" mm");
    delay(500);
}
