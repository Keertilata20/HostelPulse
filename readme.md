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
| 👥 People Counting | Sensor-counter prototype and MQTT telemetry firmware exist; physical sensor-to-backend flow is not yet verified | Prototype |
| 📊 Live Occupancy | Backend stores counts and the dashboard reads current occupancy from the API | Implemented with demo MQTT events |
| ⏱️ Wait-Time Estimation | Estimates queue waiting time using occupancy and exit rate | Planned |
| 📡 MQTT Communication | Local Mosquitto, backend subscriber, database, and API work with generated telemetry | Verified with demo events |
| 📈 Occupancy History | Stores events and charts recent occupancy readings | Implemented |
| 🔔 Telegram Bot | `/status` replies and alerts when occupancy crosses the Busy threshold | Implemented for one configured chat or group |
| 💧 Water Tank Monitoring | Originally proposed extension for hostel water monitoring | Future scope |

---

## 🏗️ System Architecture

The current software demo uses a local Mosquitto broker and generated MQTT events. The ESP8266 counter and telemetry sketches are present, but physical sensor telemetry has not yet been verified end to end through the backend.

```text
       ┌──────────────────────┐
       │ Generated MQTT event │
       │ (current demo input) │
       └──────────┬───────────┘
                  │
                  │ MQTT
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
       │ Telemetry + current  │
       └──────────┬───────────┘
                  │
           ┌──────┴───────┐
           ▼              ▼
┌───────────────────┐ ┌───────────────────┐
│ Static Dashboard  │ │ Telegram Bot API   │
│ HTML / CSS / JS   │ │ status + alerts    │
└───────────────────┘ └───────────────────┘
```

The ESP8266 sensors and firmware are the intended telemetry source; the current verified software demo injects generated events at the MQTT broker.

### Communication Strategy

- **Local-first operation:** The ESP8266 and laptop communicate over a shared Wi-Fi hotspot.
- **MQTT messaging:** Mosquitto acts as the local message broker.
- **Backend processing:** FastAPI validates telemetry received by a Python MQTT subscriber and exposes health, occupancy, event-history, and crowd-status endpoints.
- **Data persistence:** PostgreSQL stores telemetry events and the latest occupancy per device.
- **Visualization:** FastAPI serves the static HTML/CSS/JavaScript dashboard, which polls the API and displays occupancy, crowd level, recent events, and a history chart.
- **Notifications:** The optional Telegram bot replies to `/status` and sends a message when occupancy crosses into the configured Busy level.

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
| Frontend | HTML, CSS, JavaScript (served by FastAPI) |
| Notifications | Telegram Bot API |
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

The telemetry topic is used by the current backend. The device status topic and MQTT LWT behavior are proposed but not implemented yet.

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
│       ├── mess-node.ino                 # People-counter prototype
│       ├── mqtt_sensor_integration/      # Sensor counting + MQTT sketch
│       ├── mqtt_telemetry_test/          # One-shot generated telemetry sketch
│       ├── mqtt_test/                    # Standalone MQTT connectivity test
│       ├── sensor_diagnostic/            # Sensor pin diagnostic
│       └── wifi_test/                    # Wi-Fi connectivity test
│
├── hardware/
│   └── components.md
│
├── backend/
│   ├── app/
│   │   ├── main.py                       # FastAPI routes and startup
│   │   ├── mqtt_client.py                 # MQTT subscription and validation
│   │   ├── telemetry.py                   # Versioned telemetry schema
│   │   ├── database.py                    # PostgreSQL storage and queries
│   │   ├── crowd.py                       # Configurable crowd levels
│   │   └── telegram_bot.py                # Telegram commands and alerts
│   ├── get_telegram_chat_id.ps1           # Telegram chat ID helper
│   ├── requirements.txt
│   ├── .env.example                        # Environment variable template
│   └── mqtt/
│       └── hostelpulse-mosquitto.conf
│
├── frontend/
│   ├── index.html                         # Dashboard served by FastAPI
│   └── frontend-space.html                # Standalone visual theme prototype
│
├── documentation/
│   ├── architecture.md
│   ├── mqtt-data-format.md
│   ├── mqtt-setup.md
│   ├── telegram-bot-setup.md
│   └── wifi-connectivity.md
│
├── .gitignore
└── readme.md
```

---

## 🚧 Development Roadmap

- [x] Initial ESP8266 hardware setup
- [x] IR sensor experimentation
- [x] Wi-Fi connectivity testing
- [x] Mosquitto MQTT broker installation
- [x] Local MQTT publish/subscribe testing
- [x] Hotspot network connectivity verification
- [x] Standalone ESP8266-to-Mosquitto publishing test
- [x] MQTT test message publishing
- [x] Sensor-based MQTT telemetry sketch implemented
- [x] Python MQTT subscriber receives and validates generated telemetry
- [x] PostgreSQL event and current-occupancy persistence
- [x] FastAPI occupancy, event, health, and crowd-status endpoints
- [x] HTML/CSS/JavaScript dashboard with occupancy history chart
- [x] Configurable crowd levels and Telegram Busy-threshold alert
- [x] Telegram `/status` and `/help` commands
- [x] Physical ESP8266 sensor telemetry integrated with and verified through the backend
- [x] MQTT device status topic and Last Will configuration
- [x] On-site testing and accuracy evaluation

### Backend Milestone

Verified for the software demo: generated `ENTRY` and `EXIT` events pass through local Mosquitto, are validated by the Python subscriber, stored in PostgreSQL, and returned by the FastAPI endpoints. The dashboard reads those endpoints, and the Telegram bot responds to `/status` and sends a Busy-threshold alert. This confirms the software path; physical ESP8266 sensor telemetry through the backend and on-site sensor accuracy remain unverified.

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
- MQTT device availability with LWT.
- Multi-hostel monitoring and analytics.

---

## 🌱 Our Vision

HostelPulse is an attempt to make everyday hostel life a little more predictable through accessible IoT technology.

We believe shared resources should be monitored intelligently, not managed through guesswork.

**Built with curiosity, code, and the goal of making hostel life smarter.**
