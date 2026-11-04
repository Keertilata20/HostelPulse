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

| Feature | What It Does |
|---|---|
| 👥 Smart People Counting | Uses two IR sensors to detect entry and exit movements. |
| 📊 Occupancy Tracking | Maintains the estimated number of people inside the mess. |
| 📡 Wireless Communication | Sends device data through Wi-Fi using MQTT. |
| ⚙️ Backend Processing | Receives, validates and processes incoming telemetry. |
| 🗄️ Data Storage | Stores occupancy events and monitoring information in PostgreSQL. |
| 📈 Monitoring Dashboard | Presents occupancy information and monitoring data visually. |
| 🔔 Telegram Alerts | Provides mess updates and notifications through a Telegram bot. |
| 💧 Future Expansion | Can be extended to monitor water tanks and other hostel resources. |

---

## 🧠 How Does It Actually Work?

Think of HostelPulse as a small digital team where every component has a specific job!

### Step 1: 👀 Sensors Keep an Eye on the Door

Two IR sensors are placed near the mess entrance.

They detect the order in which a person crosses the doorway:

- Sensor A → Sensor B = **ENTRY**
- Sensor B → Sensor A = **EXIT**

### Step 2: ⚡ ESP8266 Does the Counting

The NodeMCU ESP8266 processes sensor signals and maintains entry, exit and occupancy counts.

The basic calculation is:

`Current Occupancy = Total Entries - Total Exits`

### Step 3: 📡 MQTT Sends the Information

The ESP8266 communicates over Wi-Fi and publishes telemetry using MQTT.

Eclipse Mosquitto acts as the MQTT message broker.

### Step 4: 🐍 Backend Processes the Data

The Python backend receives incoming MQTT messages, validates the data and makes monitoring information available through FastAPI.

### Step 5: 🗄️ PostgreSQL Stores the Records

Telemetry and occupancy information are stored for future access and analysis.

### Step 6: 📊 Dashboard Makes Everything Understandable

The dashboard presents monitoring information in a user-friendly visual format, helping users understand the current mess situation.

---

## 🏗️ System Architecture

The complete data flow follows this architecture:

```text
       👀 TWO IR SENSORS
       Entry / Exit Detection
                |
                v
       ⚡ ESP8266 NodeMCU
       People Counting Logic
                |
                | Wi-Fi / MQTT
                v
       📡 MOSQUITTO BROKER
       Message Communication
                |
                v
       🐍 PYTHON BACKEND
       MQTT Subscriber + FastAPI
                |
                v
       🗄️ POSTGRESQL DATABASE
       Telemetry & Occupancy Data
                |
                v
       📊 REACT DASHBOARD
       Monitoring & Visualization
                |
                v
       🔔 TELEGRAM NOTIFICATIONS
       Mess Updates & Alerts
```

**In simple words:** Sensors detect people, ESP8266 counts them, MQTT transfers the information, the backend processes it, PostgreSQL stores it, and the dashboard makes it easy to understand.

---

## 🛠️ Technology Stack

| Layer | Technology | Purpose |
|---|---|---|
| Microcontroller | NodeMCU ESP8266 | Processes sensor data |
| Sensors | IR Proximity Sensors | Detects movement |
| Firmware | Embedded C++ / Arduino IDE | Hardware programming |
| Communication | Wi-Fi, MQTT | Transfers telemetry |
| MQTT Broker | Eclipse Mosquitto | Message handling |
| Backend | Python, FastAPI | Data processing and APIs |
| Database | PostgreSQL | Data storage |
| Frontend | React | Monitoring interface |
| Notifications | Telegram Bot API | Alerts and updates |
| Version Control | Git & GitHub | Source-code management |

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

**Example Telemetry Message**

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

### 📡 MQTT Communication
- [x] Mosquitto MQTT broker installation and configuration
- [x] Local MQTT publish/subscribe testing
- [x] ESP8266-to-MQTT integration
- [x] MQTT test message publishing
- [x] Sensor-based MQTT telemetry publishing

### 🐍 Backend & Database
- [x] Python MQTT subscriber implementation
- [x] MQTT message reception and parsing
- [x] Physical ESP8266 sensor telemetry integration
- [x] FastAPI backend integration
- [x] PostgreSQL connectivity and data persistence

### 📊 Dashboard & Notifications
- [x] Dashboard integration with backend
- [x] Occupancy monitoring and visualization
- [x] Telegram bot integration
- [x] Telegram notifications verified with live telemetry

### 🧪 Testing
- [x] Hardware and connectivity testing
- [x] End-to-end system verification
- [x] On-site testing and accuracy evaluation

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

- 🚰 **Smart Water Tank Monitoring:** Monitor hostel water levels.
- 📊 **Predictive Crowd Analysis:** Identify busy hours using historical data.
- ⏱️ **Improved Waiting-Time Estimation:** Provide more useful queue predictions.
- 🏢 **Multi-Hostel Support:** Extend monitoring to multiple hostel buildings.
- 📉 **Advanced Analytics:** Generate occupancy reports and usage trends.

---

## 🌱 Our Vision

We believe technology should solve everyday problems, not just complicated ones.

A smarter hostel doesn't necessarily need expensive infrastructure. Sometimes, a few sensors, a microcontroller and a thoughtful software system can make a meaningful difference.

HostelPulse is our step towards making hostel life more organized, convenient and intelligent.

<p align="center">
  <strong>Built with curiosity, technology and a vision for smarter hostels. 🌿</strong>
</p>
