
#include <ESP8266WiFi.h>
#include "secrets.h"

unsigned long lastAttempt = 0;

const unsigned long RETRY_INTERVAL = 10000;

void connectWiFi() {
  Serial.println();
  Serial.println("Connecting to Wi-Fi...");

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startTime < 20000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected!");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.print("Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

  } else {
    Serial.println("Wi-Fi connection failed.");
    Serial.println("Will retry later.");
  }

  lastAttempt = millis();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("HostelPulse Wi-Fi Test");

  connectWiFi();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    static unsigned long lastStatus = 0;

    if (millis() - lastStatus >= 10000) {
      Serial.println("Wi-Fi: Connected");

      Serial.print("IP: ");
      Serial.println(WiFi.localIP());

      Serial.print("RSSI: ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm");

      lastStatus = millis();
    }
  }
  else if (millis() - lastAttempt >= RETRY_INTERVAL) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    connectWiFi();
  }

  delay(100);
}
