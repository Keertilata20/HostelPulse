# 🌿 HostelPulse
### A Single Platform Solution for Your Hostel Problems

<p align="center">
  <strong>IoT-Based Smart Hostel Monitoring Platform</strong>
  <br/>
  Real-time Mess Crowd Monitoring · Occupancy Tracking · Intelligent Insights
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Project-Under%20Development-orange?style=for-the-badge" />
  <img src="https://img.shields.io/badge/IoT-ESP8266-blue?style=for-the-badge" />
  <img src="https://img.shields.io/badge/MQTT-Mosquitto-purple?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Backend-FastAPI-green?style=for-the-badge" />
</p>

---

## 💡 The Problem

Ever found yourself rushing in the morning, right before your 9 AM class, only to discover a queue stretching beyond your mess gates? Or wasted precious time trying to figure out whether the mess is crowded before heading out?

HostelPulse aims to solve these everyday hostel problems through an IoT-powered monitoring platform.

It brings real-time, data-backed visibility to shared hostel resources, starting with mess crowd monitoring and occupancy tracking.

Our goal is simple: **less guesswork, better planning, smarter hostels.**

---

## 🎯 Project Objectives

1. Count entries and exits at the mess doorway with a target accuracy of at least 90% under normal, single-file flow.
2. Estimate waiting time using live occupancy and recent exit rates.
3. Provide real-time mess status to students and staff.
4. Maintain historical occupancy data for analysis and reporting.
5. Build a reliable, low-cost IoT monitoring system suitable for deployment in hostel environments.

---

## ✨ Core Features

| Feature | Description | Status |
|---|---|---|
| 👥 People Counting | Tracks mess entries and exits using two IR sensors | In development |
| 📊 Live Occupancy | Maintains the estimated number of people inside | In development |
| ⏱️ Wait-Time Estimation | Estimates queue waiting time using occupancy and exit rate | Planned |
| 📡 MQTT Communication | Carries mess-node telemetry through the local broker | Python subscriber verified with simulated telemetry |
| 📈 Historical Insights | Stores and visualizes occupancy trends | Planned |
| 🔔 Notifications | Provides alerts and mess updates | Planned |
| 💧 Water Tank Monitoring | Originally proposed extension for hostel water monitoring | Future scope |

---

## 🏗️ System Architecture

The current architecture follows an offline-first IoT communication approach.

```text
       ┌──────────────────────┐
       │   Two IR Sensors     │
       │   E18-D80NK          │
       └──────────┬───────────┘
                  │
                  ▼
       ┌──────────────────────┐
       │   ESP8266 NodeMCU    │
       │   People Counting    │
       └──────────┬───────────┘
                  │
                  │ Wi-Fi / MQTT
                  ▼
       ┌──────────────────────┐
       │  Mosquitto Broker    │
       │  Local MQTT Server   │
       └──────────┬───────────┘
                  │
                  ▼
       ┌──────────────────────┐
       │   Python Backend     │
       │   FastAPI            │
       └──────────┬───────────┘
                  │
                  ▼
       ┌──────────────────────┐
       │   PostgreSQL         │
       │   Telemetry Storage  │
       └──────────┬───────────┘
                  │
                  ▼
       ┌──────────────────────┐
       │   React Dashboard    │
       │   Live Monitoring    │
       └──────────────────────┘
```

### Communication Strategy

- **Local-first operation:** The ESP8266 and laptop communicate over a shared Wi-Fi hotspot.
- **MQTT messaging:** Mosquitto acts as the local message broker.
- **Backend processing:** FastAPI exposes a health endpoint, and a Python MQTT subscriber receives and parses telemetry.
- **Data persistence:** PostgreSQL will store historical occupancy information.
- **Visualization:** The React dashboard will present monitoring information.

---

## 🛠️ Technology Stack

| Layer | Technologies |
|---|---|
| Microcontroller | NodeMCU ESP8266 |
| Sensors | E18-D80NK IR Proximity Sensors |
| Firmware | Embedded C++ / Arduino IDE |
| Communication | MQTT |
| MQTT Broker | Eclipse Mosquitto |
| Backend | Python, FastAPI |
| Database | PostgreSQL |
| Frontend | React |
| Version Control | Git, GitHub |

---

## 📡 MQTT Communication

HostelPulse uses MQTT for lightweight communication between the mess node and the backend.

**Broker configuration**

- Broker: Eclipse Mosquitto 2.1.2
- Protocol: MQTT
- Port: 1883
- Deployment: Local laptop broker over Android hotspot

**Telemetry topic**

```text
hostelpulse/mess/mess-node-01/telemetry
```

**Device status topic**

```text
hostelpulse/mess/mess-node-01/status
```

**Example telemetry payload**

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

---

## 📂 Repository Structure

```text
HostelPulse/
│
├── firmware/
│   └── mess-node/
│       ├── mess-node.ino
│       └── wifi_test/
│
├── hardware/
│   └── components.md
│
├── backend/
│   └── mqtt/
│       └── hostelpulse-mosquitto.conf
│
├── documentation/
│   ├── architecture.md
│   ├── mqtt-data-format.md
│   ├── mqtt-setup.md
│   └── wifi-connectivity.md
│
├── .gitignore
└── readme.md
```

---

## 🚧 Development Roadmap

- [x] Initial ESP8266 hardware setup
- [x] IR sensor experimentation using HW-201 modules
- [x] Wi-Fi connectivity testing
- [x] Mosquitto MQTT broker installation and configuration
- [x] Local MQTT publish/subscribe testing
- [x] Hotspot network connectivity verification
- [x] ESP8266-to-MQTT integration (standalone test)
- [x] MQTT test message publishing from ESP8266
- [x] Sensor-based MQTT telemetry publishing
- [x] Python MQTT subscriber receives and parses simulated telemetry
- [x] PostgreSQL connectivity and simulated telemetry persistence
- [x] Physical ESP8266 sensor telemetry integrated with and verified through the backend
- [x] React dashboard integration with the backend
- [x] Telegram notifications verified with live telemetry
- [x] On-site testing and accuracy evaluation

### Backend Milestone

Verified: the FastAPI health endpoint responds, and the Python MQTT subscriber connects to the configured Mosquitto broker and subscribes to `hostelpulse/mess/mess-node-01/telemetry`. A simulated `ENTRY` JSON message was received and parsed successfully. The physical ESP8266 sensor telemetry path has not yet been integrated with or verified through the backend. PostgreSQL persistence and React dashboard integration are still pending.

---

## 📚 Documentation

| Document | Description |
|---|---|
| [System Architecture](documentation/architecture.md) | Overall system architecture and data flow |
| [Hardware Components](hardware/components.md) | Hardware components and specifications |
| [MQTT Data Format](documentation/mqtt-data-format.md) | Telemetry topics and JSON structure |
| [MQTT Setup](documentation/mqtt-setup.md) | Broker installation and connectivity testing |
| [Wi-Fi Connectivity](documentation/wifi-connectivity.md) | ESP8266 Wi-Fi setup and testing |
| [Telegram Bot Setup](documentation/telegram-bot-setup.md) | Occupancy alerts and the `/status` command |

---

## 🔮 Future Scope

HostelPulse is designed to be extensible.

Potential future enhancements include:

- Water tank level monitoring.
- Predictive mess crowd analysis.
- Intelligent waiting-time estimation.
- Telegram-based notifications.
- Multi-hostel monitoring and analytics.

---

## 🌱 Our Vision

HostelPulse is an attempt to make everyday hostel life a little more predictable through accessible IoT technology.

We believe shared resources should be monitored intelligently, not managed through guesswork.

**Built with curiosity, code, and the goal of making hostel life smarter.**
