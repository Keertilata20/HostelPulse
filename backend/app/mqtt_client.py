import json
import os
from collections.abc import Callable
from typing import Any

import paho.mqtt.client as mqtt


class MqttSubscriber:
    def __init__(self, on_telemetry: Callable[[dict[str, Any]], None]) -> None:
        self._on_telemetry = on_telemetry
        self._host = os.getenv("MQTT_HOST", "127.0.0.1")
        self._port = int(os.getenv("MQTT_PORT", "1883"))
        self._topic = os.getenv(
            "MQTT_TELEMETRY_TOPIC",
            "hostelpulse/mess/mess-node-01/telemetry",
        )
        self._client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self._client.on_connect = self._handle_connect
        self._client.on_message = self._handle_message

    def start(self) -> None:
        try:
            self._client.connect(self._host, self._port, keepalive=60)
            self._client.loop_start()
            print(f"Connecting to MQTT at {self._host}:{self._port}", flush=True)
        except OSError as exc:
            print(f"MQTT connection failed: {exc}", flush=True)

    def stop(self) -> None:
        self._client.disconnect()
        self._client.loop_stop()

    def _handle_connect(self, client, userdata, flags, reason_code, properties) -> None:
        if reason_code == 0:
            client.subscribe(self._topic)
            print(f"Subscribed to {self._topic}", flush=True)
        else:
            print(f"MQTT connection refused: {reason_code}", flush=True)

    def _handle_message(self, client, userdata, message) -> None:
        try:
            payload = json.loads(message.payload.decode("utf-8"))
            if not isinstance(payload, dict):
                raise ValueError("Telemetry JSON must be an object")
            self._on_telemetry(payload)
        except (UnicodeDecodeError, json.JSONDecodeError, ValueError) as exc:
            print(f"Ignored invalid telemetry: {exc}", flush=True)
