# HostelPulse Telegram Bot Setup

The backend can send an occupancy update after it saves valid telemetry and respond to `/status` in Telegram. Notifications are labeled as simulated for the current demo.

## 1. Create the bot

1. In Telegram, open [@BotFather](https://t.me/BotFather).
2. Send `/newbot` and follow its prompts.
3. Save the bot token privately. Anyone with the token can control the bot; do not commit it or share it in screenshots or chat.
4. Open the new bot's `t.me` link and press **Start**, then send `/start`.

## 2. Find your chat ID

You already sent `/start` to the bot. From the project root in PowerShell, run:

```powershell
.\backend\get_telegram_chat_id.ps1
```

Enter the BotFather token at the hidden prompt. The helper prints chat IDs from recent bot messages without displaying the token or message text. Copy the `ChatId` for your private bot chat. If it reports no chat, send `/start` to the bot again and rerun the helper. Do not run the backend bot polling yet, because Telegram's `getUpdates` polling must not be active in two places at once.

## 3. Configure the backend

Add these lines to `backend/.env`, preserving the existing `DATABASE_URL`:

```dotenv
TELEGRAM_BOT_TOKEN=your_bot_token
TELEGRAM_CHAT_ID=your_chat_id
```

`backend/.env` is excluded from Git. Do not put your real credentials in `.env.example` or source files.

## 4. Run and use it

Restart the backend after Mosquitto is running. With both Telegram settings present and the laptop online, the backend logs `Telegram bot started` and sends a connection message to the configured chat.

- Send `/status` to receive the latest occupancy, entries, and exits from PostgreSQL.
- Send `/start` or `/help` to see the command hint.
- Publish a valid MQTT telemetry message; after it is saved, the bot sends an occupancy update.

The bot only handles commands from the configured chat ID. The backend uses Telegram's HTTPS Bot API and long polling, so the laptop needs outbound internet access; no public webhook URL is required. See the official [Telegram Bot API](https://core.telegram.org/bots/api) and [bot setup guide](https://core.telegram.org/bots/features).
