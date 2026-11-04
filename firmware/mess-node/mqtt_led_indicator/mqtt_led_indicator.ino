#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include "secrets.h"

// Set this to the laptop's current IPv4 address on the shared Wi-Fi hotspot.
// Mosquitto must listen on that interface for the ESP8266 to reach it.
const char* MQTT_SERVER = "10.231.36.131";
const int MQTT_PORT = 1883;
const char* CROWD_TOPIC = "hostelpulse/mess/mess-node-01/crowd-level";

// LED anodes connect to these pins through individual current-limiting
// resistors; LED cathodes connect to GND. These pins avoid D5/D6 sensor pins.
const int LED_LOW = D1;       // Green
const int LED_MODERATE = D2;  // Yellow
const int LED_BUSY = D7;      // Red

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void showCrowdLevel(const char* level) {
  digitalWrite(LED_LOW, LOW);
  digitalWrite(LED_MODERATE, LOW);
  digitalWrite(LED_BUSY, LOW);

  if (strcmp(level, "LOW") == 0) {
    digitalWrite(LED_LOW, HIGH);
  } else if (strcmp(level, "MODERATE") == 0) {
    digitalWrite(LED_MODERATE, HIGH);
  } else if (strcmp(level, "BUSY") == 0) {
    digitalWrite(LED_BUSY, HIGH);
  } else {
    Serial.print("Unknown crowd level: ");
    Serial.println(level);
    return;
  }

  Serial.print("Crowd level received: ");
  Serial.println(level);
}

void handleMqttMessage(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, CROWD_TOPIC) != 0 || length >= 16) {
    return;
  }

  char level[16];
  memcpy(level, payload, length);
  level[length] = '\0';
  showCrowdLevel(level);
}

void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected.");
  Serial.print("ESP8266 IP: ");
  Serial.println(WiFi.localIP());
}

void connectToMqtt() {
  while (!mqttClient.connected()) {
    String clientId = "HostelPulse-led-node-";
    clientId += String(ESP.getChipId(), HEX);

    Serial.print("Connecting to MQTT broker...");
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected.");
      if (mqttClient.subscribe(CROWD_TOPIC, 1)) {
        Serial.print("Subscribed to ");
        Serial.println(CROWD_TOPIC);
      } else {
        Serial.println("Could not subscribe to crowd-level topic.");
      }
    } else {
      Serial.print("failed, state = ");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_LOW, OUTPUT);
  pinMode(LED_MODERATE, OUTPUT);
  pinMode(LED_BUSY, OUTPUT);
  digitalWrite(LED_LOW, LOW);
  digitalWrite(LED_MODERATE, LOW);
  digitalWrite(LED_BUSY, LOW);

  Serial.println();
  Serial.println("HostelPulse MQTT crowd-level LED indicator");
  Serial.println("Green=LOW, yellow=MODERATE, red=BUSY");

  connectToWiFi();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(handleMqttMessage);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    connectToWiFi();
  }

  if (!mqttClient.connected()) {
    connectToMqtt();
  }

  mqttClient.loop();
  delay(20);
}
