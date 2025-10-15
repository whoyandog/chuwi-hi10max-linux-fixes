#!/usr/bin/env python3
from evdev import UInput, ecodes as e
import signal

ui = UInput({e.EV_SW: [e.SW_TABLET_MODE]}, name="chuwi-tablet-mode-switch")

ui.write(e.EV_SW, e.SW_TABLET_MODE, 1)
ui.syn()

def handler(sig, frame):
    ui.close()
    exit(0)

signal.signal(signal.SIGTERM, handler)
signal.signal(signal.SIGINT, handler)

signal.pause()

