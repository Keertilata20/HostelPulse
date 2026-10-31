#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

// This standalone sketch publishes a simulated event. It does not read sensors.
const char* MQTT_SERVER = "10.231.36.131";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "hostelpulse/mess/mess-node-01/telemetry";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

bool telemetryPublished = false;
unsigned long lastPublishAttempt = 0;

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
    String clientId = "HostelPulse-telemetry-";
    clientId += String(ESP.getChipId(), HEX);

    Serial.print("Connecting to MQTT broker...");
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected.");
    } else {
      Serial.print("failed, state = ");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("HostelPulse simulated telemetry test");
  Serial.println("Sensors are not read by this sketch.");

  connectToWiFi();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
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

  if (!telemetryPublished && millis() - lastPublishAttempt >= 5000) {
    lastPublishAttempt = millis();

    char payload[192];
    snprintf(
      payload,
      sizeof(payload),
      "{\"schema_version\":1,\"device_id\":\"mess-node-01\",\"event\":\"ENTRY\",\"entries\":1,\"exits\":0,\"occupancy\":1,\"sensor_a\":0,\"sensor_b\":1,\"uptime_ms\":%lu}",
      millis()
    );

    if (mqttClient.publish(MQTT_TOPIC, payload)) {
      Serial.println("Simulated telemetry published once:");
      Serial.println(payload);
      telemetryPublished = true;
    } else {
      Serial.println("Publish failed; will retry in 5 seconds.");
    }
  }
}
