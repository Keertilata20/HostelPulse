$ErrorActionPreference = "Stop"

$null = Read-Host "Copy the complete BotFather token to your clipboard, then press Enter here"
$botToken = (Get-Clipboard -Raw).Trim()

try {
    if ($botToken -notmatch '^\d+:[A-Za-z0-9_-]+$') {
        Write-Host "The clipboard does not contain a complete BotFather token. Copy the full token from BotFather, then run this helper again. The token was not displayed."
        return
    }

    $curl = Get-Command "curl.exe" -ErrorAction Stop
    $processInfo = New-Object System.Diagnostics.ProcessStartInfo
    $processInfo.FileName = $curl.Source
    $processInfo.Arguments = "--ipv4 --silent --show-error --max-time 20 --config -"
    $processInfo.UseShellExecute = $false
    $processInfo.RedirectStandardInput = $true
    $processInfo.RedirectStandardOutput = $true
    $processInfo.RedirectStandardError = $true

    $process = New-Object System.Diagnostics.Process
    $process.StartInfo = $processInfo
    $null = $process.Start()
    $requestUrl = "https://api.telegram.org/bot$botToken/getUpdates"
    $process.StandardInput.WriteLine("url = $requestUrl")
    $process.StandardInput.Close()
    $responseText = $process.StandardOutput.ReadToEnd()
    $curlError = $process.StandardError.ReadToEnd()
    $process.WaitForExit()

    if ($process.ExitCode -ne 0) {
        $curlError = $curlError.Replace($botToken, "[token hidden]")
        $curlError = $curlError.Replace($requestUrl, "[Telegram request URL hidden]")
        Write-Host "Telegram API request failed: $curlError"
        return
    }

    $response = $responseText | ConvertFrom-Json
    if (-not $response.ok) {
        Write-Host "Telegram did not accept the request: $($response.description)"
        return
    }
    $chats = @(
        $response.result |
            Where-Object { $_.message.chat.id } |
            ForEach-Object {
                [PSCustomObject]@{
                    ChatId = $_.message.chat.id
                    ChatType = $_.message.chat.type
                    Username = $_.message.chat.username
                }
            } |
            Sort-Object ChatId -Unique
    )

    if ($chats.Count -eq 0) {
        Write-Host "No chat found yet. Open the bot, send /start, then run this helper again."
    } else {
        $chats | Format-Table -AutoSize
        Write-Host "Copy the ChatId for your private bot chat into backend/.env. Keep the bot token secret."
    }
} catch {
    $httpResponse = $_.Exception.Response
    if ($null -ne $httpResponse -and $null -ne $httpResponse.StatusCode) {
        $statusCode = [int]$httpResponse.StatusCode
        Write-Host "Telegram returned HTTP $statusCode."
        if ($statusCode -eq 401) {
            Write-Host "Telegram rejected the token. Check that you copied the complete token from BotFather."
        } elseif ($statusCode -eq 409) {
            Write-Host "Telegram reports another getUpdates poller or a webhook is active. Stop the other poller or remove the webhook, then retry."
        } else {
            Write-Host "Check Telegram's response and try again. The token and request URL were not printed."
        }
    } else {
        $safeMessage = $_.Exception.Message
        if ($botToken) { $safeMessage = $safeMessage.Replace($botToken, "[token hidden]") }
        Write-Host "Could not reach Telegram. Connection detail: $safeMessage"
    }
} finally {
    if ($null -ne $process) { $process.Dispose() }
    Remove-Variable botToken, requestUrl, response, responseText, curlError, chats -ErrorAction SilentlyContinue
}
