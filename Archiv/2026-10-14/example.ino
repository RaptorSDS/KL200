#include "XKC_KL200.h"

// Sensor an Serial1 anschließen
XKC_KL200 sensor(Serial1);

void setup() {
  Serial.begin(115200);  // Für Debug-Ausgaben
  Serial.println("XKC-KL200 Laser-Abstandssensor Test (Auto-Modus)");
  
  sensor.begin(9600);    // Standard-Baudrate des Sensors
  
  // Konfiguration des Sensors
  uint8_t result = sensor.setCommunicationMode(1);  // UART-Modus
  if (result != XKC_SUCCESS) {
    Serial.print("Fehler beim Setzen des Kommunikationsmodus: ");
    Serial.println(result);
  }
  
  result = sensor.setUploadMode(true);  // Automatischer Upload-Modus
  if (result != XKC_SUCCESS) {
    Serial.print("Fehler beim Setzen des Upload-Modus: ");
    Serial.println(result);
  }
  
  result = sensor.setUploadInterval(5);  // Intervall: 500ms
  if (result != XKC_SUCCESS) {
    Serial.print("Fehler beim Setzen des Upload-Intervalls: ");
    Serial.println(result);
  }
  
  Serial.println("Sensor konfiguriert. Empfange automatische Messungen...");
}

void loop() {
  // Prüfen, ob neue Daten verfügbar sind
  if (sensor.available()) {
    // Daten verarbeiten
    if (sensor.processAutoData()) {
      // Distanz auslesen und ausgeben
      uint16_t distance = sensor.getDistance();
      Serial.print("Distanz: ");
      Serial.print(distance);
      Serial.println(" mm");
    }
  }
  
  // Andere Aufgaben können hier parallel ausgeführt werden
  // ...
}
