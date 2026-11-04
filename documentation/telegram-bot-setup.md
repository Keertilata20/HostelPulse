# HostelPulse Telegram Bot Setup

The backend can send an occupancy update after it saves valid telemetry and respond to `/status` in Telegram. During the current demo, the backend receives generated MQTT events; the bot reports the telemetry it receives.

Crowd levels use `CROWD_MODERATE_THRESHOLD` and `CROWD_BUSY_THRESHOLD` from `backend/.env`. The demo defaults are 5 and 10 people. `MESS_CAPACITY` defaults to 80 as a demo value for the dashboard gauge. Telegram sends a crowd alert only when occupancy rises from below the Busy threshold to at or above it; it will not repeat the alert for every event while occupancy remains Busy. `/status` and the dashboard display the current level.

## 1. Create the bot

1. In Telegram, open [@BotFather](https://t.me/BotFather).
2. Send `/newbot` and follow its prompts.
3. Save the bot token privately. Anyone with the token can control the bot; do not commit it or share it in screenshots or chat.
4. Open the new bot's `t.me` link and press **Start**, then send `/start`.

## 2. Find your chat ID

You already sent `/start` to the bot. Copy the complete token from BotFather to the clipboard. From the project root in PowerShell, run:

```powershell
.\backend\get_telegram_chat_id.ps1
```

Press Enter at the prompt; the helper reads the token from the clipboard without displaying it. It prints chat IDs from recent bot messages without showing the token or message text. Copy the `ChatId` for your private bot chat. If it reports no chat, send `/start` to the bot again and rerun the helper. Do not run the backend bot polling yet, because Telegram's `getUpdates` polling must not be active in two places at once.

## 3. Configure the backend

Add these lines to `backend/.env`, preserving the existing `DATABASE_URL`:

```dotenv
TELEGRAM_BOT_TOKEN=your_bot_token
TELEGRAM_CHAT_ID=your_chat_id
```

`backend/.env` is excluded from Git. Do not put your real credentials in `.env.example` or source files.

## 4. Run and use it

Restart the backend after Mosquitto is running. With both Telegram settings present and the laptop online, the backend logs `Telegram bot started` and sends a connection message to the configured chat.

- Send `/status` to receive the latest occupancy, crowd level, entry and exit totals, and the most recent event with its received time (India time).
- Send `/start` or `/help` to see the command hint.
- Publish a valid MQTT telemetry message; after it is saved, the bot sends an occupancy update.

The bot only handles commands from the configured chat ID, which can be a private chat or a group where the bot is present. The backend uses Telegram's HTTPS Bot API and long polling, so the laptop needs outbound internet access; no public webhook URL is required. See the official [Telegram Bot API](https://core.telegram.org/bots/api) and [bot setup guide](https://core.telegram.org/bots/features).
