# XKC_KL200 Arduino Library

This library provides an interface to the XKC KL200 laser distance sensor. It allows for easy communication and control of the sensor via an Arduino board.

Developed by Tobias Baumann aka RaptorSDS and ChatGPT based on the XKC-KL-200 PDF version 10 from 2023. This project is not affiliated with XKC. No error correction has been implemented, so this project is in version 0.5 Beta.

## Features

- Change sensor settings such as baud rate, LED mode, and relay mode.
- Switch between manual and automatic data upload modes.
- Read distance measurements from the sensor.
- Implement safety checks for settings (e.g., baud rate, LED mode).

## Installation

1. Download the library as a ZIP file from the GitHub repository.
2. Open the Arduino IDE.
3. Go to `Sketch` -> `Include Library` -> `Add .ZIP Library...`.
4. Select the downloaded ZIP file.
5. The library is now installed and ready to use.

## Usage

### Include the Library


#include <XKC_KL200.h>




Initialize the Sensor

Create an instance of the XKC_KL200 class, passing the hardware serial interface to the constructor:

XKC_KL200 sensor(Serial);

In the setup() function, initialize the sensor with the desired baud rate:




void setup() {
    Serial.begin(9600);
    sensor.begin();
}

Functions
begin()

Initializes the sensor.





setUploadMode(bool mode)

Sets the upload mode of the sensor.

    true for automatic upload mode
    false for manual upload mode

Returns true if the setting was successful, false otherwise.





setLEDMode(uint8_t mode)

Sets the LED mode of the sensor.

    0: Light on when there is induction
    1: Light always on
    2: Light always off
    3: Flashing

Returns true if the setting was successful, false otherwise.





changeBaudRate(uint8_t rate)

Changes the baud rate of the sensor.

    0: 1200
    1: 2400
    2: 4800
    3: 9600
    4: 14400
    5: 19200
    6: 38400
    7: 57600
    8: 115200
    9: 230400

Returns true if the setting was successful, false otherwise.




setUploadInterval(uint8_t interval)

Sets the upload interval for the sensor in automatic mode (not applicable in manual mode).

    Interval in 100ms units (e.g., 10 for 1 second)

Returns true if the setting was successful, false otherwise.





setRelayMode(uint8_t mode)

Sets the relay mode of the sensor.

    0: Start when there is induction
    1: Stop when there is induction

Returns true if the setting was successful, false otherwise.




readDistance()

Reads the distance measurement from the sensor in manual mode.

Returns the distance in millimeters.





available()

Checks if there is data available from the sensor in automatic mode.

Returns true if data is available, false otherwise.




getDistance()

Gets the distance measurement from the sensor in automatic mode.

Returns the distance in millimeters.




getLastReceivedDistance()

Gets the last received distance measurement.

Returns the distance in millimeters.

