
# HostelPulse - Wi-Fi Connectivity Test

## Objective

Verify that the ESP8266 mess node can connect to a wireless network and maintain connectivity.

## Test Environment

- Microcontroller: NodeMCU ESP8266 (ESP-12E)
- Development environment: Arduino IDE 2.3.10
- Network: Android mobile hotspot
- Communication: 2.4 GHz Wi-Fi
- Hardware connections: USB only, no sensors connected

## Test Results

| Parameter | Observed Result |
|---|---|
| Wi-Fi connection | Successful |
| IP address | 10.231.36.170 |
| RSSI reading 1 | -67 dBm |
| RSSI reading 2 | -66 dBm |
| RSSI reading 3 | -68 dBm |
| Connection stability | Connected during repeated checks |

## Observations

The ESP8266 successfully connected to the Android mobile hotspot and obtained an IP address.

RSSI readings fluctuated slightly between -66 dBm and -68 dBm. Such variations are normal in wireless communication.

## Limitations

- Testing was performed near the development location.
- Wi-Fi availability and signal strength at the actual mess entrance remain unverified.
- MQTT communication has not yet been implemented.

## Next Steps

- Install and configure a local MQTT broker.
- Establish communication between the ESP8266 and laptop.
- Test telemetry publishing and subscription.
- Repeat Wi-Fi testing at the actual mess entrance.
