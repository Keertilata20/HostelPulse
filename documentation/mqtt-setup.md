
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

The laptop and ESP8266 should be connected to the same local network. Broker addresses are assigned by the network and can change, so configure the current address locally instead of recording it in this guide.

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

See `backend/mqtt/hostelpulse-mosquitto.conf` for the local broker configuration. Set its listener to the current network interface as needed; do not commit machine-specific addresses or credentials.

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
& "C:\Program Files\mosquitto\mosquitto_sub.exe" -h localhost -p 1883 -t "hostelpulse/test" -v
```

### Publisher

```powershell
& "C:\Program Files\mosquitto\mosquitto_pub.exe" -h localhost -p 1883 -t "hostelpulse/test" -m "HostelPulse hotspot MQTT is working!"
```

### Received Message

```text
hostelpulse/test HostelPulse hotspot MQTT is working!
```

The successful message confirms that the broker accepts MQTT connections through the laptop's hotspot network interface.

## 8. Network Connectivity Verification

The laptop successfully pinged the ESP8266:

```powershell
ping <current-device-address>
```

Use the current device address from the local network settings if you need to check basic IP connectivity.

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
| ESP8266-to-MQTT test publishing | Completed for a standalone test message |
| Physical sensor telemetry integrated with backend | Pending |
| Python MQTT subscriber | Verified with simulated telemetry |
| PostgreSQL integration | Implemented with simulated telemetry; physical telemetry flow pending |

## 10. Next Development Steps

1. Install the PubSubClient library in Arduino IDE.
2. Connect the ESP8266 to the Mosquitto broker.
3. Publish MQTT test messages from the ESP8266.
4. Integrate MQTT publishing with the mess people-counter firmware.
5. Integrate physical sensor telemetry with the Python backend and verify it end to end.
6. Verify physical sensor telemetry through the backend and dashboard.

## 11. Security and Deployment Notes

- Anonymous MQTT access is currently enabled only for a controlled local hotspot demonstration.
- Do not expose port 1883 to public networks or the internet.
- Add authentication and appropriate access controls before wider deployment.
- The broker must be running on the laptop for local MQTT communication.
- The laptop and ESP8266 must be connected to the same local network.
- The current broker is manually started and is not yet configured as a persistent Windows service.


## 12. Standalone ESP8266-to-MQTT Test

The ESP8266 was successfully connected to the local Mosquitto broker using the PubSubClient 2.8.0 Arduino library.

### Configuration

| Parameter | Value |
|---|---|
| Device | NodeMCU ESP8266 |
| MQTT Library | PubSubClient 2.8.0 |
| Broker address | Current local broker address (configure locally) |
| Broker Port | 1883 |
| MQTT Topic | hostelpulse/test |
| Publishing Interval | 5 seconds |

### Test Results

- ESP8266 successfully connected to the Android hotspot.
- ESP8266 successfully established an MQTT connection.
- MQTT messages were published repeatedly.
- Mosquitto received the messages.
- PowerShell subscriber displayed the received messages.

### Sample Message

```text
Hello from HostelPulse ESP8266!
```

### Result

ESP8266-to-Mosquitto MQTT communication was successfully verified over the local hotspot network.

This standalone test does not verify physical sensor telemetry flowing through the backend. Integrating that path into the mess people-counter firmware remains pending.

## 13. Python Backend MQTT Milestone

The following backend components have been verified:

- The FastAPI `/health` endpoint responds successfully.
- The Python MQTT subscriber connects to the configured Mosquitto broker and subscribes to `hostelpulse/mess/mess-node-01/telemetry`.
- A simulated JSON telemetry message with `"event": "ENTRY"` was received and parsed successfully.
- PostgreSQL accepts connections, the backend creates its tables, and a simulated MQTT `ENTRY` payload is validated, stored, and returned by `/telemetry/latest`, `/occupancy/current`, and `/telemetry/events`.

To repeat the simulated telemetry check, start the broker, then start the backend from the `backend` directory. If the broker is not at the default `127.0.0.1:1883`, set `MQTT_HOST` and `MQTT_PORT` in that terminal first. Use the broker address configured on your current network; do not copy a test-session IP into the repository:

```powershell
if (-not (Test-Path .env)) { Copy-Item .env.example .env }
.\.venv\Scripts\Activate.ps1
python -m pip install -r requirements.txt
$env:MQTT_HOST = "127.0.0.1" # Use the address configured on the broker listener.
$env:MQTT_PORT = "1883"
uvicorn app.main:app --reload
```

In a second terminal, check health and publish a sample message (adjust the Mosquitto executable path if needed):

```powershell
Invoke-RestMethod http://127.0.0.1:8000/health
& "C:\Program Files\mosquitto\mosquitto_pub.exe" -h localhost -p 1883 -t "hostelpulse/mess/mess-node-01/telemetry" -m '{"schema_version":1,"device_id":"mess-node-01","event":"ENTRY","entries":1,"exits":0,"occupancy":1,"sensor_a":0,"sensor_b":1,"uptime_ms":125000}'
```

The backend terminal should print that the telemetry was saved. Check `/telemetry/latest`, `/occupancy/current`, and `/telemetry/events` in the API or at `http://127.0.0.1:8000/docs`. This test uses a generated message; physical ESP8266 sensor telemetry through the backend remains unverified. The dashboard is served at `http://127.0.0.1:8000/` and reads the same API.

## 14. Crowd-Level LED Return Path

The backend publishes `LOW`, `MODERATE`, or `BUSY` to `hostelpulse/mess/mess-node-01/crowd-level` after storing a valid telemetry event. The separate `firmware/mess-node/mqtt_led_indicator/mqtt_led_indicator.ino` sketch subscribes to that topic and drives green, yellow, or red LEDs. It does not read the sensors, so generated telemetry can exercise the output path.

For the ESP8266 to reach Mosquitto, the broker listener must include the laptop's hotspot interface. The repository broker configuration now listens on its network interfaces; it allows anonymous connections for this controlled local demo, so keep the broker on a trusted private hotspot and do not expose the MQTT port to the internet. Set `MQTT_SERVER` in the LED sketch to the laptop's current hotspot IPv4 address (check `ipconfig` if it changed). Create the local `secrets.h` from the example and set the hotspot Wi-Fi credentials.

Wire each LED through its own current-limiting resistor: green to D1, yellow to D2, red to D7, and each LED's cathode to GND. Upload the LED indicator sketch. Start Mosquitto and the backend, then publish telemetry as usual. The backend log should show the published crowd level; a subscriber to the crowd-level topic should see the same value, and the matching LED should turn on. The dashboard and Telegram status use the same backend occupancy and crowd thresholds.
