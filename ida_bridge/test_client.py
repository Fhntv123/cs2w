"""
test_client.py — быстрый локальный smoke-test моста.
Запусти: python test_client.py
"""
import json
import urllib.request

BASE = "http://127.0.0.1:8765"

def get(path):
    with urllib.request.urlopen(BASE + path) as r:
        return json.loads(r.read().decode())

print("health:", get("/health"))
print("info:", get("/info"))
funcs = get("/functions?limit=5")
print("first 5 functions:", funcs)
if funcs:
    ea = funcs[0]["ea"]
    print("disasm of", ea, ":", get(f"/disasm?ea={ea}&count=10"))
