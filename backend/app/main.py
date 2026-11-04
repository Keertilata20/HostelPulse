from contextlib import asynccontextmanager
from pathlib import Path

from fastapi import FastAPI, Query
from fastapi.staticfiles import StaticFiles
import psycopg

from . import database
from .crowd import BUSY_THRESHOLD, MODERATE_THRESHOLD, crowd_level
from .mqtt_client import MqttSubscriber
from .telegram_bot import TelegramBot
from .telemetry import TelemetryPayload

subscriber: MqttSubscriber | None = None
telegram_bot: TelegramBot | None = None


def save_telemetry(payload: TelemetryPayload) -> None:
    try:
        current_rows = database.get_current_occupancy()
        previous = next(
            (row for row in current_rows if row["device_id"] == payload.device_id),
            None,
        )
        previous_occupancy = previous["occupancy"] if previous else None
        saved = database.save_telemetry(payload.model_dump())
    except psycopg.Error as exc:
        print(f"Could not save telemetry to PostgreSQL: {exc}", flush=True)
        return
    print(f"Telemetry saved: device={saved['device_id']} event={saved['event']} occupancy={saved['occupancy']}", flush=True)
    if telegram_bot is not None:
        telegram_bot.notify_telemetry(saved, previous_occupancy)


@asynccontextmanager
async def lifespan(app: FastAPI):
    global subscriber, telegram_bot
    database.initialize_database()
    telegram_bot = TelegramBot()
    telegram_bot.start()
    subscriber = MqttSubscriber(on_telemetry=save_telemetry)
    subscriber.start()
    yield
    subscriber.stop()
    telegram_bot.stop()


app = FastAPI(title="HostelPulse API", lifespan=lifespan)

frontend_dir = Path(__file__).resolve().parents[2] / "frontend"


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


@app.get("/crowd/status")
def get_crowd_status() -> dict:
    current_rows = database.get_current_occupancy()
    current = next(
        (row for row in current_rows if row["device_id"] == "mess-node-01"),
        None,
    )
    occupancy = current["occupancy"] if current else 0
    return {
        "device_id": "mess-node-01",
        "occupancy": occupancy,
        "level": crowd_level(occupancy),
        "moderate_threshold": MODERATE_THRESHOLD,
        "busy_threshold": BUSY_THRESHOLD,
    }


app.mount("/", StaticFiles(directory=frontend_dir, html=True), name="frontend")
