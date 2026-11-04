import json
import os
from collections.abc import Callable

import paho.mqtt.client as mqtt
from pydantic import ValidationError

from .crowd import crowd_level
from .telemetry import TelemetryPayload


class MqttSubscriber:
    def __init__(self, on_telemetry: Callable[[TelemetryPayload], bool]) -> None:
        self._on_telemetry = on_telemetry
        self._host = os.getenv("MQTT_HOST", "127.0.0.1")
        self._port = int(os.getenv("MQTT_PORT", "1883"))
        self._topic = os.getenv(
            "MQTT_TELEMETRY_TOPIC",
            "hostelpulse/mess/mess-node-01/telemetry",
        )
        self._crowd_topic = os.getenv(
            "MQTT_CROWD_LEVEL_TOPIC",
            "hostelpulse/mess/mess-node-01/crowd-level",
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
            raw_payload = json.loads(message.payload.decode("utf-8"))
            payload = TelemetryPayload.model_validate(raw_payload)
            if self._on_telemetry(payload):
                self._publish_crowd_level(client, payload.occupancy)
        except (UnicodeDecodeError, json.JSONDecodeError, ValidationError, ValueError) as exc:
            print(
                f"Ignored invalid telemetry on {message.topic}; "
                f"received bytes={message.payload!r}; error={exc}",
                flush=True,
            )

    def _publish_crowd_level(self, client: mqtt.Client, occupancy: int) -> None:
        level = crowd_level(occupancy).upper()
        result = client.publish(self._crowd_topic, level, qos=1, retain=True)
        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            print(
                f"Published crowd level: {level} to {self._crowd_topic}",
                flush=True,
            )
        else:
            print(f"Could not publish crowd level: MQTT error {result.rc}", flush=True)
