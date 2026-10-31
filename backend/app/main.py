from contextlib import asynccontextmanager

from fastapi import FastAPI, Query
import psycopg

from . import database
from .mqtt_client import MqttSubscriber
from .telemetry import TelemetryPayload

subscriber: MqttSubscriber | None = None


def save_telemetry(payload: TelemetryPayload) -> None:
    try:
        saved = database.save_telemetry(payload.model_dump())
    except psycopg.Error as exc:
        print(f"Could not save telemetry to PostgreSQL: {exc}", flush=True)
        return
    print(f"Telemetry saved: device={saved['device_id']} event={saved['event']} occupancy={saved['occupancy']}", flush=True)


@asynccontextmanager
async def lifespan(app: FastAPI):
    global subscriber
    database.initialize_database()
    subscriber = MqttSubscriber(on_telemetry=save_telemetry)
    subscriber.start()
    yield
    subscriber.stop()


app = FastAPI(title="HostelPulse API", lifespan=lifespan)


@app.get("/health")
def health() -> dict[str, str]:
    with database.connect() as connection:
        connection.execute("SELECT 1")
    return {"status": "ok", "database": "connected"}


@app.get("/telemetry/latest")
def get_latest_telemetry() -> dict | None:
    return database.get_latest_telemetry()


@app.get("/occupancy/current")
def get_current_occupancy() -> list[dict]:
    return database.get_current_occupancy()


@app.get("/telemetry/events")
def get_telemetry_events(limit: int = Query(default=50, ge=1, le=500)) -> list[dict]:
    return database.get_telemetry_events(limit)
