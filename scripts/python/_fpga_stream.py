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
from typing import Any, Optional, Set, List, Tuple, Dict
from enum import Enum, auto, IntEnum
from dataclasses import dataclass, field


################################################################################
#
#
#
#    0.     CLASSES AND DATA-TYPE DEFINITIONS...
################################################################################
################################################################################



################################################################################
#           0.1.    GENERIC TYPES.
################################################################################

#   "OperationMode"
#
class OperationMode(IntEnum):
    """Simple enumeration type to define what state the Python script is operating in."""
    Default     = 0
    Hardware    = auto()
    Simulated   = auto()
    COUNT       = auto()



################################################################################
#           0.2.    INTER-PROCESS COMMUNICATION (IPC) STUFF.
################################################################################

CommandValue                            = float | int | None
cv_MIN_INTEGRATION_WINDOW   : float     = 0.05



################################################################################
#           0.3.    INTER-PROCESS COMMUNICATION:  COMMAND TYPES.
################################################################################

#   "CommandKind"
#
class CommandKind(str, Enum):
    """Taxonomy for IPC commands: action (procedure) vs param (state update)."""
    ACTION      = "action"
    PARAM       = "param"


#   "CommandID"
#
class CommandID(str, Enum):
    """Canonical command vocabulary received from stdin (IPC contract)."""
    INTEGRATION_WINDOW      = "integration_window"
    COINCIDENCE_WINDOW      = "coincidence_window"
    QUIT                    = "quit"


#   "Command"
#
@dataclass(frozen=True, slots=True)
class Command:
    """Typed command parsed from stdin and delivered via the command queue."""
    kind                    : CommandKind
    id                      : CommandID
    value                   : CommandValue


#   "RuntimeState"
#
@dataclass(slots=True)
class RuntimeState:
    """POD-style runtime tunables updated by IPC commands."""
    integration_window      : float
    coincidence_window      : int
    quit_requested          : bool = False



################################################################################
#           0.3X.   IPC COMMAND REGISTRY (SINGLE SOURCE OF TRUTH)...
################################################################################

class ValueType(str, Enum):
    """Value type carried by PARAM commands (used for parsing + validation + CLI seeding)."""
    NONE        = "none"
    INT         = "int"
    FLOAT       = "float"
    STR         = "str"
    PATH        = "path"


class ValidationPolicy(str, Enum):
    """How to handle invalid/out-of-range values for PARAM commands."""
    REJECT      = "reject"
    CLAMP       = "clamp"


@dataclass(frozen=True, slots=True)
class CommandSpec:
    """
    Canonical specification for a command supported by the IPC system.

    Notes:
    - ACTION commands typically have value_type NONE and are not seedable via CLI.
    - PARAM commands map to a RuntimeState field and are usually seedable via CLI.
    """
    kind                    : CommandKind
    id                      : CommandID

    # Wire protocol tokens (stdin)
    wire                    : str
    aliases                 : Tuple[str, ...]

    # Value semantics (PARAM only)
    value_type              : ValueType
    validation              : ValidationPolicy
    min_value               : Optional[float]
    max_value               : Optional[float]

    # State mapping (PARAM only)
    state_field             : Optional[str]

    # CLI seeding (PARAM only, typically)
    cli_flag                : Optional[str]
    seedable                : bool

    # Documentation
    help                    : str


#   NOTE:
#   - This registry is not yet wired into parsing/apply in this stage.
#   - Next step: drive _parse_command_line(...) and drain_and_apply_commands(...) from this table.
COMMAND_REGISTRY           : Tuple[CommandSpec, ...] = (
    CommandSpec(
          kind          = CommandKind.PARAM
        , id            = CommandID.INTEGRATION_WINDOW
        , wire          = CommandID.INTEGRATION_WINDOW.value
        , aliases       = ( "duration", "time" )
        , value_type    = ValueType.FLOAT
        , validation    = ValidationPolicy.CLAMP
        , min_value     = cv_MIN_INTEGRATION_WINDOW
        , max_value     = None
        , state_field   = "integration_window"
        , cli_flag      = "--integration-window"
        , seedable      = True
        , help          = "Seconds per acquisition (integration window)."
    )
  , CommandSpec(
          kind          = CommandKind.PARAM
        , id            = CommandID.COINCIDENCE_WINDOW
        , wire          = CommandID.COINCIDENCE_WINDOW.value
        , aliases       = ( "window", )
        , value_type    = ValueType.INT
        , validation    = ValidationPolicy.REJECT
        , min_value     = 0.0
        , max_value     = None
        , state_field   = "coincidence_window"
        , cli_flag      = "--coincidence-window"
        , seedable      = True
        , help          = "Coincidence window (ticks)."
    )
  , CommandSpec(
          kind          = CommandKind.ACTION
        , id            = CommandID.QUIT
        , wire          = CommandID.QUIT.value
        , aliases       = ( "exit", )
        , value_type    = ValueType.NONE
        , validation    = ValidationPolicy.REJECT
        , min_value     = None
        , max_value     = None
        , state_field   = None
        , cli_flag      = None
        , seedable      = False
        , help          = "Request clean exit."
    )
)


#   DERIVED LOOKUP TABLES (LOUD FAILURES IF REGISTRY IS INCONSISTENT)
_COMMAND_SPEC_BY_ID        : Dict[CommandID, CommandSpec] = { spec.id: spec for spec in COMMAND_REGISTRY }

_COMMAND_SPEC_BY_WIRE      : Dict[str, CommandSpec]       = {}
for _spec in COMMAND_REGISTRY:
    _tokens = (_spec.wire, *_spec.aliases)
    for _tok in _tokens:
        if _tok in _COMMAND_SPEC_BY_WIRE:
            raise RuntimeError(f"duplicate wire token in COMMAND_REGISTRY: {_tok!r}")
        _COMMAND_SPEC_BY_WIRE[_tok] = _spec


def get_command_spec_for_wire(token: str) -> Optional[CommandSpec]:
    spec : Optional[CommandSpec] = _COMMAND_SPEC_BY_WIRE.get(token)
    return spec



################################################################################
#           0.4.    INTER-PROCESS COMMUNICATION:  RECEPTION TYPES.
################################################################################


"""
IPC_SCHEMA_KEYS_vA

    - Recommended approach: use a `str`-backed Enum to define the IPC JSON “wire keys”.
    - This gives you a single authoritative vocabulary, prevents typos, and scales cleanly
    - as you add more message types / fields later.

    - Intended home     : `_fpga_stream.py` (imported as `cc` by `fpga_stream.py`).
"""
IPC_SCHEMA_VERSION          : Tuple[int,int,int]    = (0, 1, 0)



#   "JsonKey"
#
class JsonKey(str, Enum):
    """Canonical JSON key vocabulary for IPC records (wire format)."""
    # Common / envelope fields (reserved for future expansion)
    TYPE                = "type"
    VERSION             = "v"

    # Data-packet fields
    TIME                = "t"
    CYCLES              = "cycles"
    COUNTS              = "counts"


#   "MessageType"
#
class MessageType(str, Enum):
    """Canonical message types for IPC records (wire format)."""
    DATA                = "data"
    STATUS              = "status"
    ERROR               = "error"



#
#
#
################################################################################
################################################################################    #   END [[ 0.  "CLASSES + DATA-TYPES" ]].








################################################################################
#
#
#
#    1.     CAPTURING GLOBALS FROM PRIMARY PYTHON FILE...
################################################################################
################################################################################



################################################################################
#           1.1.    INITIALIZATION VARIABLES.
################################################################################
_INITIALIZED            : bool                          = False

#   Protect critical names from being overwritten by init(...)
_PROTECTED_NAMES        : Set[str]                      = {
      "init"
    , "_INITIALIZED"
    , "_PROTECTED_NAMES"
}

#   PACKETS OF SAMPLE FPGA DATA...
SAMPLE_DATA0            : List[Tuple[List[int], int]]   = None
SAMPLE_DATA1            : List[Tuple[List[int], int]]   = None
SAMPLE_DATA2            : List[Tuple[List[int], int]]   = None



################################################################################
#           1.2.    INITIALIZATION FUNCTIONS.
################################################################################

#   "init"
#
def init(*, allow_overwrite: bool = False, **kwargs: Any) -> None:
    global _INITIALIZED, SAMPLE_DATA0, SAMPLE_DATA1, SAMPLE_DATA2


    _load_data()    #  POPULATE THE "SAMPLE_DATA*" DICTIONARIES...
    

    if _INITIALIZED and (not allow_overwrite):
        raise RuntimeError("_fpga_stream.init() called more than once")

    for name, value in kwargs.items():
        # basic sanity checks
        if (not isinstance(name, str)) or (not name.isidentifier()):
            raise ValueError(f"invalid init name: {name!r}")

        # enforce your global naming convention (optional but recommended)
        # e.g. require leading underscore for “global-ish” values
        if not name.startswith("_"):
            raise ValueError(f"init name must start with '_': {name}")

        if (name in _PROTECTED_NAMES) and (not allow_overwrite):
            raise ValueError(f"refusing to overwrite protected name: {name}")

        if (name in globals()) and (not allow_overwrite):
            raise ValueError(f"name already defined in _fpga_stream: {name}")

        globals()[name] = value

    _INITIALIZED = True
    
    return


#   "require"
#
def require(name: str) -> Any:
    """Defensive accessor: forces a clear error if a required injected global is missing."""
    
    if name not in globals():
        raise RuntimeError(f"required global not initialized: {name}")
        
    return globals()[name]



#
#
#
################################################################################
################################################################################    #   END [[ 1.  "INIT" ]].






################################################################################
#
#
#
#    2.     FPGA-HARDWARE / NI-FPGA API STUFF...
################################################################################
################################################################################

#  "measure_raw"
#
def measure_raw(session):
    counts  = [int(x) for x in session.registers["Counts"].read()]
    cycles  = int(session.registers["CYCLES"].read())
    
    return counts, cycles


#  "start_measure"
#
def start_measure(session, coincidence_ticks=1):
    counts:List     = None
    cycles:int      = 0
    enable          = session.registers["ENABLE"]
    clear           = session.registers["CLEAR"]
    winreg          = session.registers["Conicidence Window"]

    enable          .write(False)
    clear           .write(True)
    winreg          .write(coincidence_ticks)

    counts, cycles  = measure_raw(session)
    if cycles or any(counts):
        raise RuntimeError("Clear/Stop failed")

    clear.write(False)
    enable.write(True)
    
    return;


#  "finish_measure"
#
def finish_measure(session):
    finish_duration:float   = globals().get( '_MEASUREMENT_COMPLETION_DELAY'   , 0.1 )

    enable                  = session.registers["ENABLE"]
    
    enable.write(False)
    time.sleep(finish_duration)
    
    return measure_raw(session)



#
#
#
################################################################################
################################################################################    #   END [[ 2.  "FPGA-HARDWARE" ]].






################################################################################
#
#
#
#    3.     INTER-PROCESS COMMUNICATION (IPC) AND CONCURRENCY STUFF...
################################################################################
################################################################################


################################################################################
#           3.1.    INTER-PROCESS COMMUNICATION:  "TRANSMISSION" TYPES.
################################################################################

#   "_utc_timestamp_z_seconds"
#
def _utc_timestamp_z_seconds() -> str:
    ts_utc              : datetime.datetime = datetime.datetime.now(datetime.timezone.utc)
    ts_iso              : str               = ts_utc.isoformat(timespec="seconds")

    # `isoformat()` yields "...+00:00" for UTC; normalize to the `"Z"` suffix.
    if ts_iso.endswith("+00:00"):
        ts_iso = ts_iso[:-6] + "Z"

    return ts_iso


#   "_make_data_record"
#
def _make_data_record(counts: List[int], cycles: int) -> Dict[str, Any]:
    ts                  : str               = _utc_timestamp_z_seconds()
    record              : Dict[str, Any]    = {
          JsonKey.TYPE.value        : MessageType.DATA.value
        , JsonKey.VERSION.value     : IPC_SCHEMA_VERSION
        , JsonKey.TIME.value        : ts
        , JsonKey.CYCLES.value      : cycles
        , JsonKey.COUNTS.value      : counts
    }

    return record


#   "emit_data_record"
#
def emit_data_record(counts: List[int], cycles: int) -> None:
    record              : Dict[str, Any]    = _make_data_record(counts, cycles)
    payload             : str               = json.dumps(record)

    sys.stdout.write(payload + "\n")
    sys.stdout.flush()

    return



################################################################################
#           3.2.    INTER-PROCESS COMMUNICATION:  "RECEPTION" TYPES.
################################################################################

#   "_parse_command_line"
#
def _parse_command_line(line: str) -> Optional[Command]:
    toks                    : list[str]             = line.strip().split()
    token                   : str                   = ""
    spec                    : Optional[CommandSpec] = None
    raw                     : Optional[str]         = None

    if not toks:
        return None

    token = toks[0].lower()
    spec  = get_command_spec_for_wire(token)
    if spec is None:
        return None


    #   ACTION COMMANDS: no payload (by design, at this stage).
    if spec.kind is CommandKind.ACTION:
        if len(toks) != 1:
            return None
        return Command(kind=spec.kind, id=spec.id, value=None)


    #   PARAM COMMANDS: single scalar payload at this stage.
    if spec.kind is CommandKind.PARAM:
        if len(toks) != 2:
            return None

        raw = toks[1]

        #   1) COERCE STRING -> TYPED VALUE
        try:
            match spec.value_type:
                case ValueType.FLOAT:
                    value = float(raw)
                case ValueType.INT:
                    value = int(raw, 0)     # accepts decimal or 0x... form
                case ValueType.STR:
                    value = str(raw)
                case ValueType.PATH:
                    value = str(raw)        # keep as str for now (Path wiring later)
                case ValueType.NONE:
                    value = None
                case _:
                    return None
        except ValueError:
            return None

        #   2) VALIDATE / CLAMP (numeric types only)
        if spec.value_type in (ValueType.FLOAT, ValueType.INT):
            v_f                     : float     = float(value)
            min_v                   : Optional[float] = spec.min_value
            max_v                   : Optional[float] = spec.max_value

            if spec.validation is ValidationPolicy.REJECT:
                if (min_v is not None) and (v_f < min_v):
                    return None
                if (max_v is not None) and (v_f > max_v):
                    return None

            elif spec.validation is ValidationPolicy.CLAMP:
                if (min_v is not None) and (v_f < min_v):
                    v_f = min_v
                if (max_v is not None) and (v_f > max_v):
                    v_f = max_v

            # restore original numeric type
            if spec.value_type is ValueType.INT:
                value = int(v_f)
            else:
                value = float(v_f)

        return Command(kind=spec.kind, id=spec.id, value=value)


    return None




################################################################################
#           3.4.    IPC UTILITY FUNCTIONS.
################################################################################

#   "drain_and_apply_commands"
#
def drain_and_apply_commands(command_queue: "queue.Queue[Command]", state: RuntimeState) -> None:
    cmd                         : Optional[Command]             = None
    last_param_updates          : Dict[CommandID, CommandValue] = {}
    quit_seen                   : bool                          = False

    #   1) DRAIN QUEUE (COALESCE PARAM UPDATES: LATEST WINS)
    while True:
        try:
            cmd = command_queue.get_nowait()
        except queue.Empty:
            break

        if cmd.kind is CommandKind.PARAM:
            last_param_updates[cmd.id] = cmd.value
            continue

        if cmd.kind is CommandKind.ACTION:
            if cmd.id is CommandID.QUIT:
                quit_seen = True
            continue

        continue

    #   2) APPLY PARAM UPDATES (REGISTRY-DRIVEN)
    for cmd_id, value in last_param_updates.items():
        spec : Optional[CommandSpec] = _COMMAND_SPEC_BY_ID.get(cmd_id)
        if spec is None:
            raise RuntimeError(f"no CommandSpec for CommandID: {cmd_id!r}")

        if spec.kind is not CommandKind.PARAM:
            raise RuntimeError(f"CommandSpec kind mismatch for {cmd_id!r}: {spec.kind!r}")

        field_name : Optional[str] = spec.state_field
        if field_name is None:
            raise RuntimeError(f"PARAM command missing state_field mapping: {cmd_id!r}")

        if not hasattr(state, field_name):
            raise RuntimeError(f"RuntimeState missing field {field_name!r} for command {cmd_id!r}")

        # parse guarantees value is already typed/validated; still coerce defensively
        match spec.value_type:
            case ValueType.FLOAT:
                setattr(state, field_name, float(value))
            case ValueType.INT:
                setattr(state, field_name, int(value))
            case ValueType.STR:
                setattr(state, field_name, str(value))
            case ValueType.PATH:
                setattr(state, field_name, str(value))  # Path wiring later
            case _:
                raise RuntimeError(f"unsupported ValueType for PARAM command {cmd_id!r}: {spec.value_type!r}")

    #   3) APPLY ACTIONS
    if quit_seen:
        state.quit_requested = True

    return




################################################################################
#           3.X.    IPC DAEMON FUNCTIONS.
################################################################################

#  "stdin_reader"
#
def stdin_reader(command_queue: "queue.Queue[Command]") -> None:
    """Accept text commands from the C++ host (one per line)."""

    line                    : str                   = ""
    cmd                     : Optional[Command]     = None

    for line in sys.stdin:
        cmd = _parse_command_line(line)
        if cmd is None:
            continue

        command_queue.put(cmd)
        if cmd.id is CommandID.QUIT:
            break

    return



#
#
#
################################################################################
################################################################################    #   END [[ 3.  "IPC" ]].












################################################################################
#
#
#
#    X.     DATA SIMULATION AND GENERATION...
################################################################################
################################################################################
"""
[   UNUSED,     D,          C,          CD,
    B,          BD,         BC,         BCD,
    A,          AD,         AC,         ACD,
    AB,         ABD,        ABC,        ABCD    ]
    
    Coincidence window: 10
"""

################################################################################
#           X.1.    DATA SIMULATION / GENERATION FUNCTIONS.
################################################################################

#   "mock_packets"
#
def mock_packets(data:List[Tuple[List[int], int]]):
    i           : int       = 0
    length      : int       = len(data)


    while True:
        counts, cycles      = data[i]
        jittered            = [
            int(random.poisson(mu) if (mu > 20)     else mu)
                if hasattr(random, "poisson")   else mu
            for mu in counts
        ]
        yield jittered, cycles
        i = (i + 1) % length


    return



################################################################################
#           X.2.    READ-ONLY DATA EXCERPTS.
################################################################################

#   "_load_data"
#
def _load_data():
    global SAMPLE_DATA0, SAMPLE_DATA1, SAMPLE_DATA2


    #   "SAMPLE_DATA1"
    #       - CALIBRATION DATA (For testing AVERAGE-VALUE Computations, etc)...
    #
    #   UNUSED.     D.      C.      CD.     B.      BD.     BC.     BCD.    A.      AD.     AC.     ACD.    AB.     ABD.    ABC.    ABCD.       FPGA CYCLES.    #
    ##############################################################################################################################################################
    SAMPLE_DATA1 = [
        ([0,        0,      5,      0,      0,      0,      0,      0,      1,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      2,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      3,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      4,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      5,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      6,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      7,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      8,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      9,      0,      0,      0,      0,      0,      0,      0],         0)
    ]



    #   "SAMPLE_DATA2"
    #       - SAMPLE FPGA DATA (For running is Dummy Mode)...
    #
    SAMPLE_DATA2 = [
        ([0, 418, 567, 0, 46168, 1, 1, 1, 76437, 1, 0, 0, 223, 0, 6, 78], 280571200),
        ([0, 438, 554, 0, 46727, 1, 2, 0, 76220, 0, 3, 0, 228, 1, 6, 62], 280366940),
        ([0, 383, 592, 0, 46708, 1, 1, 2, 76678, 1, 2, 0, 222, 0, 3, 73], 280473760),
        ([0, 389, 540, 0, 45668, 0, 1, 0, 76889, 3, 2, 0, 218, 1, 5, 67], 280263795),
        ([0, 423, 582, 0, 45829, 0, 1, 3, 77436, 0, 4, 0, 230, 1, 0, 74], 280381675),
        ([0, 373, 561, 0, 46734, 0, 3, 1, 76781, 1, 1, 1, 233, 0, 1, 68], 280412090),
        ([0, 411, 573, 0, 47073, 0, 2, 0, 77032, 1, 4, 0, 212, 0, 2, 65], 280397250),
        ([0, 408, 571, 0, 46020, 0, 2, 0, 77612, 2, 1, 0, 228, 0, 5, 97], 280285880),
        ([0,        0,      5,      0,      0,      0,      0,      0,      6,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      7,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      8,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0, 394, 607, 0, 47034, 0, 2, 2, 77256, 2, 3, 3, 239, 1, 6, 81], 280301000),
        ([0, 409, 597, 0, 46613, 2, 4, 0, 77362, 1, 5, 2, 236, 0, 4, 74], 280422345),
        ([0, 418, 568, 0, 46767, 0, 0, 1, 77252, 4, 1, 0, 225, 0, 3, 76], 280443485),
        ([0, 401, 596, 0, 46826, 0, 1, 1, 77547, 1, 0, 0, 226, 2, 5, 84], 280343420),
        ([0, 403, 607, 0, 46756, 3, 0, 1, 77649, 2, 4, 1, 217, 0, 6, 75], 280318465),
        ([0, 392, 578, 0, 47265, 0, 2, 4, 77869, 3, 3, 0, 205, 1, 3, 77], 280536935),
        ([0, 425, 623, 0, 46783, 2, 4, 3, 77269, 0, 3, 1, 231, 1, 5, 79], 280323085),
        ([0, 406, 641, 0, 50740, 3, 0, 2, 82000, 2, 4, 0, 295, 0, 4, 79], 280449155),
        ([0, 429, 561, 0, 52491, 0, 1, 0, 82824, 2, 1, 0, 271, 1, 4, 84], 280179165),
        ([0,        0,      5,      0,      0,      0,      0,      0,      8,      0,      0,      0,      0,      0,      0,      0],         0),
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
        ([0,        0,      5,      0,      0,      0,      0,      0,      6,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      7,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      8,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      6,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      7,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      8,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      6,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      7,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      8,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      6,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      7,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      8,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0, 378, 576, 0, 47391, 1, 2, 1, 80598, 1, 3, 0, 212, 1, 5, 85], 280500255),
        ([0, 402, 560, 0, 47302, 1, 1, 1, 80205, 0, 3, 0, 265, 0, 3, 91], 280317625),
        ([0,        0,      5,      0,      0,      0,      0,      0,      6,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      7,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0,        0,      5,      0,      0,      0,      0,      0,      8,      0,      0,      0,      0,      0,      0,      0],         0),
        ([0, 399, 579, 0, 48121, 2, 1, 2, 80527, 2, 2, 0, 243, 1, 5, 79], 280538055),
        ([0, 434, 559, 0, 47768, 0, 1, 1, 80314, 1, 1, 1, 235, 2, 5, 95], 280386120),
        ([0, 411, 603, 0, 47725, 1, 0, 1, 80741, 0, 2, 0, 246, 0, 4, 94], 280507290),
        ([0, 398, 615, 0, 47895, 2, 1, 1, 80413, 0, 4, 0, 232, 0, 1, 76], 280296415),
        ([0, 392, 626, 0, 47609, 1, 0, 2, 80671, 1, 3, 0, 249, 2, 3, 78], 280308455),
        ([0, 398, 599, 0, 48222, 2, 3, 1, 80299, 5, 3, 0, 252, 2, 2, 75], 280415800),
        ([0, 399, 593, 0, 47679, 2, 1, 2, 80355, 2, 2, 1, 251, 2, 2, 82], 280547330),
        ([0, 402, 542, 0, 47453, 1, 2, 0, 80813, 3, 1, 0, 258, 0, 4, 60], 280400820)
    ]


    #   "SAMPLE_DATA0"
    #       - SAMPLE FPGA DATA (For running is Dummy Mode)...
    #
    SAMPLE_DATA0 = [
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
    
    return;



#
#
#
################################################################################
################################################################################    #   END [[ X.  "DATA SIMULATION" ]].











################################################################################
##
##
##
################################################################################
################################################################################    #  END [[ ALL ]].
