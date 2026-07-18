# Публичный доступ к IDA Bridge

Локально мост слушает `127.0.0.1:8765`. Чтобы Lindy мог его дергать — нужен
публичный туннель.

## Вариант A: ngrok (проще)
1. Установи ngrok: https://ngrok.com/download
2. Зарегистрируйся, получи authtoken, сделай `ngrok config add-authtoken <TOKEN>`
3. Запусти:
   ```
   ngrok http 8765
   ```
4. Скопируй `https://xxxx.ngrok-free.app` и отдай Lindy.

## Вариант B: cloudflared (бесплатно, без регистрации для quick tunnel)
1. Установи `cloudflared` (https://github.com/cloudflare/cloudflared/releases)
2. Запусти:
   ```
   cloudflared tunnel --url http://127.0.0.1:8765
   ```
3. Скопируй выданный `https://xxx.trycloudflare.com` URL.

## Безопасность
Оба варианта делают мост доступным из интернета. Обязательно:
- Задай токен: перед стартом IDA в терминале
  ```
  set IDA_BRIDGE_TOKEN=какой-нибудь-длинный-секрет     (Windows)
  export IDA_BRIDGE_TOKEN=какой-нибудь-длинный-секрет  (Linux/Mac)
  ```
- Передай этот же токен Lindy — он будет слать его в заголовке `X-Bridge-Token`.

Без токена любой, кто угадает URL, сможет читать/переименовывать в твоей IDA.
