
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

// Wi-Fi credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// MQTT broker details
const char* mqtt_server = "10.231.36.131";
const int mqtt_port = 1883;

// MQTT topic
const char* mqtt_topic = "hostelpulse/test";

// Wi-Fi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastPublish = 0;

// Connect ESP8266 to Wi-Fi
void setupWiFi() {
  Serial.print("Connecting to Wi-Fi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("ESP8266 IP: ");
  Serial.println(WiFi.localIP());
}

// Connect ESP8266 to MQTT broker
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");

    String clientId = "HostelPulse-";
    clientId += String(ESP.getChipId(), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Connected!");
    } else {
      Serial.print("Failed, state = ");
      Serial.print(client.state());
      Serial.println(". Retrying in 5 seconds.");

      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println("     HOSTELPULSE MQTT TEST");
  Serial.println("================================");

  setupWiFi();

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {

  // Reconnect Wi-Fi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    setupWiFi();
  }

  // Reconnect MQTT if disconnected
  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();

  // Publish a message every 5 seconds
  if (millis() - lastPublish >= 5000) {
    lastPublish = millis();

    const char* message = "Hello from HostelPulse ESP8266!";

    if (client.publish(mqtt_topic, message)) {
      Serial.println("MQTT message published successfully!");
      Serial.println(message);
    } else {
      Serial.println("MQTT publish failed!");
    }
  }
}
