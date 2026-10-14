# XKC_KL200 Arduino Library

This library provides an interface to the XKC KL200 laser distance sensor (XKC-KL200-2M-UART). It allows for easy communication and control of the sensor via an Arduino board.

Developed by Tobias Baumann aka RaptorSDS based on the XKC-KL200 PDF version 10 from 2023 (see folder `from XKC`). This project is not affiliated with XKC.

Old versions of the library are kept in the folder `Archiv` (outside of `src`, so they are not compiled).

## Wiring

| Wire   | Function                                                    |
|--------|-------------------------------------------------------------|
| Brown  | VCC 5-24 V                                                  |
| Blue   | GND                                                         |
| Yellow | Sensor TX / switch output -> board RX (approx. 1k pull-up to MCU VCC) |
| Black  | Sensor RX <- board TX                                       |

Default serial settings: 9600 baud, 8N1.

## Protocol

Every frame has 9 bytes:

| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|---|---|---|---|---|---|---|---|---|
| 0x62 (0x61 for line/trailing mode) | command | length 0x09 | address high | address low | data high | data low | answer (0x66 = OK) | XOR of bytes 0-7 |

A distance frame looks like `62 33 09 00 00 DH DL 00 XOR`, distance in mm = `DH << 8 | DL`.

The library synchronises on the header and length byte, checks the checksum and recovers automatically if bytes are lost or garbage is received.

## Modes

**UART mode** – the sensor communicates via the serial interface.

* Manual mode: the sensor sends data only when requested (`readDistance()`).
* Automatic mode: the sensor continuously sends data at a set interval (`available()` / `getDistance()`).

**Relay mode** – the yellow wire works as a simple switch output, no distance data.

It is not possible to use UART and relay at the same time, because both use the TX wire of the sensor.

## Installation

1. Download the library as a ZIP file from the GitHub repository.
2. Arduino IDE: `Sketch` -> `Include Library` -> `Add .ZIP Library...`.
3. Examples are found under `File` -> `Examples` -> `XKC_KL200`.

## Usage

```cpp
#include <XKC_KL200.h>

XKC_KL200 sensor(Serial1);          // HardwareSerial, begin() sets the baud rate
// SoftwareSerial s(10, 11); XKC_KL200 sensor(s);   // Uno/Nano: call s.begin(9600) yourself

void setup() {
    Serial.begin(115200);
    sensor.begin(9600);
    sensor.setDistanceRange(1, 2200);   // drop implausible values
    sensor.setCommunicationMode(1);     // UART
    sensor.setUploadMode(false);        // manual
}

void loop() {
    uint16_t mm = sensor.readDistance();
    if (mm > 0) Serial.println(mm);
    else        Serial.println(sensor.getLastError());
    delay(200);
}
```

Do not use the same `Serial` for the sensor and debug output.

## Return values

All configuration functions return a `uint8_t` error code. **`XKC_SUCCESS` is 0**, so check with `== XKC_SUCCESS`, not with `if (sensor.setX())`.

| Code | Name                  | Meaning                         |
|------|-----------------------|---------------------------------|
| 0    | XKC_SUCCESS           | OK, sensor answered with 0x66   |
| 1    | XKC_INVALID_PARAMETER | parameter out of range          |
| 2    | XKC_TIMEOUT           | no answer                       |
| 3    | XKC_CHECKSUM_ERROR    | only frames with wrong checksum |
| 4    | XKC_RESPONSE_ERROR    | answer without 0x66             |
| 5    | XKC_OUT_OF_RANGE      | distance outside `setDistanceRange()` |

## Functions

| Function | Description |
|----------|-------------|
| `XKC_KL200(HardwareSerial&, address = 0xFFFF)` | constructor, 0xFFFF = broadcast |
| `XKC_KL200(Stream&, address = 0xFFFF)` | constructor for SoftwareSerial etc. |
| `begin(baud = 9600)` | start communication |
| `setCommunicationMode(mode)` | 0 = relay mode, 1 = UART mode |
| `setUploadMode(bool)` | true = automatic upload, false = manual query |
| `setUploadInterval(1..100)` | interval in 100 ms steps (100 ms .. 10 s) |
| `setLEDMode(mode)` | 0 = on when detecting, 1 = off when detecting, 2 = always off, 3 = always on |
| `setRelayMode(mode)` | 0 = active when detecting, 1 = inactive when detecting |
| `setTrailingMode(mode)` | 0 = high precision, 1 = normal, 2 = high stability |
| `changeBaudRate(0..9)` | 0=2400 1=4800 2=9600 3=14400 4=19200 5=38400 6=56000 7=57600 8=115200 9=128000; HardwareSerial is switched automatically |
| `changeAddress(0..65534)` | change sensor address |
| `hardReset()` | restore XKC factory default (FE) |
| `softReset()` | restore user default (FD) |
| `setDistanceRange(min, max)` | valid distance range in mm, default 1..65535 |
| `readDistance(timeout = 1000)` | manual mode: query distance, returns mm or 0 on error |
| `available()` | non-blocking, processes received bytes, true if a new distance is waiting |
| `getDistance()` | returns the distance and clears `available()` |
| `getLastReceivedDistance()` | last valid distance |
| `getLastRawValue()` | raw value of the last distance frame (also out of range) |
| `getLastError()` | error code of the last operation |
| `getChecksumErrorCount()` | number of dropped frames (diagnostics) |
