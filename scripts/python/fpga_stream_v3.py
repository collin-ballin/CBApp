#!/usr/bin/env python3
"""
"fpga_stream.py"
    @brief Streams coincidence‑counter data to stdout (JSON lines). 
    VERSION 2.0 --- May 24, 2025.

    
MODES OF OPERATION:
-----
    1) Real hardware    (default)
    2) Mock             (--mock or hardware unavailable)


COMMANDS VIA STDIN:
------------------
    duration    <sec>           #   seconds per acquisition   (alias: time)
    window      <clks>          #   coincidence‑window register
    quit                        #   clean exit

"""
import sys, time, json, threading, queue, signal, datetime, argparse, random
from typing import List, Tuple
#
#   from _FPGA_SAMPLE_DATA import SAMPLE_DATA0 as SAMPLE_PACKETS
#   from _FPGA_SAMPLE_DATA import SAMPLE_DATA2 as SAMPLE_PACKETS

################################################################################
#
#
#    1.  GLOBAL CONSTANTS...
################################################################################
################################################################################
USE_CALIBRATION_DATA    = False     # W/O not to use "real" FPGA data or "calibration" data to test AVG, etc.



#   CASE 1 :    Use "CALIBRATION DATA"      -- Artificial data that is used to test the average computations and etc...
if ( USE_CALIBRATION_DATA ):
    from _FPGA_SAMPLE_DATA import SAMPLE_DATA2 as SAMPLE_PACKETS
#
#   CASE 2 :    Use "REAL FPGA DATA"        -- This is MOCK DATA that was recorded directly from the FPGA...
else:
    from _FPGA_SAMPLE_DATA import SAMPLE_DATA0 as SAMPLE_PACKETS


BITFILE                 = r"c:\Users\larsenal\Downloads\SPADCCUMK2\LabVIEW Data\SPADMK2\FPGA Bitfiles\spadccumk2_FPGATarget_DSPTesting_zbVGNUvhPcI.lvbitx"
RESOURCE                = r"rio://172.22.11.2/RIO0"
USE_HARDWARE            = False


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


#       3.      GRACEFUL SIGNALS...
################################################################################
################################################################################
threading.Thread(target=stdin_reader, daemon=True).start()
signal.signal(signal.SIGINT,  signal.SIG_DFL)
signal.signal(signal.SIGTERM, signal.SIG_DFL)



#       4.      MOCK‑PACKET GENERATOR...
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

    return






################################################################################
#
#
#
#   5.      MAIN LOOP...
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
