from typing import Literal

from pydantic import BaseModel, ConfigDict, Field


class TelemetryPayload(BaseModel):
    model_config = ConfigDict(extra="forbid", strict=True)

    schema_version: Literal[1]
    device_id: str = Field(min_length=1, max_length=100)
    event: Literal["ENTRY", "EXIT"]
    entries: int = Field(ge=0)
    exits: int = Field(ge=0)
    occupancy: int = Field(ge=0)
    sensor_a: Literal[0, 1]
    sensor_b: Literal[0, 1]
    uptime_ms: int = Field(ge=0)
