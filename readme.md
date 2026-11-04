# 🌿 HostelPulse
### Making Hostel Life Smarter, One Sensor at a Time!

<p align="center">
  <strong>IoT-Based Smart Hostel Monitoring Platform</strong><br/>
  Real-Time Mess Monitoring · People Counting · Occupancy Tracking · Smart Insights
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Project-Completed-success?style=for-the-badge" />
  <img src="https://img.shields.io/badge/IoT-ESP8266-blue?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Communication-MQTT-purple?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Backend-FastAPI-green?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Database-PostgreSQL-blue?style=for-the-badge" />
</p>

---

## 💡 The Story Behind HostelPulse

Imagine this:

It's 8:45 AM. Your class starts at 9. You're hungry, so you rush to the mess. But surprise! There's already a long queue waiting outside.

Now you're wondering:

- Is the mess really that crowded?
- Should I go now or come back later?
- How long will I have to wait?
- Is there a smarter way to know all this?

**That's exactly the problem HostelPulse aims to solve!**

HostelPulse is an IoT-powered smart hostel monitoring platform that tracks mess entry, exit and occupancy using sensors and provides useful monitoring information through a digital dashboard.

Our idea is simple:

**Less guessing. Less waiting. Smarter hostel management.**

---

## 🎯 What Are We Trying to Achieve?

HostelPulse focuses on making everyday hostel management more intelligent and convenient.

1. **People Counting:** Detect how many people enter and leave the mess.
2. **Live Occupancy:** Maintain an estimate of the number of people currently inside.
3. **Waiting-Time Estimation:** Use occupancy and exit-rate information to estimate waiting time.
4. **Smart Monitoring:** Make mess status available through a digital interface.
5. **Historical Analysis:** Store telemetry data for future analysis and reporting.
6. **Affordable IoT:** Build a practical monitoring solution using accessible hardware and open-source technologies.

---

## ✨ What Can HostelPulse Do?

| Feature | Description | Status |
|---|---|---|
| 👥 People Counting | Sensor-counter prototype and MQTT telemetry firmware exist; physical sensor-to-backend flow is not yet verified | Prototype |
| 📊 Live Occupancy | Backend stores counts and the dashboard reads current occupancy from the API | Implemented with demo MQTT events |
| ⏱️ Wait-Time Estimation | Estimates queue waiting time using occupancy and exit rate | Planned |
| 📡 MQTT Communication | Local Mosquitto, backend subscriber, database, and API work with generated telemetry | Verified with demo events |
| 📈 Occupancy History | Stores events and charts recent occupancy readings | Implemented |
| 🚦 Crowd-Level LEDs | Backend MQTT response and ESP8266 LED receiver sketch | Implemented in code; hardware verification pending |
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

For the demo's return path, the backend publishes the calculated crowd level back to MQTT. An ESP8266 LED indicator sketch subscribes to that level, while the dashboard and Telegram bot read the same backend state. This demonstrates a two-direction MQTT flow with generated input; the LED sketch does not read sensors.

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

HostelPulse uses MQTT, a lightweight messaging protocol suitable for IoT devices.

**Broker Details**

- Broker: Eclipse Mosquitto 2.1.2
- Protocol: MQTT
- Port: 1883
- Development setup: Local broker over a shared Wi-Fi hotspot

**Telemetry Topic**

```text
hostelpulse/mess/mess-node-01/telemetry
```

**Device Status Topic**

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

This message tells the backend which device sent the information, what event occurred, and the current counting information.

*Note: The example values are illustrative.*

---

## 📂 Repository Structure

```text
HostelPulse/
│
├── firmware/
│   └── mess-node/
│       ├── mess-node.ino                 # People-counter prototype
│       ├── mqtt_sensor_integration/      # Sensor counting + MQTT sketch
│       ├── mqtt_led_indicator/           # Crowd-level MQTT receiver and LED outputs
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
└── README.md
```

---

## 🚀 Development Roadmap

From the first hardware experiment to the complete monitoring workflow, here's what we've worked on:

### 🔌 Hardware & Connectivity
- [x] Initial ESP8266 hardware setup
- [x] IR sensor experimentation
- [x] Wi-Fi connectivity testing
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
- [x] Backend crowd-level response topic and ESP8266 LED receiver sketch
- [ ] Verify physical LED response over the shared hotspot
- [ ] Physical ESP8266 sensor telemetry integrated with and verified through the backend
- [ ] MQTT device status topic and Last Will configuration
- [ ] On-site testing and accuracy evaluation

### 🐍 Backend & Database
- [x] Python MQTT subscriber implementation
- [x] MQTT message reception and parsing
- [ ] Physical ESP8266 sensor telemetry integration
- [x] FastAPI backend integration
- [x] PostgreSQL connectivity and data persistence

Verified for the software demo: generated `ENTRY` and `EXIT` events pass through local Mosquitto, are validated by the Python subscriber, stored in PostgreSQL, and returned by the FastAPI endpoints. The dashboard reads those endpoints, Telegram responds to `/status` and sends Busy-threshold alerts, and the backend publishes a crowd-level response for the ESP8266 LED sketch. Physical sensor telemetry and the LED response over Wi-Fi still need hardware verification.

---

## 📚 Project Documentation

Want to explore how everything works behind the scenes?

| Document | Description |
|---|---|
| [System Architecture](documentation/architecture.md) | Understand the complete system and data flow. |
| [Hardware Components](hardware/components.md) | Explore the hardware components and specifications. |
| [MQTT Data Format](documentation/mqtt-data-format.md) | Learn how device telemetry is structured. |
| [MQTT Setup](documentation/mqtt-setup.md) | Understand broker installation and communication. |
| [Wi-Fi Connectivity](documentation/wifi-connectivity.md) | Explore ESP8266 wireless connectivity. |
| [Telegram Bot Setup](documentation/telegram-bot-setup.md) | Learn about bot configuration and notifications. |

---

## 🔮 What's Next? — Future Scope

HostelPulse starts with mess monitoring, but the idea doesn't have to stop there!

Possible future enhancements include:

- Water tank level monitoring.
- Predictive mess crowd analysis.
- Intelligent waiting-time estimation.
- MQTT device availability with LWT.
- Multi-hostel monitoring and analytics.

---

## 🌱 Our Vision

We believe technology should solve everyday problems, not just complicated ones.

A smarter hostel doesn't necessarily need expensive infrastructure. Sometimes, a few sensors, a microcontroller and a thoughtful software system can make a meaningful difference.

HostelPulse is our step towards making hostel life more organized, convenient and intelligent.

<p align="center">
  <strong>Built with curiosity, technology and a vision for smarter hostels. 🌿</strong>
</p>
