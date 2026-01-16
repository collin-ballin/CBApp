#!/usr/bin/env python3
"""
fpga_stream.py  –  Streams coincidence-counter data to stdout (JSON lines). 
    VERSION 2.0 --- May 24, 2025.

Modes:
-----
1) Real hardware  (default)
2) Mock           (--mock or hardware unavailable)

Commands via stdin:
------------------
    integration_window <sec>
    coincidence_window <ticks>
    quit
"""
import sys
import time
import threading
import queue
import signal
import argparse
from typing import Optional, List

try:
    import _fpga_stream as cc
except ImportError:
    sys.stderr.write("critical error: unable to import accompanying Python file `_fpga_stream.py`. abort.\n")
    raise SystemExit(1)


#       CASE 0 :    ENFORCE PYTHON INTERPRETER VERSION...
if sys.version_info < (3, 9):
    raise RuntimeError("This script requires Python 3.9+.")


################################################################################
#
#
#
#    1.     GLOBAL CONSTANTS...
################################################################################
################################################################################


################################################################################
#           1.1.    SCRIPT BEHAVIOR TOGGLES.
################################################################################
_ABORT_ON_IMPORT_ERROR      = False
_HARDWARE_AVAILABLE         = False
_STARTUP_SYNC_GRACE_SEC     = 0.050


################################################################################
#           1.2.    IMPLEMENT SCRIPT BEHAVIORS.
################################################################################

try:
    from nifpga import Session
    _HARDWARE_AVAILABLE = True
except ImportError:
    _HARDWARE_AVAILABLE = False


################################################################################
#           1.3.    DEFAULT SCRIPT PARAMETERS.
################################################################################

BITFILE                                 = r"C:\Users\Admin\Desktop\FPGA\spadccumk2_FPGATarget_DSPTesting_zbVGNUvhPcI.lvbitx"
RESOURCE                                = r"rio://172.22.11.2/RIO0"

_MEASUREMENT_COMPLETION_DELAY           = 0.1


################################################################################
#           1.4.    INITIALIZE SECONDARY PYTHON FILE.
################################################################################
cc.init(
      _MEASUREMENT_COMPLETION_DELAY   = _MEASUREMENT_COMPLETION_DELAY
)


#
#
#
################################################################################
################################################################################    #   END [[ 1.  "GLOBAL CONSTANTS" ]].






################################################################################
#
#
#
#    3.     INTER-PROCESS COMMUNICATION (IPC) AND CONCURRENCY STUFF...
################################################################################
################################################################################

_COMMAND_QUEUE      : "queue.Queue[cc.Command]"     = queue.Queue()


threading.Thread    ( target=cc.stdin_reader, args=(_COMMAND_QUEUE,), daemon=True ).start()
signal.signal       ( signal.SIGINT,  signal.SIG_DFL )
signal.signal       ( signal.SIGTERM, signal.SIG_DFL )



#
#
#
################################################################################
################################################################################    #   END [[ 3.  "IPC" ]].







################################################################################
#
#
#
#    4.     AUXILIARY FUNCTIONS (NON-HARDWARE RELATED)...
################################################################################
################################################################################



################################################################################
#           4.1.    AUXILIARY APPLICATION FUNCTIONS.
################################################################################

#   "setup_script"
#
def setup_script(argv: Optional[List[str]] = None) -> argparse.Namespace:
    parser: argparse.ArgumentParser = argparse.ArgumentParser()
    args  : argparse.Namespace      = None

    #   1.      SET-UP EACH COMMAND-LINE ARGUMENT...
    parser.add_argument(
          "--mock"
        , action     = "store_true"
        , help       = "Force mock-data mode even if hardware present"
    )

    cc.add_seed_arguments(parser)

    #   2.      ...
    args = parser.parse_args(argv)

    return args



################################################################################
#           4.2.    MAIN MANAGER FUNCTIONS FOR EACH OPERATION-MODE.
################################################################################

#   "_main_simulation"
#
def _main_simulation(state: cc.RuntimeState) -> int:

    #           0.1.    VARIABLE DEFINITIONS.
    exit_code               : int               = 0
    #   pkt_iter                                    = cc.mock_packets(cc.SAMPLE_DATA2)
    pkt_iter                                    = cc.mock_packets(cc.SAMPLE_DATA0)

    while True:
        cc.drain_and_apply_commands(_COMMAND_QUEUE, state)
        if state.quit_requested:
            return exit_code

        counts, cycles = next(pkt_iter)
        cc.emit_data_record(counts, cycles)
        time.sleep(state.integration_window)

    return exit_code



#   "_main_hardware"
#
def _main_hardware(state: cc.RuntimeState, bitfile: str, resource: str) -> int:

    #           0.1.    VARIABLE DEFINITIONS.
    exit_code               : int           = 0

    with Session(bitfile=bitfile, resource=resource) as session:
        session.reset()
        session.run()

        while True:
            cc.drain_and_apply_commands(_COMMAND_QUEUE, state)
            if state.quit_requested:
                return exit_code

            cc.start_measure(session, state.coincidence_window)
            time.sleep(state.integration_window)
            counts, cycles = cc.finish_measure(session)

            cc.emit_data_record(counts, cycles)

    return exit_code



#
#
#
################################################################################
################################################################################    #   END [[ 4.  "AUXILIARY FUNCTIONS" ]].






################################################################################
#
#
#
#       5.      MAIN LOOP...
################################################################################
################################################################################
    
#   "main"
#
def main(args: argparse.Namespace) -> int:
    #           0.1.    VARIABLE DEFINITIONS.
    exit_code               : int                   = 0


    #           0.2.    ASSERTIONS / DEFENSIVE STATEMENTS.
    assert (args is not None)


    try:
        #           0.3.    OBTAIN REQUIRED GLOBALS (LOUD FAILURE IF MISSING).
        command_queue        : "queue.Queue[cc.Command]"    = globals()["_COMMAND_QUEUE"]
        startup_grace_sec    : float                        = float(globals()["_STARTUP_SYNC_GRACE_SEC"])
        hardware_available   : bool                         = bool(globals()["_HARDWARE_AVAILABLE"])
        bitfile              : str                          = str(globals()["BITFILE"])
        resource             : str                          = str(globals()["RESOURCE"])


        #           0.4.    INITIALIZE RUNTIME STATE (SINGLE SOURCE FOR DEFAULTS IN THIS PROCESS RUN).
        state                : cc.RuntimeState              = cc.make_default_runtime_state()
        cc.apply_seed_args(args, state)


        #           0.6.    STARTUP SYNC GRACE: DRAIN ANY IMMEDIATE IPC UPDATES BEFORE FIRST EMISSION.
        sync_deadline        : float                        = time.monotonic() + startup_grace_sec
        while time.monotonic() < sync_deadline:
            cc.drain_and_apply_commands(command_queue, state)
            if state.quit_requested:
                return exit_code
            time.sleep(0.001)

        cc.drain_and_apply_commands(command_queue, state)
        if state.quit_requested:
            return exit_code


        #           0.7.    DETERMINE SCRIPT OPERATION MODE.
        simulated_mode       : bool                         = bool(args.mock) or (not hardware_available)

        if not simulated_mode:
            try:
                with Session(bitfile=bitfile, resource=resource) as _tmp:
                    pass
            except Exception as e:
                sys.stderr.write(f"Hardware open failed: {e}.  Falling back to simulated.\n")
                sys.stderr.flush()
                simulated_mode = True

        operation_mode       : cc.OperationMode             = (
            cc.OperationMode.Simulated if (simulated_mode) else cc.OperationMode.Hardware
        )


        #   1.      DISPATCH MAIN FUNCTION...
        if operation_mode is cc.OperationMode.Simulated:
            sys.stderr.write("`fpga_stream` operating in `simulated` mode.\n")
            sys.stderr.flush()
            exit_code = _main_simulation(state)

        elif operation_mode is cc.OperationMode.Hardware:
            sys.stderr.write("`fpga_stream` operating in `hardware` mode.\n")
            sys.stderr.flush()
            exit_code = _main_hardware(state, bitfile, resource)

        else:
            sys.stderr.write(f"Unknown OperationMode: {operation_mode}\n")
            sys.stderr.flush()
            exit_code = 1


    except KeyError as e:
        missing = e.args[0] if (len(e.args) > 0) else str(e)
        sys.stderr.write(f"critical error: missing required global: {missing!r}\n")
        sys.stderr.flush()
        exit_code = 2

    except Exception as e:
        sys.stderr.write(f"critical error: unhandled exception in main: {type(e).__name__}: {e}\n")
        sys.stderr.flush()
        exit_code = 1


    return exit_code



################################################################################
#           5.1.    APPLICATION ENTRY POINT.
################################################################################

if __name__ == "__main__":
    args            : Optional[argparse.Namespace]      = None
    exit_code       : int                               = 0

    args            = setup_script()
    exit_code       = main(args)

    raise SystemExit(exit_code)



#
#
#
################################################################################
################################################################################    #   END [[ 5.  "MAIN" ]].
