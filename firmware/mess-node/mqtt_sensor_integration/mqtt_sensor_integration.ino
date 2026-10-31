#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

// Uses the existing active-low two-sensor sequence logic.
// Do not connect an E18-D80NK output to these pins until its voltage is measured.
const int SENSOR_A = D5;
const int SENSOR_B = D6;

const char* MQTT_SERVER = "10.231.36.131";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "hostelpulse/mess/mess-node-01/telemetry";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

int peopleCount = 0;
int entries = 0;
int exits = 0;
int previousA = HIGH;
int previousB = HIGH;

unsigned long sequenceStart = 0;
const unsigned long TIMEOUT = 1500;

int sequence = 0;
// 0 = waiting
// 1 = A triggered first
// 2 = B triggered first

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
    String clientId = "HostelPulse-mess-node-01-";
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

void publishTelemetry(const char* eventName, int a, int b) {
  char payload[224];
  snprintf(
    payload,
    sizeof(payload),
    "{\"schema_version\":1,\"device_id\":\"mess-node-01\",\"event\":\"%s\",\"entries\":%d,\"exits\":%d,\"occupancy\":%d,\"sensor_a\":%d,\"sensor_b\":%d,\"uptime_ms\":%lu}",
    eventName,
    entries,
    exits,
    peopleCount,
    a == LOW ? 1 : 0,
    b == LOW ? 1 : 0,
    millis()
  );

  if (mqttClient.publish(MQTT_TOPIC, payload)) {
    Serial.print("Published ");
    Serial.print(eventName);
    Serial.print(": ");
    Serial.println(payload);
  } else {
    Serial.println("MQTT publish failed.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(SENSOR_A, INPUT);
  pinMode(SENSOR_B, INPUT);

  Serial.println();
  Serial.println("HostelPulse two-sensor MQTT firmware");
  Serial.println("Sensor A -> D5; Sensor B -> D6");
  Serial.println("Counters start at zero after each restart.");

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  connectToWiFi();
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

  int a = digitalRead(SENSOR_A);
  int b = digitalRead(SENSOR_B);

  bool hitA = (a == LOW && previousA == HIGH);
  bool hitB = (b == LOW && previousB == HIGH);

  if (sequence == 0) {
    if (hitA) {
      sequence = 1;
      sequenceStart = millis();
    } else if (hitB) {
      sequence = 2;
      sequenceStart = millis();
    }
  } else if (sequence == 1 && hitB) {
    entries++;
    peopleCount++;
    sequence = 0;

    Serial.print("ENTRY | People inside: ");
    Serial.println(peopleCount);
    publishTelemetry("ENTRY", a, b);
  } else if (sequence == 2 && hitA) {
    if (peopleCount > 0) {
      peopleCount--;
      exits++;
    }

    sequence = 0;

    Serial.print("EXIT | People inside: ");
    Serial.println(peopleCount);
    publishTelemetry("EXIT", a, b);
  }

  if (sequence != 0 && millis() - sequenceStart > TIMEOUT) {
    sequence = 0;
  }

  previousA = a;
  previousB = b;

  delay(20);
}
