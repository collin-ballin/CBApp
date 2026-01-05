#!/usr/bin/env python3
"""
"fpga_stream.py"
------------------
    @brief Streams coincidence‑counter data to stdout (JSON lines). 
    VERSION 2.0.    --- May 24, 2025.
    VERSION 3.0.    --- August 29, 2025.
    VERSION 3.1.    --- December 27, 2025. 
    
    
[[TO-DO]]:          FOR VERSION 3.2...
------------------
    1.  LOGGING AND MESSAGING:
            - We need a way to establish a "messaging" channel to exchange logging 
              information back-and-forth between the main process and this Python script.  
            - Mostly, this is necessesary so we can report back if the script 
              is running in MOCK mode (fake data)  **OR**  if it was unable to 
              `import nifpga` and/or to report back any other problems that prevented
              a successful connection to the National Instruments hardware that we were expecting. 
              
    2.  COMMAND-LINE ARGUMENTS:
            1.  Add a "config" set of arguments...
                E.g.: we can spawn the process as: 
                    (venv) $:  python3 fpga_stream_v3.py  --integration-window=5.09  --coincidence_window=9.1  --variable_name=<my_value>    # etc...
                          
                    - Allow us to invoke the script from the command-line in a way that spawns the process 
                      with a specific value for each variable that we can control from our IPC implementation.  
                    - The reason we need this is because, currently, the only way to communicate is via IPC.  
                      **HOWEVER** --- there is concern as to w/o not the script receives this information BEFORE 
                      it performs the first instance of data collection.  
                    - TL;DR: 
                        We MUST guarantee that the script is able to utilize the CURRENT value of each parameter immediately upon execution. 
              
    3.  ADD AN OPTION FOR "ABORT IF IMPORT ERROR":
            * Currently --- the script behaves in the following manner:
                - In the event that Python raises an `ImportError` exception upon failure to `import nifpga`, the script will,
                  instead, proceed to utilize the `mock` / `simulated` / `artificial` data generation (not using the hardwear).  
                
            * We need an option for the following: 
                - If Python fails to `import nifpga`, abort all 
    
    4.  PROBLEM WITH `ticks`:
            - Currently, we set the `coincidence window` by the number of TICKS.  We need to 
              adapt the script so that it is able to correctly convert to/from: 
                `NUM. OF TICKS`    <==>    `NUM. OF SECONDS`.
    
    5.  PROBLEM WITH `FPGA clock cycle`:
            - The FPGA reports back the frequency **AFTER** the data is reported 
              back --- however, it is unclear whether or not there is an ability 
              to reliably control what the desired or target FPGA clock frequency is.  
    
    6.  EXPAND THE SET OF COMMAND-LINE ARGUMENTS:
            - Accept filepath arguments from the CLI arguments. 
            - ...
    
    7.  BETTER METHOD OF PAUSING THE DATA-COLLECTION FOR `integration window`:
            - Should we really be using `sleep`?
    
    8.  MAKE USE OF MORE FEATURES AVAILABLE FROM THE `nifpga` API:
            - What happens when we disconnect the FPGA from our computer?
            - Can we get more verbose error streaming to understand if the FPGA is disconnected?
    
    
[[TO-DO]]:          FOR VERSION 4.0...
------------------
    X1. STANDARDIZED INTERFACE:
            - We need some type of abstraction that someone can easily use as a skeleton 
              in order to set up a script that is designed to interface with our `PyStream` 
              class on the C++ side of things.  
    X2. INITIALIZATION CHECK:
            - We need a centralized data structure that stores ALL of the commands that we 
              are expecting to recieve from `stdin` and all the commands we expect to report
              back via `stdout`.  
            - This would allow us to perform a `self check operation` wherein the master process 
              can invoke this Python script in a `config` mode (perhaps we add a CLI argument so 
              the master process can explicitly invoke this mode when it runs the script).  
            - This `self checkup` procedure would validate that: 
                1.  The command-words that the master process expects to use to control the Python
                    process are, in fact, valid commands that the Python process will respond to. 
                2.  The command-words that are used to report information back to `stdout` are,
                    in fact, command-words that the master process is prepared to catch and respond to.
    X3. MORE SOPHISTOCATED METHOD OF PRODUCING MOCK DATA:
            - ...
    
    
MODES OF OPERATION:
------------------
    1.  Real hardware       (default)
    2.  Mock                (--mock or hardware unavailable)


INTER-PROCESS COMMUNICATION (IPC) COMMANDS VIA STDIN:
------------------
    duration    <sec>           #   seconds per acquisition   (alias: time)
    window      <clks>          #   coincidence‑window register
    quit                        #   clean exit

"""
import sys, time, json, threading, queue, signal, datetime, argparse, random
from typing import List, Tuple



################################################################################
#
#
#
#    1.     GLOBAL CONSTANTS...
################################################################################
################################################################################
ABORT_ON_IMPORT_ERROR   = False     #   if TRUE: the script will ABORT if unable to 
USE_CALIBRATION_DATA    = False     #   W/O not to use "real" FPGA data or "calibration" data to test AVG, etc.



#   CASE 1 :    Use "CALIBRATION DATA"      -- Artificial data that is used to test the average computations and etc...
if ( USE_CALIBRATION_DATA ):
    from _FPGA_SAMPLE_DATA import SAMPLE_DATA2 as SAMPLE_PACKETS
#
#   CASE 2 :    Use "REAL FPGA DATA"        -- This is MOCK DATA that was recorded directly from the FPGA...
else:
    from _FPGA_SAMPLE_DATA import SAMPLE_DATA0 as SAMPLE_PACKETS


BITFILE                 = r"C:\Users\Admin\Desktop\FPGA\spadccumk2_FPGATarget_DSPTesting_zbVGNUvhPcI.lvbitx"
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
#
#    2.     HELPER FUNCTIONS (SAME AS IN THE ORIGINAL SCRIPT)...
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



################################################################################
#
#           3.1.    GRACEFUL SIGNALS  [ SETUP SIGNAL HANDLERS ]...
################################################################################
################################################################################
threading.Thread    ( target=stdin_reader       , daemon=True           ).start()
signal.signal       ( signal.SIGINT             , signal.SIG_DFL        )
signal.signal       ( signal.SIGTERM            , signal.SIG_DFL        )



################################################################################
#
#           3.2.    MOCK‑PACKET GENERATOR  [ SET-UP CCOUNTER DATA SIMULATOR ]...
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
#       4.      MAIN LOOP...
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
#           5.1.    APPLICATION ENTRY POINT.
################################################################################
################################################################################

if __name__ == "__main__":
    main()












################################################################################
#
#
#
################################################################################
################################################################################    #  END ALL.
