import json
import os
import threading
from zoneinfo import ZoneInfo
from urllib.error import HTTPError, URLError
from urllib.request import Request, urlopen

from . import database
from .crowd import BUSY_THRESHOLD, MESS_CAPACITY, MODERATE_THRESHOLD, crowd_level


class TelegramBot:
    """Optional Telegram notifications and /status command for HostelPulse."""

    def __init__(self) -> None:
        self._token = os.getenv("TELEGRAM_BOT_TOKEN", "").strip()
        self._chat_id = os.getenv("TELEGRAM_CHAT_ID", "").strip()
        self._enabled = bool(self._token and self._chat_id)
        self._stop_event = threading.Event()
        self._thread: threading.Thread | None = None
        self._offset = 0

    def start(self) -> None:
        if not self._enabled:
            print("Telegram bot disabled; set TELEGRAM_BOT_TOKEN and TELEGRAM_CHAT_ID in backend/.env", flush=True)
            return

        self._thread = threading.Thread(target=self._poll_updates, name="telegram-bot", daemon=True)
        self._thread.start()
        self.send_message_async("HostelPulse bot connected. Send /status for current occupancy.")
        print("Telegram bot started", flush=True)

    def stop(self) -> None:
        self._stop_event.set()
        if self._thread is not None:
            self._thread.join(timeout=2)

    def notify_telemetry(self, event: dict, previous_occupancy: int | None) -> None:
        if not self._enabled:
            return
        occupancy = event["occupancy"]
        if occupancy < BUSY_THRESHOLD or (
            previous_occupancy is not None and previous_occupancy >= BUSY_THRESHOLD
        ):
            return
        message = (
            "⚠️ HostelPulse crowd alert (demo)\n"
            f"Event: {event['event']}\n"
            f"Crowd level: {crowd_level(occupancy)}\n"
            f"People inside: {occupancy} (Busy threshold: {BUSY_THRESHOLD})\n"
            f"Entries: {event['entries']}  |  Exits: {event['exits']}"
        )
        self.send_message_async(message)

    def send_message_async(self, message: str) -> None:
        threading.Thread(
            target=self._send_message,
            args=(message,),
            name="telegram-send",
            daemon=True,
        ).start()

    def _api_request(self, method: str, data: dict, timeout: float = 20) -> dict:
        url = f"https://api.telegram.org/bot{self._token}/{method}"
        body = json.dumps(data).encode("utf-8")
        request = Request(url, data=body, headers={"Content-Type": "application/json"})
        with urlopen(request, timeout=timeout) as response:
            result = json.loads(response.read().decode("utf-8"))
        if not result.get("ok"):
            raise RuntimeError("Telegram API returned an unsuccessful response")
        return result

    def _send_message(self, message: str) -> None:
        try:
            self._api_request("sendMessage", {"chat_id": self._chat_id, "text": message}, timeout=10)
        except (HTTPError, URLError, TimeoutError, OSError, ValueError, RuntimeError) as exc:
            print(f"Telegram send failed ({type(exc).__name__})", flush=True)

    def _poll_updates(self) -> None:
        while not self._stop_event.is_set():
            try:
                result = self._api_request(
                    "getUpdates",
                    {
                        "offset": self._offset,
                        "timeout": 15,
                        "allowed_updates": ["message"],
                    },
                    timeout=20,
                )
                for update in result.get("result", []):
                    self._offset = max(self._offset, int(update["update_id"]) + 1)
                    self._handle_update(update)
            except (HTTPError, URLError, TimeoutError, OSError, ValueError, RuntimeError, KeyError) as exc:
                if not self._stop_event.is_set():
                    print(f"Telegram polling failed ({type(exc).__name__}); retrying", flush=True)
                    self._stop_event.wait(3)

    def _handle_update(self, update: dict) -> None:
        message = update.get("message", {})
        chat_id = str(message.get("chat", {}).get("id", ""))
        if chat_id != self._chat_id:
            return

        text = str(message.get("text", "")).strip()
        command = text.split(maxsplit=1)[0].split("@", 1)[0].lower() if text else ""
        if command in {"/start", "/help"}:
            self.send_message_async("HostelPulse demo bot is ready. Use /status to view current occupancy.")
        elif command == "/status":
            self.send_message_async(self._status_message())

    def _status_message(self) -> str:
        rows = database.get_current_occupancy()
        row = next((item for item in rows if item["device_id"] == "mess-node-01"), None)
        if row is None:
            return "🧪 HostelPulse demo: no occupancy data received yet."
        latest = database.get_latest_telemetry()
        if latest is not None and latest["device_id"] == row["device_id"]:
            received_at = latest["received_at"].astimezone(ZoneInfo("Asia/Kolkata"))
            recent_event = (
                f"Last event: {latest['event']} at "
                f"{received_at.strftime('%d %b %Y, %I:%M:%S %p IST')}"
            )
        else:
            recent_event = "Last event: none recorded"
        return (
            "HostelPulse occupancy status (demo)\n"
            f"People inside: {row['occupancy']} of {MESS_CAPACITY} demo seats\n"
            f"Crowd level: {crowd_level(row['occupancy'])}\n"
            f"Moderate from {MODERATE_THRESHOLD}; Busy from {BUSY_THRESHOLD}\n"
            f"Entries: {row['entries']}\n"
            f"Exits: {row['exits']}\n"
            f"{recent_event}"
        )
