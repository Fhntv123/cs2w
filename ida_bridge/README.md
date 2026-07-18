# IDA Bridge

Тонкий HTTP-мост поверх IDA Pro (idapython). Позволяет удалённо (в т.ч. из Lindy)
вызывать функции анализа: декомпиляцию, xrefs, переименование, чтение байтов и т.д.

## Как это работает
1. Открываешь свой .idb / бинарь в IDA Pro.
2. В IDA: `File → Script file...` → выбираешь `ida_bridge_plugin.py`.
3. Плагин стартует Flask-сервер на `127.0.0.1:8765` в отдельном потоке.
4. Для внешнего доступа поднимаешь ngrok / cloudflared, публичный URL даёшь Lindy.
5. Lindy дергает HTTP-эндпоинты — они внутри идут в main-thread IDA через `execute_sync`.

## Запуск

### 1. Установи зависимости (в python, который использует IDA)
IDA обычно использует свой python (см. `File → Script file` — тот же интерпретатор).
Открой в IDA `File → Script command...` и выполни:

```python
import subprocess, sys
subprocess.check_call([sys.executable, "-m", "pip", "install", "flask"])
```

Если ругается на права — запусти IDA от админа один раз для установки.

### 2. Загрузи бинарь в IDA, дождись авто-анализа.

### 3. Запусти плагин
`File → Script file...` → `ida_bridge/ida_bridge_plugin.py`

В консоли IDA должно появиться:
```
[ida_bridge] listening on http://127.0.0.1:8765
```

### 4. Проверь локально
```
curl http://127.0.0.1:8765/health
curl http://127.0.0.1:8765/info
```

### 5. Публичный URL (ngrok)
```
ngrok http 8765
```
Получишь URL вида `https://xxxx-xx-xx-xx-xx.ngrok-free.app` — отдай его Lindy.

## Эндпоинты

| Метод | Путь | Параметры | Что делает |
|---|---|---|---|
| GET  | `/health` | — | ping |
| GET  | `/info` | — | путь к базе, битность, процессор |
| GET  | `/functions` | `q` (опц. подстрока), `limit` | список функций |
| GET  | `/func` | `ea` или `name` | инфо о функции |
| GET  | `/decompile` | `ea` или `name` | Hex-Rays декомпиляция (нужен декомпилер) |
| GET  | `/disasm` | `ea`, `count` (строк) | листинг ассемблера |
| GET  | `/xrefs_to` | `ea` | xrefs на адрес |
| GET  | `/xrefs_from` | `ea` | xrefs из адреса |
| GET  | `/strings` | `q`, `limit` | поиск строк |
| GET  | `/bytes` | `ea`, `size` | чтение сырых байт (hex) |
| GET  | `/name_at` | `ea` | имя по адресу |
| POST | `/rename` | JSON `{"ea": "0x...", "name": "..."}` | переименовать |
| POST | `/comment` | JSON `{"ea": "0x...", "text": "...", "repeatable": bool}` | комментарий |
| POST | `/set_type` | JSON `{"ea": "...", "type": "int __fastcall f(int a)"}` | задать прототип |

## Безопасность
Мост слушает `127.0.0.1`, наружу его показывает только ngrok/cloudflared с твоего
согласия. Токен-заголовок опционально: задай env `IDA_BRIDGE_TOKEN=xxx` перед стартом
IDA — тогда все запросы должны иметь `X-Bridge-Token: xxx`.
