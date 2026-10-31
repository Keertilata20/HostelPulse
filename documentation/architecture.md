
# HostelPulse - System Architecture

## 1. Introduction

HostelPulse is an IoT-based hostel monitoring system designed to monitor mess occupancy and provide real-time information about crowd levels.

The system uses an ESP8266-based hardware node with two infrared proximity sensors to detect people entering and exiting the mess. The calculated occupancy information is transmitted to a backend system, which processes and stores the data for visualization through a web dashboard.

## 2. High-Level Architecture

The system consists of five major layers:

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
|   PostgreSQL (planned)               |
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
|           React Dashboard            |
|                                      |
|   - Current occupancy                |
|   - Entry and exit statistics        |
|   - Crowd level                      |
|   - Historical graphs                |
|   - Estimated waiting time            |
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

After calculating occupancy, the ESP8266 is intended to publish telemetry data to an MQTT broker through Wi-Fi. The Python backend subscriber has been verified separately with simulated telemetry; physical sensor telemetry has not yet been integrated with or verified through the backend.

Proposed MQTT topics:

```text
hostelpulse/mess/mess-node-01/telemetry
hostelpulse/mess/mess-node-01/status
```

The telemetry topic will carry occupancy and sensor information, while the status topic will indicate device connectivity.

### 4.4 Backend Processing

The backend exposes a FastAPI health endpoint and a Python MQTT subscriber. The health endpoint responds, and the subscriber has connected to the configured Mosquitto broker, subscribed to `hostelpulse/mess/mess-node-01/telemetry`, and successfully received and parsed a simulated `ENTRY` JSON message.

The planned backend responsibilities include:

- Receiving sensor-node telemetry.
- Processing occupancy information.
- Maintaining historical records.
- Providing data to the frontend.
- Supporting crowd-level monitoring.

PostgreSQL persistence is not implemented yet. React dashboard integration is also pending.

### 4.5 Frontend Visualization

The React-based dashboard will present the processed information in an understandable format.

Planned dashboard indicators include:

- Current number of people inside the mess.
- Total entries and exits.
- Crowd-level status.
- Historical occupancy graphs.
- Estimated waiting time.

## 5. Crowd-Level Classification

The system will classify mess occupancy into different crowd levels using configurable thresholds.

An initial conceptual classification is:

| Crowd Level | Interpretation |
|---|---|
| Green | Low crowd |
| Yellow | Moderate crowd |
| Red | High crowd |

The actual occupancy thresholds will be configured after testing and observing the mess capacity.

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
| Python MQTT subscriber | Connected, subscribed, and parsed simulated `ENTRY` telemetry |
| Physical ESP8266 sensor telemetry through backend | Pending |
| PostgreSQL persistence | Not implemented |
| React dashboard integration | Pending |
| End-to-end integration | Pending |

## 8. Future Improvements

The system can be extended with:

- More robust sensor filtering and debounce logic.
- Automatic device status monitoring.
- Improved occupancy accuracy.
- Telegram-based crowd alerts.
- Historical analytics and visualization.
- Estimated waiting-time calculations.

## 9. Conclusion

HostelPulse combines embedded systems, IoT communication, backend processing, and frontend visualization to provide a centralized mess occupancy monitoring solution.

By performing initial movement detection and occupancy calculation directly on the ESP8266, the system reduces the amount of raw sensor data that must be transmitted and allows the backend to focus on monitoring, storage, and visualization.
