# XKC_KL200 Arduino Library

This library provides an interface to the XKC KL200 laser distance sensor. It allows for easy communication and control of the sensor via an Arduino board.

Developed by Tobias Baumann aka RaptorSDS and ChatGPT based on the XKC-KL-200 PDF version 10 from 2023. This project is not affiliated with XKC. No error correction has been implemented, so this project is in version 0.5 Beta.


## Modes
UART Mode

In UART mode, the sensor communicates via the serial interface. This mode supports both manual and automatic data upload modes.

    Manual Mode: The sensor sends data only when requested.
    Automatic Mode: The sensor continuously sends data at a set interval.

Relay Mode

In Relay mode, the sensor operates as a simple switch and does not send distance data. Instead, it activates or deactivates a relay based on the detection conditions.
Relay Output Modes

The relay can be configured to operate in different modes:

    Mode 0: Relay activates when there is induction.
    Mode 1: Relay deactivates when there is induction.


### its not possible to use both uart and relay same time because both use TX of Sensor
    

## Features

- Change sensor settings such as baud rate, LED mode, and relay mode.
- Switch between manual and automatic data upload modes. (Only UART)
- Read distance measurements from the sensor. (only UART)
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




###Initialize the Sensor

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




### upload mode
setUploadMode(bool mode)

Sets the upload mode of the sensor.

    true for automatic upload mode
    false for manual upload mode

Returns true if the setting was successful, false otherwise.




### Led mode
setLEDMode(uint8_t mode)

Sets the LED mode of the sensor.

    0: Light on when there is induction
    1: Light always on
    2: Light always off
    3: Flashing

Returns true if the setting was successful, false otherwise.




### Baud
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



### Intervall
setUploadInterval(uint8_t interval)

Sets the upload interval for the sensor in automatic mode (not applicable in manual mode).

    Interval in 100ms units (e.g., 10 for 1 second)

Returns true if the setting was successful, false otherwise.




### Relay output
setRelayMode(uint8_t mode)

Sets the relay mode of the sensor.

    0: Start when there is induction
    1: Stop when there is induction

Returns true if the setting was successful, false otherwise.



### Distance
readDistance()

Reads the distance measurement from the sensor in manual mode.

Returns the distance in millimeters.




### availible
available()

Checks if there is data available from the sensor in automatic mode.

Returns true if data is available, false otherwise.


 
### Distance
getDistance()

Gets the distance measurement from the sensor in automatic mode.

Returns the distance in millimeters.



### Distance
getLastReceivedDistance()

Gets the last received distance measurement.

Returns the distance in millimeters.


### Reset
HardReset(uint8_t type)
SoftReset(uint8_t type)

Performs a factory reset of the sensor.

     Soft reset (reset to XKC settings)
     Hard reset (resets to factory settings)

Returns true if the reset was successful, false otherwise.

### Communication mode
setCommunicationMode(uint8_t mode)

Sets the communication mode of the sensor.

    1: UART mode
    0: Relay mode

Returns true if the setting was successful, false otherwise.
