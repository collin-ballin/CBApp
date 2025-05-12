#!/usr/bin/env python3
import sys, time, select, signal

signal.signal(signal.SIGINT,  signal.SIG_DFL)   # clean exit on SIGINT/SIGTERM
signal.signal(signal.SIGTERM, signal.SIG_DFL)

delay = 1.0           # seconds
n     = 1
stdin_fd = sys.stdin.fileno()

while True:
    # ---- check if C++ sent us a line ----
    ready, _, _ = select.select([stdin_fd], [], [], 0)
    if ready:                        # non‑blocking
        try:
            new_delay = float(sys.stdin.readline().strip())
            if new_delay > 0:
                delay = new_delay
        except ValueError:
            pass                     # ignore malformed input

    # ---- emit next number ----
    sys.stdout.write(f"{n}\n")
    sys.stdout.flush()
    n += 1
    time.sleep(delay)
