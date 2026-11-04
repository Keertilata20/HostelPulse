import os


MODERATE_THRESHOLD = int(os.getenv("CROWD_MODERATE_THRESHOLD", "5"))
BUSY_THRESHOLD = int(os.getenv("CROWD_BUSY_THRESHOLD", "10"))
MESS_CAPACITY = int(os.getenv("MESS_CAPACITY", "80"))

if MODERATE_THRESHOLD < 0 or BUSY_THRESHOLD <= MODERATE_THRESHOLD or MESS_CAPACITY <= 0:
    raise RuntimeError(
        "Crowd thresholds must satisfy 0 <= CROWD_MODERATE_THRESHOLD "
        "< CROWD_BUSY_THRESHOLD, and MESS_CAPACITY must be positive."
    )


def crowd_level(occupancy: int) -> str:
    if occupancy >= BUSY_THRESHOLD:
        return "Busy"
    if occupancy >= MODERATE_THRESHOLD:
        return "Moderate"
    return "Low"
