import json
import os
from contextlib import asynccontextmanager
from typing import Any

from fastapi import FastAPI

from .mqtt_client import MqttSubscriber

latest_telemetry: dict[str, Any] | None = None
subscriber: MqttSubscriber | None = None


def save_telemetry(payload: dict[str, Any]) -> None:
    global latest_telemetry
    latest_telemetry = payload
    print(f"Telemetry received: {json.dumps(payload)}", flush=True)


@asynccontextmanager
async def lifespan(app: FastAPI):
    global subscriber
    subscriber = MqttSubscriber(on_telemetry=save_telemetry)
    subscriber.start()
    yield
    subscriber.stop()


app = FastAPI(title="HostelPulse API", lifespan=lifespan)


@app.get("/health")
def health() -> dict[str, str]:
    return {"status": "ok"}


@app.get("/telemetry/latest")
def get_latest_telemetry() -> dict[str, Any] | None:
    return latest_telemetry
