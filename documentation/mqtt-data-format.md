
# HostelPulse - MQTT Data Format

## 1. Overview

HostelPulse uses the MQTT protocol for communication between the ESP8266 mess node and the backend system.

The ESP8266 processes sensor readings locally and publishes structured telemetry messages to an MQTT broker.

HiveMQ Cloud is the planned MQTT broker.

## 2. Communication Architecture

```text
ESP8266 Mess Node
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

The following MQTT topics are proposed:

| Topic | Purpose |
|---|---|
| hostelpulse/mess/mess-node-01/telemetry | Occupancy and sensor telemetry |
| hostelpulse/mess/mess-node-01/status | Device online/offline status |

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

The planned communication configuration is:

| Parameter | Configuration |
|---|---|
| Protocol | MQTT |
| Broker | HiveMQ Cloud |
| Transport | MQTT over TLS |
| Port | 8883 |
| Payload format | JSON |
| Publisher | ESP8266 Mess Node |
| Subscriber | Python Backend |

Broker credentials must be stored securely and must not be committed to the public repository.

## 9. Device Status

The status topic will be used to communicate device availability.

The implementation will use MQTT Last Will and Testament (LWT) to indicate unexpected disconnections.

A retained online message will be published after a successful connection.

## 10. Implementation Status

- [x] MQTT topic structure proposed.
- [x] JSON telemetry schema defined.
- [ ] JSON serialization in ESP8266 firmware.
- [ ] HiveMQ Cloud configuration.
- [ ] MQTT connection from ESP8266.
- [ ] Backend subscription.
- [ ] Telemetry storage.

## 11. Future Extensions

The message schema can be extended with:

- Timestamp from NTP synchronization.
- Signal strength (RSSI).
- Device diagnostics.
- Sensor fault indicators.
- Firmware version.
