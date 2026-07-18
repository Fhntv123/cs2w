# -*- coding: utf-8 -*-
"""
ida_bridge_plugin.py

Запусти в IDA: File -> Script file... -> этот файл.
Стартует HTTP-сервер на 127.0.0.1:8765 в отдельном потоке.
Все вызовы IDA API маршализуются в main-thread через ida_kernwin.execute_sync.
"""

import os
import threading
import traceback

# ------- IDA imports -------
import idaapi
import idautils
import idc
import ida_bytes
import ida_funcs
import ida_name
import ida_kernwin
import ida_xref
import ida_nalt
import ida_lines
import ida_hexrays
import ida_typeinf

# ------- Flask -------
try:
    from flask import Flask, request, jsonify
except ImportError:
    print("[ida_bridge] Flask not installed. Run in IDA:")
    print("  import subprocess, sys; subprocess.check_call([sys.executable,'-m','pip','install','flask'])")
    raise

PORT = int(os.environ.get("IDA_BRIDGE_PORT", "8765"))
HOST = os.environ.get("IDA_BRIDGE_HOST", "127.0.0.1")
TOKEN = os.environ.get("IDA_BRIDGE_TOKEN", "")

app = Flask(__name__)


# ---------- helpers ----------
def _parse_ea(val):
    if val is None:
        return idaapi.BADADDR
    if isinstance(val, int):
        return val
    s = str(val).strip()
    if s.startswith(("0x", "0X")):
        return int(s, 16)
    # если это имя — резолвим
    ea = ida_name.get_name_ea(idaapi.BADADDR, s)
    if ea != idaapi.BADADDR:
        return ea
    try:
        return int(s, 16)
    except ValueError:
        return idaapi.BADADDR


def in_main_thread(fn):
    """Декоратор: выполнить fn в main-thread IDA, вернуть результат."""
    def wrapper(*args, **kwargs):
        box = {}

        def target():
            try:
                box["result"] = fn(*args, **kwargs)
            except Exception as e:
                box["error"] = "".join(traceback.format_exception(type(e), e, e.__traceback__))
            return 1

        ida_kernwin.execute_sync(target, ida_kernwin.MFF_WRITE)
        if "error" in box:
            raise RuntimeError(box["error"])
        return box.get("result")
    wrapper.__name__ = fn.__name__
    return wrapper


@app.before_request
def _auth():
    if TOKEN and request.headers.get("X-Bridge-Token", "") != TOKEN:
        return jsonify({"error": "unauthorized"}), 401


@app.errorhandler(Exception)
def _err(e):
    return jsonify({"error": str(e), "trace": traceback.format_exc()}), 500


# ---------- endpoints ----------
@app.get("/health")
def health():
    return jsonify({"ok": True})


@in_main_thread
def _info():
    # Modern API (IDA 7.7+): ida_ida module
    try:
        import ida_ida
        procname = ida_ida.inf_get_procname()
        min_ea = ida_ida.inf_get_min_ea()
        max_ea = ida_ida.inf_get_max_ea()
        if ida_ida.inf_is_64bit():
            bits = 64
        elif ida_ida.inf_is_32bit_exactly():
            bits = 32
        else:
            bits = 16
    except Exception:
        # Legacy fallback
        info = idaapi.get_inf_structure()
        procname = info.procname
        min_ea = info.min_ea
        max_ea = info.max_ea
        bits = 64 if info.is_64bit() else (32 if info.is_32bit() else 16)
    return {
        "input_file": ida_nalt.get_input_file_path(),
        "input_file_name": ida_nalt.get_root_filename(),
        "processor": procname,
        "bits": bits,
        "min_ea": hex(min_ea),
        "max_ea": hex(max_ea),
    }

@app.get("/info")
def info():
    return jsonify(_info())


@in_main_thread
def _functions(q, limit):
    out = []
    for ea in idautils.Functions():
        name = ida_name.get_name(ea) or ""
        if q and q.lower() not in name.lower():
            continue
        out.append({"ea": hex(ea), "name": name})
        if len(out) >= limit:
            break
    return out

@app.get("/functions")
def functions():
    q = request.args.get("q", "")
    limit = int(request.args.get("limit", "500"))
    return jsonify(_functions(q, limit))


@in_main_thread
def _func(ea):
    f = ida_funcs.get_func(ea)
    if not f:
        return None
    return {
        "ea": hex(f.start_ea),
        "end_ea": hex(f.end_ea),
        "name": ida_name.get_name(f.start_ea),
        "size": f.size(),
        "flags": f.flags,
    }

@app.get("/func")
def func():
    ea = _parse_ea(request.args.get("ea") or request.args.get("name"))
    if ea == idaapi.BADADDR:
        return jsonify({"error": "bad ea/name"}), 400
    data = _func(ea)
    if not data:
        return jsonify({"error": "no function at ea"}), 404
    return jsonify(data)


@in_main_thread
def _decompile(ea):
    if not ida_hexrays.init_hexrays_plugin():
        raise RuntimeError("Hex-Rays decompiler not available")
    f = ida_funcs.get_func(ea)
    if not f:
        raise RuntimeError("no function at ea")
    cfunc = ida_hexrays.decompile(f)
    if not cfunc:
        raise RuntimeError("decompile failed")
    return {
        "ea": hex(f.start_ea),
        "name": ida_name.get_name(f.start_ea),
        "pseudocode": str(cfunc),
    }

@app.get("/decompile")
def decompile():
    ea = _parse_ea(request.args.get("ea") or request.args.get("name"))
    if ea == idaapi.BADADDR:
        return jsonify({"error": "bad ea/name"}), 400
    return jsonify(_decompile(ea))


@in_main_thread
def _disasm(ea, count):
    lines = []
    cur = ea
    for _ in range(count):
        if cur == idaapi.BADADDR:
            break
        line = ida_lines.generate_disasm_line(cur, 0) or ""
        lines.append({"ea": hex(cur), "text": ida_lines.tag_remove(line)})
        nxt = idc.next_head(cur)
        if nxt <= cur:
            break
        cur = nxt
    return lines

@app.get("/disasm")
def disasm():
    ea = _parse_ea(request.args.get("ea") or request.args.get("name"))
    count = int(request.args.get("count", "40"))
    if ea == idaapi.BADADDR:
        return jsonify({"error": "bad ea/name"}), 400
    return jsonify(_disasm(ea, count))


@in_main_thread
def _xrefs_to(ea):
    out = []
    for x in idautils.XrefsTo(ea, 0):
        out.append({"frm": hex(x.frm), "to": hex(x.to), "type": x.type, "iscode": bool(x.iscode)})
    return out

@app.get("/xrefs_to")
def xrefs_to():
    ea = _parse_ea(request.args.get("ea"))
    if ea == idaapi.BADADDR:
        return jsonify({"error": "bad ea"}), 400
    return jsonify(_xrefs_to(ea))


@in_main_thread
def _xrefs_from(ea):
    out = []
    for x in idautils.XrefsFrom(ea, 0):
        out.append({"frm": hex(x.frm), "to": hex(x.to), "type": x.type, "iscode": bool(x.iscode)})
    return out

@app.get("/xrefs_from")
def xrefs_from():
    ea = _parse_ea(request.args.get("ea"))
    if ea == idaapi.BADADDR:
        return jsonify({"error": "bad ea"}), 400
    return jsonify(_xrefs_from(ea))


@in_main_thread
def _strings(q, limit):
    out = []
    for s in idautils.Strings():
        text = str(s)
        if q and q.lower() not in text.lower():
            continue
        out.append({"ea": hex(s.ea), "length": s.length, "text": text})
        if len(out) >= limit:
            break
    return out

@app.get("/strings")
def strings():
    q = request.args.get("q", "")
    limit = int(request.args.get("limit", "200"))
    return jsonify(_strings(q, limit))


@in_main_thread
def _bytes(ea, size):
    data = ida_bytes.get_bytes(ea, size) or b""
    return {"ea": hex(ea), "size": len(data), "hex": data.hex()}

@app.get("/bytes")
def read_bytes():
    ea = _parse_ea(request.args.get("ea"))
    size = int(request.args.get("size", "64"))
    if ea == idaapi.BADADDR:
        return jsonify({"error": "bad ea"}), 400
    return jsonify(_bytes(ea, size))


@in_main_thread
def _name_at(ea):
    return {"ea": hex(ea), "name": ida_name.get_name(ea) or ""}

@app.get("/name_at")
def name_at():
    ea = _parse_ea(request.args.get("ea"))
    if ea == idaapi.BADADDR:
        return jsonify({"error": "bad ea"}), 400
    return jsonify(_name_at(ea))


@in_main_thread
def _rename(ea, name):
    ok = ida_name.set_name(ea, name, ida_name.SN_NOCHECK | ida_name.SN_NOWARN)
    return {"ea": hex(ea), "name": name, "ok": bool(ok)}

@app.post("/rename")
def rename():
    body = request.get_json(force=True) or {}
    ea = _parse_ea(body.get("ea"))
    name = body.get("name")
    if ea == idaapi.BADADDR or not name:
        return jsonify({"error": "need ea and name"}), 400
    return jsonify(_rename(ea, name))


@in_main_thread
def _comment(ea, text, repeatable):
    ok = idc.set_cmt(ea, text, 1 if repeatable else 0)
    return {"ea": hex(ea), "ok": bool(ok)}

@app.post("/comment")
def comment():
    body = request.get_json(force=True) or {}
    ea = _parse_ea(body.get("ea"))
    text = body.get("text", "")
    repeatable = bool(body.get("repeatable", False))
    if ea == idaapi.BADADDR:
        return jsonify({"error": "bad ea"}), 400
    return jsonify(_comment(ea, text, repeatable))


@in_main_thread
def _set_type(ea, type_str):
    tif = ida_typeinf.tinfo_t()
    if not ida_typeinf.parse_decl(tif, None, type_str + ";", ida_typeinf.PT_SIL):
        raise RuntimeError("parse_decl failed")
    ok = ida_typeinf.apply_tinfo(ea, tif, ida_typeinf.TINFO_DEFINITE)
    return {"ea": hex(ea), "type": type_str, "ok": bool(ok)}

@app.post("/set_type")
def set_type():
    body = request.get_json(force=True) or {}
    ea = _parse_ea(body.get("ea"))
    type_str = body.get("type")
    if ea == idaapi.BADADDR or not type_str:
        return jsonify({"error": "need ea and type"}), 400
    return jsonify(_set_type(ea, type_str))


# ---------- server ----------
_server_thread = None

def _run():
    # werkzeug dev server, threaded — этого достаточно для локального моста
    app.run(host=HOST, port=PORT, threaded=True, use_reloader=False)

def start():
    global _server_thread
    if _server_thread and _server_thread.is_alive():
        print("[ida_bridge] already running")
        return
    _server_thread = threading.Thread(target=_run, daemon=True, name="ida_bridge")
    _server_thread.start()
    print("[ida_bridge] listening on http://{}:{}".format(HOST, PORT))
    if TOKEN:
        print("[ida_bridge] token auth enabled (X-Bridge-Token)")

start()
