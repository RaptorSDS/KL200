/*
 * XKC-KL200 an Arduino Uno/Nano mit SoftwareSerial
 * Pin 10 = RX (gelber Draht, Sensor-TX), Pin 11 = TX (schwarzer Draht, Sensor-RX)
 */
#include <SoftwareSerial.h>
#include <XKC_KL200.h>

SoftwareSerial sensorSerial(10, 11);
XKC_KL200 sensor(sensorSerial);

void setup() {
    Serial.begin(115200);
    sensorSerial.begin(9600);   // bei Stream-Objekten selbst starten
    sensor.begin();

    sensor.setDistanceRange(1, 2200);
    sensor.setCommunicationMode(1);
    sensor.setUploadMode(false);
}

void loop() {
    uint16_t distance = sensor.readDistance();
    if (distance > 0) {
        Serial.print("Distanz: ");
        Serial.print(distance);
        Serial.println(" mm");
    }
    delay(200);
}
