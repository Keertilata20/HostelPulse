#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include "secrets.h"

// MQTT broker configuration
const char* MQTT_SERVER = "10.231.36.131";
const int MQTT_PORT = 1883;

const char* CROWD_TOPIC =
    "hostelpulse/mess/mess-node-01/crowd-level";

// LED pin assignments
// LOW      = Red LED on D1
// MODERATE = White LED on D2
// BUSY     = Red LED on D7

const int LED_LOW = D1;
const int LED_MODERATE = D2;
const int LED_BUSY = D7;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


// ------------------------------------
// DISPLAY CROWD LEVEL
// ------------------------------------

void showCrowdLevel(const char* level) {

  // Turn OFF all LEDs first
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


// ------------------------------------
// MQTT MESSAGE CALLBACK
// ------------------------------------

void handleMqttMessage(
    char* topic,
    byte* payload,
    unsigned int length
) {

  if (strcmp(topic, CROWD_TOPIC) != 0 || length >= 16) {
    return;
  }

  char level[16];

  memcpy(level, payload, length);
  level[length] = '\0';

  showCrowdLevel(level);
}


// ------------------------------------
// WIFI CONNECTION
// ------------------------------------

void connectToWiFi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");

  unsigned long startAttempt = millis();

  while (
      WiFi.status() != WL_CONNECTED &&
      millis() - startAttempt < 20000
  ) {

    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println();
    Serial.println("Wi-Fi connected.");

    Serial.print("ESP8266 IP: ");
    Serial.println(WiFi.localIP());

  } else {

    Serial.println();
    Serial.println("Wi-Fi connection timed out.");
  }
}


// ------------------------------------
// MQTT CONNECTION
// ------------------------------------

void connectToMqtt() {

  while (
      WiFi.status() == WL_CONNECTED &&
      !mqttClient.connected()
  ) {

    String clientId = "HostelPulse-led-node-";
    clientId += String(ESP.getChipId(), HEX);

    Serial.print("Connecting to MQTT broker...");

    if (mqttClient.connect(clientId.c_str())) {

      Serial.println("connected.");

      if (mqttClient.subscribe(CROWD_TOPIC, 1)) {

        Serial.print("Subscribed to: ");
        Serial.println(CROWD_TOPIC);

      } else {

        Serial.println("MQTT subscription failed.");
      }

    } else {

      Serial.print("MQTT connection failed. State: ");
      Serial.println(mqttClient.state());

      delay(5000);
    }
  }
}


// ------------------------------------
// SETUP
// ------------------------------------

void setup() {

  Serial.begin(115200);

  pinMode(LED_LOW, OUTPUT);
  pinMode(LED_MODERATE, OUTPUT);
  pinMode(LED_BUSY, OUTPUT);

  // Initially turn OFF all LEDs
  digitalWrite(LED_LOW, LOW);
  digitalWrite(LED_MODERATE, LOW);
  digitalWrite(LED_BUSY, LOW);

  Serial.println();
  Serial.println("--------------------------------");
  Serial.println("HOSTELPULSE MQTT LED CONTROLLER");
  Serial.println("--------------------------------");

  Serial.println("Red   = LOW");
  Serial.println("White = MODERATE");
  Serial.println("Red   = BUSY");

  connectToWiFi();

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(handleMqttMessage);
}


// ------------------------------------
// MAIN LOOP
// ------------------------------------

void loop() {

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("Wi-Fi disconnected. Reconnecting...");

    connectToWiFi();
  }

  if (WiFi.status() == WL_CONNECTED &&
      !mqttClient.connected()) {

    connectToMqtt();
  }

  if (mqttClient.connected()) {

    mqttClient.loop();
  }

  delay(20);
}
