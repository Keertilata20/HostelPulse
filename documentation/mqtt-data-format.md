
# HostelPulse - MQTT Data Format

## 1. Overview

HostelPulse uses MQTT to deliver mess occupancy telemetry to the backend. The verified software demo publishes generated events to local Mosquitto; physical sensor-to-backend publishing remains in progress.

The ESP8266 processes sensor readings locally and publishes structured telemetry messages to an MQTT broker.

The current demo uses a local Mosquitto broker. A hosted broker such as HiveMQ is a possible future deployment option.

## 2. Communication Architecture

```text
Generated demo publisher (or, in the target setup, ESP8266 Mess Node)
       |
       | Wi-Fi
       v
MQTT Broker (HiveMQ Cloud)
       |
       | MQTT Subscription
       v
Python Backend
       |
       v
PostgreSQL Database
```

## 3. MQTT Topics

The backend subscribes to the telemetry topic and publishes the crowd-level response. The status topic is reserved for future device availability reporting:

| Topic | Purpose |
|---|---|
| hostelpulse/mess/mess-node-01/telemetry | Occupancy and sensor telemetry |
| hostelpulse/mess/mess-node-01/crowd-level | Backend-to-device crowd-level LED command (`LOW`, `MODERATE`, or `BUSY`) |
| hostelpulse/mess/mess-node-01/status | Device online/offline status |

After a valid telemetry event is stored, the backend publishes the calculated crowd level. The LED indicator sketch subscribes to this topic, and the message is retained so a reconnecting indicator receives the latest level. The LED-only demo sketch does not read sensors or publish sensor telemetry.

The device identifier `mess-node-01` uniquely identifies the mess hardware node.

## 4. Telemetry Message Structure

The telemetry message will use JSON format.

### Example Payload

```json
{
  "schema_version": 1,
  "device_id": "mess-node-01",
  "event": "ENTRY",
  "entries": 18,
  "exits": 6,
  "occupancy": 12,
  "sensor_a": 0,
  "sensor_b": 0,
  "uptime_ms": 125000
}
```

The values above are illustrative.

## 5. Field Description

| Field | Data Type | Description |
|---|---|---|
| schema_version | Integer | Version of the message structure |
| device_id | String | Unique hardware node identifier |
| event | String | Detected event: ENTRY or EXIT |
| entries | Integer | Cumulative detected entries |
| exits | Integer | Cumulative detected exits |
| occupancy | Integer | Estimated current occupancy |
| sensor_a | Integer | Sensor A state |
| sensor_b | Integer | Sensor B state |
| uptime_ms | Integer | Device uptime in milliseconds |

### Sensor State Convention

| Value | Meaning |
|---|---|
| 0 | Sensor clear |
| 1 | Object detected |

The values represent logical sensor states, not raw electrical voltage.

## 6. Occupancy Calculation

The ESP8266 calculates occupancy locally.

```text
Occupancy = Entries - Exits
```

The occupancy value must never become negative.

## 7. Event Handling

Two movement sequences are used:

| Sensor Sequence | Event |
|---|---|
| A → B | ENTRY |
| B → A | EXIT |

The ESP8266 publishes a telemetry message after a valid movement event.

## 8. MQTT Communication Configuration

The current local demo configuration is:

| Parameter | Configuration |
|---|---|
| Protocol | MQTT |
| Broker | Local Mosquitto |
| Transport | MQTT over local TCP |
| Port | 1883 |
| Payload format | JSON |
| Publisher | Generated demo events; ESP8266 firmware path not yet verified end to end |
| Subscriber | Python Backend |

Broker credentials must be stored securely and must not be committed to the public repository.

## 9. Device Status

The status topic is reserved for device availability but is not implemented yet.

Future implementation can use MQTT Last Will and Testament (LWT) to indicate unexpected disconnections.

Retained online/offline messages are not implemented yet.

## 10. Implementation Status

- [x] Version 1 JSON telemetry schema defined and validated by backend.
- [x] Backend subscription to local Mosquitto telemetry topic.
- [x] Generated telemetry persisted to PostgreSQL and exposed by API.
- [x] Standalone ESP8266-to-Mosquitto test message published.
- [ ] Verify sensor-derived telemetry from ESP8266 through backend.
- [ ] Device status topic and LWT.
- [ ] Hosted broker/TLS configuration, if needed for deployment.

## 11. Future Extensions

The message schema can be extended with:

- Timestamp from NTP synchronization.
- Signal strength (RSSI).
- Device diagnostics.
- Sensor fault indicators.
- Firmware version.
