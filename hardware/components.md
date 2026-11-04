
# HostelPulse - Hardware Components

## 1. Project Overview

HostelPulse is an IoT-based hostel monitoring system. Its mess node uses infrared proximity sensors to detect the direction of movement through an entrance and estimate the number of people inside.

The hardware node processes sensor readings locally using an ESP8266 microcontroller.

## 2. Hardware Components

| Component | Quantity | Purpose | Status |
|---|---:|---|---|
| NodeMCU ESP8266 (ESP-12E) | 1 | Main microcontroller | Tested |
| HW-201 IR sensor modules | 2 | Initial sensor testing | Basic testing completed |
| E18-D80NK IR sensors | 2 | Improved proximity detection | Arrived, testing pending |
| Breadboard | 1 | Circuit prototyping | Available |
| Jumper wires | As available | Electrical connections | Available |
| Red, yellow, and green LEDs | As available | Local Busy, Moderate, and Low indicator | Planned |
| Current-limiting resistors | One per LED | Limit LED current from ESP8266 GPIO pins | Planned |
| USB cable | 1 | Programming and power | Tested |

## 3. Microcontroller

**Board:** NodeMCU ESP8266 (ESP-12E)

The ESP8266 is responsible for:

- Reading the outputs of the two IR sensors.
- Detecting the order in which sensors are triggered.
- Determining whether a person is entering or exiting.
- Maintaining the current occupancy count.
- Publishing telemetry through Wi-Fi and MQTT (a standalone ESP8266 broker test has passed; sensor-counter-to-backend integration is still unverified).

The board uses the CH340 USB-to-serial interface for communication with the computer.

## 4. Infrared Sensors

### 4.1 HW-201 IR Sensor Modules

Two HW-201-style reflective IR sensor modules were used during the initial prototype.

Their outputs were connected to:

- Sensor A: D5
- Sensor B: D6

Basic sequential detection and entry/exit counting were successfully tested.

### 4.2 E18-D80NK Sensors

Two E18-D80NK proximity sensors have been acquired for the next hardware iteration.

These sensors will replace the initial modules after their power supply, output characteristics, detection range, and ESP8266 compatibility have been verified.

**Current status:** Not yet wired or tested.

## 5. Initial Sensor Pin Mapping

The initial HW-201 prototype uses the following connections:

| Sensor | ESP8266 connection |
|---|---|
| Sensor A VCC | 3V3 |
| Sensor A GND | GND |
| Sensor A OUT | D5 |
| Sensor B VCC | 3V3 |
| Sensor B GND | GND |
| Sensor B OUT | D6 |

**Note:** This pin mapping describes the initial HW-201 prototype only. It must not be assumed to apply to the E18-D80NK sensors.

## 6. Hardware Development Status

- [x] ESP8266 board setup and USB connection.
- [x] Initial HW-201 sensor testing.
- [x] Basic two-sensor entry/exit counting.
- [x] Initial occupancy calculation.
- [ ] E18-D80NK electrical verification.
- [ ] E18-D80NK integration.
- [ ] Final doorway sensor placement.
- [x] ESP8266 Wi-Fi connectivity test.
- [x] Standalone ESP8266-to-Mosquitto MQTT publishing test.
- [ ] Sensor-counter telemetry integrated with the backend.
- [x] MQTT crowd-level LED receiver sketch added; physical LED wiring/demo remains to be verified.

## 7. Safety Considerations

The ESP8266 GPIO pins operate at 3.3V logic levels and are not 5V tolerant.

The E18-D80NK sensor output must be verified before connecting it to an ESP8266 GPIO pin. Its power supply, output type, and voltage compatibility must be checked first.

Do not connect an unverified 5V output directly to the microcontroller.

## 8. Crowd-Level LED Indicator

The separate `firmware/mess-node/mqtt_led_indicator/mqtt_led_indicator.ino` sketch subscribes to the backend's MQTT crowd-level topic. It does not read the sensors. For a demo driven by generated telemetry, the backend calculates the level and publishes `LOW`, `MODERATE`, or `BUSY`; the ESP8266 lights green, yellow, or red respectively.

The sketch defaults to D1 for green, D2 for yellow, and D7 for red. Connect each GPIO through its own current-limiting resistor to the LED anode, and connect each LED cathode to GND. These are assumed connections; confirm the actual wiring before powering the LEDs. The laptop broker must be reachable from the ESP8266 over the shared hotspot.
