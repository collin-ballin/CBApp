#!/usr/bin/env python3
"""
fpga_stream.py  –  Streams coincidence‑counter data to stdout (JSON lines). 
    VERSION 2.0 --- May 24, 2025.

Modes:
-----
1) Real hardware  (default)
2) Mock           (--mock or hardware unavailable)

Commands via stdin:
------------------
    duration <sec>      # seconds per acquisition   (alias: time)
    window   <clks>     # coincidence‑window register
    quit                # clean exit
"""
import sys, time, json, threading, queue, signal, datetime, argparse, random
from typing import List, Tuple

################################################################################
#
#
#    1.  GLOBAL CONSTANTS...
################################################################################
################################################################################
BITFILE         = r"c:\Users\larsenal\Downloads\SPADCCUMK2\LabVIEW Data\SPADMK2\FPGA Bitfiles\spadccumk2_FPGATarget_DSPTesting_zbVGNUvhPcI.lvbitx"
RESOURCE        = r"rio://172.22.11.2/RIO0"
USE_HARDWARE    = False


#  SAMPLE FPGA DATA (For running is Dummy Mode)...
#
SAMPLE_PACKETS: List[Tuple[List[int], int]] = [
    ([0, 418, 567, 0, 46168, 1, 1, 1, 76437, 1, 0, 0, 223, 0, 6, 78], 280571200),
    ([0, 438, 554, 0, 46727, 1, 2, 0, 76220, 0, 3, 0, 228, 1, 6, 62], 280366940),
    ([0, 383, 592, 0, 46708, 1, 1, 2, 76678, 1, 2, 0, 222, 0, 3, 73], 280473760),
    ([0, 389, 540, 0, 45668, 0, 1, 0, 76889, 3, 2, 0, 218, 1, 5, 67], 280263795),
    ([0, 423, 582, 0, 45829, 0, 1, 3, 77436, 0, 4, 0, 230, 1, 0, 74], 280381675),
    ([0, 373, 561, 0, 46734, 0, 3, 1, 76781, 1, 1, 1, 233, 0, 1, 68], 280412090),
    ([0, 411, 573, 0, 47073, 0, 2, 0, 77032, 1, 4, 0, 212, 0, 2, 65], 280397250),
    ([0, 408, 571, 0, 46020, 0, 2, 0, 77612, 2, 1, 0, 228, 0, 5, 97], 280285880),
    ([0, 394, 607, 0, 47034, 0, 2, 2, 77256, 2, 3, 3, 239, 1, 6, 81], 280301000),
    ([0, 409, 597, 0, 46613, 2, 4, 0, 77362, 1, 5, 2, 236, 0, 4, 74], 280422345),
    ([0, 418, 568, 0, 46767, 0, 0, 1, 77252, 4, 1, 0, 225, 0, 3, 76], 280443485),
    ([0, 401, 596, 0, 46826, 0, 1, 1, 77547, 1, 0, 0, 226, 2, 5, 84], 280343420),
    ([0, 403, 607, 0, 46756, 3, 0, 1, 77649, 2, 4, 1, 217, 0, 6, 75], 280318465),
    ([0, 392, 578, 0, 47265, 0, 2, 4, 77869, 3, 3, 0, 205, 1, 3, 77], 280536935),
    ([0, 425, 623, 0, 46783, 2, 4, 3, 77269, 0, 3, 1, 231, 1, 5, 79], 280323085),
    ([0, 406, 641, 0, 50740, 3, 0, 2, 82000, 2, 4, 0, 295, 0, 4, 79], 280449155),
    ([0, 429, 561, 0, 52491, 0, 1, 0, 82824, 2, 1, 0, 271, 1, 4, 84], 280179165),
    ([0, 375, 556, 0, 50083, 0, 1, 2, 80593, 4, 3, 0, 243, 1, 4, 80], 280282065),
    ([0, 401, 577, 0, 46642, 1, 4, 1, 78551, 1, 2, 0, 238, 0, 4, 81], 280526435),
    ([0, 411, 579, 0, 46862, 3, 1, 3, 78070, 3, 0, 0, 233, 0, 4, 78], 280330890),
    ([0, 402, 549, 0, 47170, 3, 1, 1, 78391, 2, 4, 1, 256, 2, 4, 80], 280521115),
    ([0, 441, 535, 0, 47157, 1, 4, 4, 78319, 2, 0, 1, 217, 1, 5, 96], 280315700),
    ([0, 416, 584, 0, 47276, 1, 0, 0, 78676, 2, 3, 2, 214, 0, 5, 69], 280421575),
    ([0, 436, 581, 0, 47137, 1, 0, 1, 79030, 2, 5, 0, 233, 3, 6, 79], 280427840),
    ([0, 386, 575, 0, 47011, 0, 4, 0, 79049, 1, 5, 0, 249, 0, 7, 69], 280314615),
    ([0, 396, 599, 0, 47048, 1, 1, 0, 79262, 4, 1, 0, 238, 2, 3, 99], 280313355),
    ([0, 388, 594, 0, 47500, 1, 2, 2, 79423, 1, 4, 1, 218, 1, 7, 76], 280315350),
    ([0, 416, 579, 0, 47327, 1, 0, 2, 78934, 2, 1, 0, 241, 0, 5, 95], 280250320),
    ([0, 410, 565, 0, 47045, 1, 1, 1, 79344, 1, 1, 0, 220, 0, 6, 85], 280347515),
    ([0, 393, 572, 0, 47552, 2, 1, 1, 79628, 1, 3, 0, 247, 1, 11, 82], 280490490),
    ([0, 411, 577, 0, 47185, 1, 2, 2, 79483, 0, 2, 0, 239, 4, 2, 95], 280409990),
    ([0, 381, 639, 0, 47453, 0, 2, 0, 80134, 1, 2, 0, 239, 1, 8, 95], 280265125),
    ([0, 413, 571, 0, 47596, 0, 1, 2, 79873, 2, 3, 0, 238, 1, 3, 72], 280472885),
    ([0, 401, 580, 0, 47366, 1, 0, 0, 79989, 0, 2, 0, 260, 2, 5, 82], 280462245),
    ([0, 366, 600, 0, 47178, 1, 2, 1, 79534, 2, 4, 1, 267, 0, 4, 65], 280270270),
    ([0, 416, 564, 0, 47600, 0, 0, 2, 80260, 3, 2, 4, 230, 1, 2, 75], 280532490),
    ([0, 413, 568, 0, 47752, 2, 4, 0, 79375, 1, 3, 0, 251, 0, 3, 72], 280443240),
    ([0, 378, 553, 0, 47136, 1, 0, 2, 79330, 1, 5, 2, 243, 0, 5, 79], 280342020),
    ([0, 392, 559, 0, 47567, 3, 0, 0, 79981, 3, 3, 0, 243, 0, 3, 83], 280613165),
    ([0, 365, 599, 0, 47341, 2, 2, 3, 80437, 4, 4, 0, 258, 1, 5, 75], 280326305),
    ([0, 378, 576, 0, 47391, 1, 2, 1, 80598, 1, 3, 0, 212, 1, 5, 85], 280500255),
    ([0, 402, 560, 0, 47302, 1, 1, 1, 80205, 0, 3, 0, 265, 0, 3, 91], 280317625),
    ([0, 399, 579, 0, 48121, 2, 1, 2, 80527, 2, 2, 0, 243, 1, 5, 79], 280538055),
    ([0, 434, 559, 0, 47768, 0, 1, 1, 80314, 1, 1, 1, 235, 2, 5, 95], 280386120),
    ([0, 411, 603, 0, 47725, 1, 0, 1, 80741, 0, 2, 0, 246, 0, 4, 94], 280507290),
    ([0, 398, 615, 0, 47895, 2, 1, 1, 80413, 0, 4, 0, 232, 0, 1, 76], 280296415),
    ([0, 392, 626, 0, 47609, 1, 0, 2, 80671, 1, 3, 0, 249, 2, 3, 78], 280308455),
    ([0, 398, 599, 0, 48222, 2, 3, 1, 80299, 5, 3, 0, 252, 2, 2, 75], 280415800),
    ([0, 399, 593, 0, 47679, 2, 1, 2, 80355, 2, 2, 1, 251, 2, 2, 82], 280547330),
    ([0, 402, 542, 0, 47453, 1, 2, 0, 80813, 3, 1, 0, 258, 0, 4, 60], 280400820)
]

#  Import NI‑FPGA only if available
try:
    from nifpga import Session
    _nifpga_available = True
except ImportError:
    _nifpga_available = False



################################################################################
#
#
#    2.  HELPER FUNCTIONS (SAME AS IN THE ORIGINAL SCRIPT)...
################################################################################
################################################################################

#  "measure_raw"
#
def measure_raw(session):
    counts = [int(x) for x in session.registers["Counts"].read()]
    cycles = int(session.registers["CYCLES"].read())
    return counts, cycles


#  "start_measure"
#
def start_measure(session, coincidence_clks=1):
    enable = session.registers["ENABLE"]
    clear  = session.registers["CLEAR"]
    winreg = session.registers["Conicidence Window"]

    enable.write(False)
    clear.write(True)
    winreg.write(coincidence_clks)

    counts, cycles = measure_raw(session)
    if cycles or any(counts):
        raise RuntimeError("Clear/Stop failed")

    clear.write(False)
    enable.write(True)


#  "finish_measure"
#
def finish_measure(session):
    enable = session.registers["ENABLE"]
    enable.write(False)
    time.sleep(0.1)
    return measure_raw(session)




################################################################################
#
#
#    3.     COMMAND THREAD: Reads stdin, pushes updates -> queue
################################################################################
################################################################################
cmd_q: "queue.Queue[Tuple[str, float|int|None]]" = queue.Queue()

#  "stdin_reader"
#
def stdin_reader():
    """Accept text commands from the C++ host (one per line)."""
    for line in sys.stdin:
        toks = line.strip().split()
        if not toks:
            continue
        cmd = toks[0].lower()

        if cmd == "integration_window" and len(toks) == 2:
            try:
                cmd_q.put(("integration_window", max(0.05, float(toks[1]))))
            except ValueError:
                pass
        elif cmd == "coincidence_window" and len(toks) == 2:
            try:
                cmd_q.put(("coincidence_window", int(toks[1], 0)))
            except ValueError:
                pass
        elif cmd == "quit":
            cmd_q.put(("quit", None))
            break
    #
    # ------------------------------------------------------------------
    # Back‑compat support (optional)
    # ------------------------------------------------------------------
    #   elif cmd in ("duration", "time", "delay") and len(toks) == 2:
    #       try:
    #           cmd_q.put(("integration_window", max(0.05, float(toks[1]))))
    #       except ValueError:
    #           pass
    #   elif cmd == "window" and len(toks) == 2:
    #       try:
    #           cmd_q.put(("coincidence_window", int(toks[1], 0)))
    #       except ValueError:
    #           pass
    return


#   3.  GRACEFUL SIGNALS...
################################################################################
################################################################################
threading.Thread(target=stdin_reader, daemon=True).start()
signal.signal(signal.SIGINT,  signal.SIG_DFL)
signal.signal(signal.SIGTERM, signal.SIG_DFL)


#   4.  MOCK‑PACKET GENERATOR...
################################################################################
################################################################################

#   "mock_packets"
#
def mock_packets():
    i = 0
    while True:
        counts, cycles = SAMPLE_PACKETS[i]
        jittered = [int(random.poisson(mu) if mu > 20 else mu)
                    if hasattr(random, "poisson") else mu
                    for mu in counts]
        yield jittered, cycles
        i = (i + 1) % len(SAMPLE_PACKETS)






################################################################################
#
#
#   5.  MAIN LOOP...
################################################################################
################################################################################

#   "main"
#
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--mock", action="store_true",
                        help="Force mock‑data mode even if hardware present")
    args = parser.parse_args()

    mock_mode = args.mock or not _nifpga_available
    if not mock_mode:
        try:
            with Session(bitfile=BITFILE, resource=RESOURCE) as _tmp:
                pass
        except Exception as e:
            sys.stderr.write(f"Hardware open failed: {e}.  Falling back to mock.\n")
            mock_mode = True

    # --- runtime‑tunable parameters -----------------------------------------
    integration_window  = 1.0      # seconds per acquisition
    coincidence_window  = 50_000   # clock cycles

    if mock_mode:
        pkt_iter = mock_packets()
        sys.stderr.write("Running in MOCK mode\n")
        while True:
            # drain commands
            try:
                while True:
                    key, val = cmd_q.get_nowait()
                    if key == "integration_window":
                        integration_window = val
                    elif key == "coincidence_window":
                        coincidence_window = val  # kept for parity only
                    elif key == "quit":
                        return
            except queue.Empty:
                pass

            counts, cycles = next(pkt_iter)
            record = {
                "t": datetime.datetime.utcnow().isoformat(timespec="seconds") + "Z",
                "cycles": cycles,
                "counts": counts,
            }
            print(json.dumps(record), flush=True)
            time.sleep(integration_window)

    else:
        with Session(bitfile=BITFILE, resource=RESOURCE) as session:
            session.reset()
            session.run()

            while True:
                # drain commands
                try:
                    while True:
                        key, val = cmd_q.get_nowait()
                        if key == "integration_window":
                            integration_window = val
                        elif key == "coincidence_window":
                            coincidence_window = val
                        elif key == "quit":
                            return
                except queue.Empty:
                    pass

                start_measure(session, coincidence_window)
                time.sleep(integration_window)
                counts, cycles = finish_measure(session)

                record = {
                    "t": datetime.datetime.utcnow().isoformat(timespec="seconds") + "Z",
                    "cycles": cycles,
                    "counts": counts,
                }
                print(json.dumps(record), flush=True)




################################################################################
#
#
#    APPLICATION ENTRY POINT...
################################################################################
################################################################################

if __name__ == "__main__":
    main()



################################################################################
################################################################################
#  END.
