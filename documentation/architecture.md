
# HostelPulse - System Architecture

## 1. Introduction

HostelPulse is an IoT-based hostel monitoring system designed to monitor mess occupancy and provide real-time information about crowd levels.

The target system uses an ESP8266 node with two infrared proximity sensors to detect entries and exits. The current software demonstration sends generated telemetry through a local Mosquitto broker; physical sensor telemetry has not yet been verified end to end through the backend.

## 2. High-Level Architecture

The target system consists of five major layers. In the current software demo, generated events enter at the broker, bypassing the physical sensing and edge-processing layers:

1. **Sensing Layer:** Detects movement using two infrared proximity sensors.
2. **Edge Processing Layer:** Uses the ESP8266 to determine movement direction and calculate occupancy.
3. **Communication Layer:** Transfers telemetry data using Wi-Fi and MQTT.
4. **Backend Layer:** Processes incoming data and manages storage.
5. **Application Layer:** Displays occupancy information and crowd statistics through the frontend dashboard.

## 3. System Architecture Diagram

```text
+--------------------------------------+
|             SENSING LAYER            |
|                                      |
|       IR Sensor A    IR Sensor B     |
|             |            |           |
+-------------|------------|-----------+
              |            |
              v            v
+--------------------------------------+
|          EDGE PROCESSING             |
|                                      |
|           ESP8266 NodeMCU            |
|                                      |
|   - Sensor reading                   |
|   - Direction detection              |
|   - Entry/Exit counting              |
|   - Occupancy calculation            |
|                                      |
+--------------------------------------+
                  |
                  | Wi-Fi
                  v
+--------------------------------------+
|          COMMUNICATION LAYER         |
|                                      |
|             MQTT Broker              |
|       Local Mosquitto Broker          |
|                                      |
+--------------------------------------+
                  |
                  | MQTT Messages
                  v
+--------------------------------------+
|             BACKEND LAYER            |
|                                      |
|   Python Backend / FastAPI           |
|   MQTT Subscriber                    |
|   PostgreSQL                         |
|   telemetry_events                   |
|   current_occupancy                  |
|                                      |
|   - Data processing                  |
|   - Occupancy monitoring             |
|   - Historical data storage          |
+--------------------------------------+
                  |
                  | API / Data Access
                  v
+--------------------------------------+
|           APPLICATION LAYER          |
|                                      |
|    Static Web Dashboard              |
|    HTML / CSS / JavaScript           |
|                                      |
|   - Current occupancy                |
|   - Entry and exit statistics        |
|   - Crowd level                      |
|   - Recent events and history chart  |
|   - Configured capacity gauge        |
+--------------------------------------+
```

## 4. Working Principle

### 4.1 Movement Detection

Two infrared proximity sensors are positioned sequentially along the entrance path.

The order in which the sensors detect an object is used to determine the direction of movement.

| Detection Sequence | Interpretation |
|---|---|
| Sensor A followed by Sensor B | Entry |
| Sensor B followed by Sensor A | Exit |

### 4.2 Edge Processing

The ESP8266 reads the digital outputs of both sensors.

The firmware maintains three important variables:

- `entries`: Total number of detected entries.
- `exits`: Total number of detected exits.
- `occupancy`: Estimated number of people currently inside.

The occupancy is calculated as:

```text
Occupancy = Total Entries - Total Exits
```

The firmware also uses a sequence timeout to prevent incomplete sensor sequences from being counted.

### 4.3 Communication Using MQTT

For the software demonstration, generated telemetry is published to the local Mosquitto broker on `127.0.0.1:1883`. The ESP8266 also has a standalone MQTT test sketch and a sensor-counting/MQTT integration sketch. The physical sensor-to-backend path has not yet been verified end to end.

Topics:

```text
hostelpulse/mess/mess-node-01/telemetry
hostelpulse/mess/mess-node-01/status
```

The backend subscribes to the telemetry topic. The status topic and MQTT Last Will and Testament (LWT) behavior remain proposed work.

### 4.4 Backend Processing

The backend exposes a FastAPI health endpoint and a Python MQTT subscriber. With local Mosquitto running, the subscriber connects to `hostelpulse/mess/mess-node-01/telemetry`, validates version 1 JSON, and stores valid events in PostgreSQL.

PostgreSQL stores each valid event in `telemetry_events` and keeps the latest occupancy per device in `current_occupancy`. FastAPI exposes `/health`, `/telemetry/latest`, `/occupancy/current`, `/telemetry/events`, and `/crowd/status`. Database connectivity and schema initialization are checked during startup; the API does not start if PostgreSQL is unavailable.

Generated MQTT events have been received, saved, and read back through the API. Physical ESP8266 sensor telemetry through the backend remains unverified.

### 4.5 Frontend Visualization

The HTML/CSS/JavaScript dashboard in `frontend/index.html` is served by FastAPI at `/`. It polls the backend every two seconds and displays:

The dashboard polls the backend every two seconds and displays:

- Current number of people inside the mess.
- Total entries and exits.
- Crowd-level status.
- Occupancy trend and recent telemetry events.
- API connection state and configured demo capacity.

The page reads stored API data; it does not generate browser-side telemetry. `frontend/frontend-space.html` is a separate visual prototype.

### 4.6 Telegram Bot

When configured in `backend/.env`, the Telegram bot replies to `/status` and `/help`. It sends an alert when occupancy crosses from below the Busy threshold to at or above it. The bot is restricted to the configured private chat or group ID.

## 5. Crowd-Level Classification

The system classifies occupancy into crowd levels using configurable thresholds.

An initial conceptual classification is:

| Crowd Level | Interpretation |
|---|---|
| Low | Below the moderate threshold |
| Moderate | At or above moderate, below Busy |
| Busy | At or above the Busy threshold |

The demo defaults are Moderate at 5 people and Busy at 10 people. `MESS_CAPACITY` defaults to 80 for the dashboard gauge. These are configurable demo values, not measured mess-capacity settings.

## 6. Proposed Data Flow

```text
Person enters or exits
          |
          v
IR sensors detect movement
          |
          v
ESP8266 identifies direction
          |
          v
Occupancy count updated
          |
          v
Telemetry generated
          |
          v
MQTT broker receives data
          |
          v
Backend processes telemetry
          |
          v
Database stores records
          |
          v
Frontend displays information
```

## 7. Current Development Status

| Module | Status |
|---|---|
| ESP8266 board setup | Completed |
| Initial IR sensor testing | Completed |
| Basic entry/exit counting | Prototype completed |
| E18-D80NK integration | Pending |
| Wi-Fi communication | Connectivity tested; firmware integration pending |
| MQTT broker | Mosquitto tested locally |
| Python MQTT subscriber and PostgreSQL persistence | Simulated `ENTRY` received over MQTT, validated, stored, and returned through the API |
| Physical ESP8266 sensor telemetry through backend | Pending |
| PostgreSQL persistence | Implemented; simulated payload stored and read through database functions |
| Static dashboard backed by API | Implemented for generated telemetry |
| Telegram commands and Busy alert | Implemented for configured chat/group |
| Software path: generated MQTT → database → API/dashboard/bot | Verified |
| Physical sensor-to-backend end-to-end path | Pending |

## 8. Future Improvements

Potential next steps include:

- More robust sensor filtering and debounce logic.
- Automatic device status monitoring.
- Improved occupancy accuracy.
- Device availability monitoring with MQTT LWT.
- Historical analytics and visualization.
- Estimated waiting-time calculations.

## 9. Conclusion

HostelPulse combines embedded systems, IoT communication, backend processing, and frontend visualization to provide a centralized mess occupancy monitoring solution.

By performing initial movement detection and occupancy calculation directly on the ESP8266, the system reduces the amount of raw sensor data that must be transmitted and allows the backend to focus on monitoring, storage, and visualization.
