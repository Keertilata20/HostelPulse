import os
from pathlib import Path

import psycopg
from dotenv import load_dotenv
from psycopg.rows import dict_row

load_dotenv(Path(__file__).resolve().parents[1] / ".env")
DATABASE_URL = os.getenv("DATABASE_URL")

if not DATABASE_URL:
    raise RuntimeError("DATABASE_URL is required. Copy backend/.env.example to backend/.env and set it there.")


def connect() -> psycopg.Connection:
    return psycopg.connect(DATABASE_URL, row_factory=dict_row)


def initialize_database() -> None:
    with connect() as connection:
        connection.execute(
            """
            CREATE TABLE IF NOT EXISTS telemetry_events (
                id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
                schema_version SMALLINT NOT NULL CHECK (schema_version = 1),
                device_id TEXT NOT NULL,
                event TEXT NOT NULL CHECK (event IN ('ENTRY', 'EXIT')),
                entries INTEGER NOT NULL CHECK (entries >= 0),
                exits INTEGER NOT NULL CHECK (exits >= 0),
                occupancy INTEGER NOT NULL CHECK (occupancy >= 0),
                sensor_a SMALLINT NOT NULL CHECK (sensor_a IN (0, 1)),
                sensor_b SMALLINT NOT NULL CHECK (sensor_b IN (0, 1)),
                uptime_ms BIGINT NOT NULL CHECK (uptime_ms >= 0),
                received_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
            )
            """
        )
        connection.execute(
            """
            CREATE TABLE IF NOT EXISTS current_occupancy (
                device_id TEXT PRIMARY KEY,
                occupancy INTEGER NOT NULL CHECK (occupancy >= 0),
                entries INTEGER NOT NULL CHECK (entries >= 0),
                exits INTEGER NOT NULL CHECK (exits >= 0),
                event TEXT NOT NULL CHECK (event IN ('ENTRY', 'EXIT')),
                updated_at TIMESTAMPTZ NOT NULL
            )
            """
        )


def save_telemetry(payload: dict) -> dict:
    with connect() as connection:
        event = connection.execute(
            """
            INSERT INTO telemetry_events (
                schema_version, device_id, event, entries, exits, occupancy,
                sensor_a, sensor_b, uptime_ms
            ) VALUES (
                %(schema_version)s, %(device_id)s, %(event)s, %(entries)s, %(exits)s,
                %(occupancy)s, %(sensor_a)s, %(sensor_b)s, %(uptime_ms)s
            )
            RETURNING id, schema_version, device_id, event, entries, exits,
                      occupancy, sensor_a, sensor_b, uptime_ms, received_at
            """,
            payload,
        ).fetchone()
        connection.execute(
            """
            INSERT INTO current_occupancy (device_id, occupancy, entries, exits, event, updated_at)
            VALUES (%(device_id)s, %(occupancy)s, %(entries)s, %(exits)s, %(event)s, %(received_at)s)
            ON CONFLICT (device_id) DO UPDATE SET
                occupancy = EXCLUDED.occupancy,
                entries = EXCLUDED.entries,
                exits = EXCLUDED.exits,
                event = EXCLUDED.event,
                updated_at = EXCLUDED.updated_at
            """,
            {**payload, "received_at": event["received_at"]},
        )
    return event


def get_latest_telemetry() -> dict | None:
    with connect() as connection:
        return connection.execute(
            "SELECT * FROM telemetry_events ORDER BY id DESC LIMIT 1"
        ).fetchone()


def get_current_occupancy() -> list[dict]:
    with connect() as connection:
        return connection.execute(
            "SELECT * FROM current_occupancy ORDER BY device_id"
        ).fetchall()


def get_telemetry_events(limit: int) -> list[dict]:
    with connect() as connection:
        return connection.execute(
            "SELECT * FROM telemetry_events ORDER BY id DESC LIMIT %s", (limit,)
        ).fetchall()
