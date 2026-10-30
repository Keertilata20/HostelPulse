
# MQTT Broker Setup and Connectivity

## 1. Overview

HostelPulse uses MQTT (Message Queuing Telemetry Transport) for communication between the ESP8266 mess node and the backend.

Mosquitto is used as the local MQTT broker to support offline-first operation.

## 2. Technology and Environment

| Component | Configuration |
|---|---|
| Microcontroller | NodeMCU ESP8266 |
| MQTT Broker | Eclipse Mosquitto |
| Mosquitto Version | 2.1.2 |
| MQTT Port | 1883 |
| Network | Android mobile hotspot |
| Laptop OS | Windows |
| Protocol | MQTT |

## 3. Network Configuration

The laptop and ESP8266 are connected to the same Android hotspot.

Tested network addresses:

| Device | IPv4 Address |
|---|---|
| Laptop | 10.231.36.131 |
| ESP8266 | 10.231.36.170 |

Subnet mask: 255.255.255.0

Note: These are test-session addresses. DHCP may assign different addresses in future sessions.

## 4. Mosquitto Installation

Mosquitto 2.1.2 was installed on Windows.

Installation directory:

`C:\Program Files\mosquitto`

Installation was verified using:

```powershell
& "C:\Program Files\mosquitto\mosquitto.exe" -h
```

## 5. Custom Broker Configuration

A separate configuration was created for HostelPulse:

`backend/mqtt/hostelpulse-mosquitto.conf`

Configuration:

```conf
listener 1883 10.231.36.131
allow_anonymous true

persistence true
persistence_location C:/Users/Public/hostelpulse-mqtt/
```

The persistence directory was created manually.

Configuration validation was successful using:

```powershell
& "C:\Program Files\mosquitto\mosquitto.exe" --test-config -c "$HOME\Desktop\HostelPulse\backend\mqtt\hostelpulse-mosquitto.conf"
```

## 6. Broker Execution

The default Mosquitto Windows service was stopped to release port 1883.

The HostelPulse broker was started manually using:

```powershell
& "C:\Program Files\mosquitto\mosquitto.exe" -c "$HOME\Desktop\HostelPulse\backend\mqtt\hostelpulse-mosquitto.conf" -v
```

Successful startup was confirmed by:

```text
Opening ipv4 listen socket on port 1883.
mosquitto version 2.1.2 running
```

The broker currently runs in a foreground PowerShell window.

## 7. MQTT Publish/Subscribe Testing

A test topic was used:

`hostelpulse/test`

### Subscriber

```powershell
& "C:\Program Files\mosquitto\mosquitto_sub.exe" -h 10.231.36.131 -p 1883 -t "hostelpulse/test" -v
```

### Publisher

```powershell
& "C:\Program Files\mosquitto\mosquitto_pub.exe" -h 10.231.36.131 -p 1883 -t "hostelpulse/test" -m "HostelPulse hotspot MQTT is working!"
```

### Received Message

```text
hostelpulse/test HostelPulse hotspot MQTT is working!
```

The successful message confirms that the broker accepts MQTT connections through the laptop's hotspot network interface.

## 8. Network Connectivity Verification

The laptop successfully pinged the ESP8266:

```powershell
ping 10.231.36.170
```

Three direct replies were received, with response times of 2290 ms, 5 ms, and 6 ms.

This confirms basic IP-level connectivity during the test session.

## 9. Current Implementation Status

| Task | Status |
|---|---|
| Mosquitto installation | Completed |
| Broker configuration | Completed |
| Configuration validation | Completed |
| Local MQTT publish/subscribe | Completed |
| Hotspot network listener | Completed |
| Laptop-to-ESP8266 ping | Successful |
| ESP8266 Wi-Fi connectivity | Tested successfully |
| ESP8266 MQTT publishing | Pending |
| MQTT integration with firmware | Pending |
| Python MQTT subscriber | Pending |
| PostgreSQL integration | Pending |

## 10. Next Development Steps

1. Install the PubSubClient library in Arduino IDE.
2. Connect the ESP8266 to the Mosquitto broker.
3. Publish MQTT test messages from the ESP8266.
4. Integrate MQTT publishing with the mess people-counter firmware.
5. Implement the Python MQTT subscriber.
6. Store telemetry data in PostgreSQL.
7. Connect the backend to the React dashboard.

## 11. Security and Deployment Notes

- Anonymous MQTT access is currently enabled only for a controlled local hotspot demonstration.
- Do not expose port 1883 to public networks or the internet.
- Add authentication and appropriate access controls before wider deployment.
- The broker must be running on the laptop for local MQTT communication.
- The laptop and ESP8266 must be connected to the same local network.
- The current broker is manually started and is not yet configured as a persistent Windows service.
