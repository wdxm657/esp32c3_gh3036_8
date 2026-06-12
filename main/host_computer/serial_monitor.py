#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ESP32-C3 GH3036 串口日志上位机
功能：
  - 实时串口连接/断开
  - 解析 ADT / HR / HRV 日志并分窗显示
  - 原始串口数据日志窗口
"""

import tkinter as tk
from tkinter import ttk, scrolledtext
import threading
import re
import time
import datetime
import queue
from typing import Optional

try:
    import serial
    import serial.tools.list_ports
except ImportError:
    serial = None


# ──────────────────────────────────────────────
#  常量
# ──────────────────────────────────────────────
DEFAULT_BAUD = 115200
DEFAULT_PORT = "COM14"

# 解析正则
RE_ADT = re.compile(
    r"\[ADT\]\s*wear_status:\s*(-?\d+),\s*det_status:\s*(-?\d+),\s*ctr:\s*(-?\d+)"
)
RE_HR = re.compile(
    r"\[HR\]\s*flag:\s*(-?\d+),\s*out:\s*(-?\d+),\s*snr:\s*(-?\d+),"
    r"\s*level:\s*(-?\d+),\s*score:\s*(-?\d+),\s*acc:\s*(-?\d+),"
    r"\s*scence:\s*(-?\d+)"
)
RE_HRV = re.compile(
    r"\[HRV\]\s*rri0:\s*(-?\d+),\s*rri1:\s*(-?\d+),\s*rri2:\s*(-?\d+),"
    r"\s*rri3:\s*(-?\d+),\s*confi:\s*(-?\d+),\s*num:\s*(-?\d+)"
)


# ──────────────────────────────────────────────
#  串口读取线程
# ──────────────────────────────────────────────
class SerialReader(threading.Thread):
    """后台线程：持续读取串口数据并放入队列"""

    def __init__(self, ser: serial.Serial, data_queue: queue.Queue):
        super().__init__(daemon=True)
        self.ser = ser
        self.queue = data_queue
        self._running = True

    def run(self):
        buf = b""
        while self._running:
            try:
                # 一次读取最多 1024 字节，利用 timeout=0.1 避免忙等
                # 相比先查 in_waiting 再读，这种方式消除了竞态条件
                chunk = self.ser.read(1024)
                if chunk:
                    buf += chunk
                    # 按行切分
                    while b"\n" in buf:
                        line, buf = buf.split(b"\n", 1)
                        text = line.decode("utf-8", errors="replace").strip("\r")
                        if text:
                            self.queue.put(text)
                else:
                    time.sleep(0.01)
            except (serial.SerialException, OSError):
                self.queue.put("__DISCONNECTED__")
                break
            except Exception:
                break

    def stop(self):
        self._running = False


# ──────────────────────────────────────────────
#  主应用
# ──────────────────────────────────────────────
class SerialMonitorApp:
    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title("ESP32-C3 GH3036 串口监控器")
        self.root.geometry("1000x700")
        self.root.minsize(800, 500)

        if serial is None:
            tk.messagebox.showerror(
                "缺少依赖",
                "未找到 pyserial 库。\n请运行: pip install pyserial",
            )
            root.destroy()
            return

        # 状态
        self.ser: Optional[serial.Serial] = None
        self.reader: Optional[SerialReader] = None
        self.data_queue: queue.Queue = queue.Queue()
        self.rx_count = 0
        self._after_id = None

        # ── 界面搭建 ──
        self._build_connection_panel()
        self._build_parsed_panels()
        self._build_raw_log_panel()
        self._build_statusbar()

        # 定时轮询队列
        self._poll_queue()

        # 窗口关闭安全退出
        self.root.protocol("WM_DELETE_WINDOW", self._on_close)

    # ──────────────── 界面部件 ────────────────

    def _build_connection_panel(self):
        frame = ttk.LabelFrame(self.root, text="串口连接", padding=8)
        frame.pack(fill=tk.X, padx=8, pady=(8, 2))

        ttk.Label(frame, text="端口:").grid(row=0, column=0, padx=(0, 4))
        self.port_var = tk.StringVar(value=DEFAULT_PORT)
        self.port_combo = ttk.Combobox(
            frame, textvariable=self.port_var, width=14, state="readonly"
        )
        self.port_combo.grid(row=0, column=1, padx=(0, 12))
        self._refresh_ports()
        self._auto_select_default()

        ttk.Label(frame, text="波特率:").grid(row=0, column=2, padx=(0, 4))
        self.baud_var = tk.StringVar(value=str(DEFAULT_BAUD))
        self.baud_combo = ttk.Combobox(
            frame,
            textvariable=self.baud_var,
            values=["9600", "19200", "38400", "57600", "115200", "230400", "460800", "921600"],
            width=10,
            state="readonly",
        )
        self.baud_combo.grid(row=0, column=3, padx=(0, 12))

        self.btn_connect = ttk.Button(
            frame, text="🔌 连接", width=12, command=self._toggle_connection
        )
        self.btn_connect.grid(row=0, column=4, padx=(0, 8))

        ttk.Button(frame, text="刷新端口", command=self._refresh_ports).grid(
            row=0, column=5, padx=(0, 8)
        )
        ttk.Button(frame, text="清空原始日志", command=self._clear_raw).grid(
            row=0, column=6, padx=(0, 8)
        )
        ttk.Button(frame, text="清空解析数据", command=self._clear_parsed).grid(
            row=0, column=7
        )

        # 列权重
        frame.columnconfigure(8, weight=1)

    def _build_parsed_panels(self):
        frame = ttk.LabelFrame(self.root, text="解析数据（实时）", padding=8)
        frame.pack(fill=tk.BOTH, expand=False, padx=8, pady=4)

        # 三列等宽
        frame.columnconfigure(0, weight=1, uniform="col")
        frame.columnconfigure(1, weight=1, uniform="col")
        frame.columnconfigure(2, weight=1, uniform="col")

        # ── ADT ──
        self._adt_frame = ttk.LabelFrame(frame, text="ADT（佩戴检测）", padding=6)
        self._adt_frame.grid(row=0, column=0, sticky="nsew", padx=(0, 4))
        self._adt_vars = {
            "wear_status": tk.StringVar(value="--"),
            "det_status": tk.StringVar(value="--"),
            "ctr": tk.StringVar(value="--"),
        }
        _make_kv_grid(self._adt_frame, [
            ("佩戴状态 (wear_status)", self._adt_vars["wear_status"]),
            ("检测状态 (det_status)", self._adt_vars["det_status"]),
            ("计数器 (ctr)", self._adt_vars["ctr"]),
        ])

        # ── HR ──
        self._hr_frame = ttk.LabelFrame(frame, text="HR（心率）", padding=6)
        self._hr_frame.grid(row=0, column=1, sticky="nsew", padx=4)
        self._hr_vars = {
            "flag": tk.StringVar(value="--"),
            "out": tk.StringVar(value="--"),
            "snr": tk.StringVar(value="--"),
            "level": tk.StringVar(value="--"),
            "score": tk.StringVar(value="--"),
            "acc": tk.StringVar(value="--"),
            "scence": tk.StringVar(value="--"),
        }
        _make_kv_grid(self._hr_frame, [
            ("有效标志 (flag)", self._hr_vars["flag"]),
            ("心率值 (out, bpm)", self._hr_vars["out"]),
            ("信噪比 (snr)", self._hr_vars["snr"]),
            ("有效等级 (level)", self._hr_vars["level"]),
            ("有效评分 (score)", self._hr_vars["score"]),
            ("加速度 (acc)", self._hr_vars["acc"]),
            ("场景 (scence)", self._hr_vars["scence"]),
        ])

        # ── HRV ──
        self._hrv_frame = ttk.LabelFrame(frame, text="HRV（心率变异性）", padding=6)
        self._hrv_frame.grid(row=0, column=2, sticky="nsew", padx=(4, 0))
        self._hrv_vars = {
            "rri0": tk.StringVar(value="--"),
            "rri1": tk.StringVar(value="--"),
            "rri2": tk.StringVar(value="--"),
            "rri3": tk.StringVar(value="--"),
            "confi": tk.StringVar(value="--"),
            "num": tk.StringVar(value="--"),
        }
        _make_kv_grid(self._hrv_frame, [
            ("RRI[0] (ms)", self._hrv_vars["rri0"]),
            ("RRI[1] (ms)", self._hrv_vars["rri1"]),
            ("RRI[2] (ms)", self._hrv_vars["rri2"]),
            ("RRI[3] (ms)", self._hrv_vars["rri3"]),
            ("置信度 (confi)", self._hrv_vars["confi"]),
            ("有效RRI数 (num)", self._hrv_vars["num"]),
        ])

    def _build_raw_log_panel(self):
        frame = ttk.LabelFrame(self.root, text="原始串口日志", padding=8)
        frame.pack(fill=tk.BOTH, expand=True, padx=8, pady=(2, 4))

        self.raw_text = scrolledtext.ScrolledText(
            frame,
            wrap=tk.WORD,
            font=("Consolas", 10),
            bg="#1e1e1e",
            fg="#d4d4d4",
            insertbackground="white",
            state=tk.DISABLED,
        )
        self.raw_text.pack(fill=tk.BOTH, expand=True)

        # 快捷键 Ctrl+A 全选
        self.raw_text.bind("<Control-a>", self._select_all_raw)

    def _build_statusbar(self):
        self.status_var = tk.StringVar(value="就绪 — 点击「连接」开始监控")
        bar = ttk.Label(
            self.root,
            textvariable=self.status_var,
            relief=tk.SUNKEN,
            anchor=tk.W,
            padding=(6, 2),
        )
        bar.pack(fill=tk.X, side=tk.BOTTOM, padx=8, pady=(0, 4))

    # ──────────────── 动作 ────────────────

    def _refresh_ports(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.port_combo["values"] = ports
        self._auto_select_default()

    def _auto_select_default(self):
        values = self.port_combo["values"]
        if values:
            if DEFAULT_PORT in values:
                self.port_var.set(DEFAULT_PORT)
            else:
                self.port_var.set(values[0])

    def _toggle_connection(self):
        if self.ser and self.ser.is_open:
            self._disconnect()
        else:
            self._connect()

    def _connect(self):
        port = self.port_var.get().strip()
        try:
            baud = int(self.baud_var.get())
        except ValueError:
            baud = DEFAULT_BAUD

        try:
            self.ser = serial.Serial(
                port=port,
                baudrate=baud,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=0.1,
            )
        except Exception as e:
            tk.messagebox.showerror("连接失败", f"无法打开 {port}:\n{e}")
            return

        self.rx_count = 0
        self.reader = SerialReader(self.ser, self.data_queue)
        self.reader.start()

        self.btn_connect.configure(text="🔌 断开", style="")
        self.port_combo.configure(state=tk.DISABLED)
        self.baud_combo.configure(state=tk.DISABLED)
        self._append_raw(f"[{_ts()}] >>> 已连接 {port} @ {baud} baud\n")
        self._update_status()

    def _disconnect(self):
        port = getattr(self.ser, "port", "?") if self.ser else "?"
        if self.reader:
            self.reader.stop()
            self.reader = None
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.ser = None

        self.btn_connect.configure(text="🔌 连接")
        self.port_combo.configure(state="readonly")
        self.baud_combo.configure(state="readonly")
        self._append_raw(f"[{_ts()}] >>> 已断开 {port}\n")
        self._update_status()

    def _clear_raw(self):
        self.raw_text.configure(state=tk.NORMAL)
        self.raw_text.delete("1.0", tk.END)
        self.raw_text.configure(state=tk.DISABLED)

    def _clear_parsed(self):
        for var in self._adt_vars.values():
            var.set("--")
        for var in self._hr_vars.values():
            var.set("--")
        for var in self._hrv_vars.values():
            var.set("--")
        self._append_raw(f"[{_ts()}] >>> 解析数据已清空\n")

    def _select_all_raw(self, event=None):
        self.raw_text.configure(state=tk.NORMAL)
        self.raw_text.tag_add(tk.SEL, "1.0", tk.END)
        self.raw_text.mark_set(tk.INSERT, "1.0")
        self.raw_text.see(tk.INSERT)
        return "break"

    # ──────────────── 数据流水线 ────────────────

    def _poll_queue(self):
        """每 30ms 从队列取数据并处理"""
        try:
            while True:
                line = self.data_queue.get_nowait()
                if line == "__DISCONNECTED__":
                    self._disconnect()
                    self._append_raw(f"[{_ts()}] !!! 串口意外断开\n")
                    continue
                self._process_line(line)
        except queue.Empty:
            pass
        self._after_id = self.root.after(30, self._poll_queue)

    def _process_line(self, line: str):
        """解析一行数据"""
        self.rx_count += len(line) + 1  # +1 for newline

        # 追加到原始日志
        self._append_raw(f"[{_ts()}] {line}\n")

        # 尝试匹配三种格式
        m = RE_ADT.search(line)
        if m:
            self._adt_vars["wear_status"].set(m.group(1))
            self._adt_vars["det_status"].set(m.group(2))
            self._adt_vars["ctr"].set(m.group(3))
            self._flash_label(self._adt_frame)

        m = RE_HR.search(line)
        if m:
            self._hr_vars["flag"].set(m.group(1))
            self._hr_vars["out"].set(m.group(2))
            self._hr_vars["snr"].set(m.group(3))
            self._hr_vars["level"].set(m.group(4))
            self._hr_vars["score"].set(m.group(5))
            self._hr_vars["acc"].set(m.group(6))
            self._hr_vars["scence"].set(m.group(7))
            self._flash_label(self._hr_frame)

        m = RE_HRV.search(line)
        if m:
            self._hrv_vars["rri0"].set(m.group(1))
            self._hrv_vars["rri1"].set(m.group(2))
            self._hrv_vars["rri2"].set(m.group(3))
            self._hrv_vars["rri3"].set(m.group(4))
            self._hrv_vars["confi"].set(m.group(5))
            self._hrv_vars["num"].set(m.group(6))
            self._flash_label(self._hrv_frame)

        self._update_status()

    def _append_raw(self, text: str):
        self.raw_text.configure(state=tk.NORMAL)
        self.raw_text.insert(tk.END, text)
        self.raw_text.see(tk.END)
        # 限制行数防止内存泄漏
        lines = int(self.raw_text.index("end-1c").split(".")[0])
        if lines > 5000:
            self.raw_text.delete("1.0", f"{lines - 3000}.0")
        self.raw_text.configure(state=tk.DISABLED)

    def _update_status(self):
        connected = self.ser is not None and self.ser.is_open
        if connected:
            self.status_var.set(
                f"已连接 {self.ser.port} @ {self.ser.baudrate}  |  接收: {self.rx_count} 字节"
            )
        else:
            self.status_var.set("已断开")

    def _flash_label(self, frame: ttk.LabelFrame):
        """给解析面板的数值标签一个短暂的视觉反馈"""
        for child in frame.winfo_children():
            if isinstance(child, ttk.Label):
                try:
                    child.configure(foreground="#4ec9b0")
                    self.root.after(150, lambda lbl=child: lbl.configure(foreground="#569cd6"))
                except Exception:
                    pass

    # ──────────────── 关闭 ────────────────

    def _on_close(self):
        if self._after_id:
            self.root.after_cancel(self._after_id)
        self._disconnect()
        self.root.destroy()


# ──────────────────────────────────────────────
#  工具函数
# ──────────────────────────────────────────────

def _make_kv_grid(parent, items: list):
    """在 parent 内生成标签:值 的网格"""
    for i, (label, var) in enumerate(items):
        lbl = ttk.Label(parent, text=label, anchor=tk.W)
        lbl.grid(row=i, column=0, sticky="w", pady=1)

        val = ttk.Label(
            parent,
            textvariable=var,
            anchor=tk.E,
            font=("Consolas", 11, "bold"),
            foreground="#569cd6",
        )
        val.grid(row=i, column=1, sticky="e", padx=(8, 0), pady=1)
        parent.columnconfigure(1, weight=1)


def _ts():
    return datetime.datetime.now().strftime("%H:%M:%S.%f")[:12]


# ──────────────────────────────────────────────
#  入口
# ──────────────────────────────────────────────
if __name__ == "__main__":
    root = tk.Tk()
    # 设置 ttk 主题 (Windows 下 "vista" / "winnative" / "xpnative")
    try:
        style = ttk.Style()
        style.theme_use("vista")
    except Exception:
        pass
    app = SerialMonitorApp(root)
    root.mainloop()
