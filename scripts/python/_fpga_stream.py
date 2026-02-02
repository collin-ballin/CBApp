#!/usr/bin/env python3
"""
_fpga_stream.py
---------------
Secondary/auxiliary module for `fpga_stream.py` (imported as `cc`).

Holds:
- IPC schema (JSON keys, message types, version).
- Command taxonomy + central command registry.
- stdin parsing + command application (queue drain).
- Data record emit helpers.
- Hardware helpers (start/finish/measure).
- Simulation data helpers.
"""
from __future__ import annotations
from dataclasses import dataclass
from enum import Enum, auto, IntEnum
from typing import Any, Optional, Set, List, Tuple, Dict, Union
import sys, time, json, queue, random, datetime



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
CommandValue                            = Union[float, int, str, None]
cv_MIN_INTEGRATION_WINDOW   : float     = 0.05


################################################################################
#           0.3.    INTER-PROCESS COMMUNICATION:  COMMAND TYPES.
################################################################################

#   "CommandKind"
#
class CommandKind(IntEnum):
    """Command taxonomy: PARAM mutates RuntimeState; ACTION triggers a procedure."""
    Action      = 0
    Param       = auto()
    COUNT       = auto()


#   "ValueType"
#
class ValueType(IntEnum):
    """Value type carried by a PARAM command."""
    NoneType    = 0
    Int         = auto()
    Float       = auto()
    Str         = auto()
    COUNT       = auto()


#   "ValidationPolicy"
#
class ValidationPolicy(IntEnum):
    """How to handle out-of-range values for PARAM commands."""
    Reject      = 0
    Clamp       = auto()
    COUNT       = auto()


#   "CommandID"
#
class CommandID(str, Enum):
    """Canonical command vocabulary received from stdin (IPC contract)."""
    INTEGRATION_WINDOW      = "integration_window"
    COINCIDENCE_WINDOW      = "coincidence_window"
    QUIT                    = "quit"


#   "CommandSpec"
#
@dataclass(frozen=True)
class CommandSpec:
    """Central specification record for one IPC command."""
    id                      : CommandID
    kind                    : CommandKind
    value_type              : ValueType

    # CLI seeding behavior (PARAM only):
    seedable                : bool
    cli_name                : Optional[str]     # option string without leading '--' (None => default to id.value)
    state_field             : Optional[str]     # RuntimeState field name (PARAM only)

    # Validation (PARAM only; value_type in {Int, Float})
    validation              : ValidationPolicy
    min_value               : Optional[float]
    max_value               : Optional[float]

    # Default (PARAM only; used to initialize RuntimeState for a new process run)
    default_value           : Optional[CommandValue]


#   "COMMAND_REGISTRY"
#
COMMAND_REGISTRY: Tuple[CommandSpec, ...] = (
    CommandSpec(
          id              = CommandID.INTEGRATION_WINDOW
        , kind            = CommandKind.Param
        , value_type      = ValueType.Float
        , seedable        = True
        , cli_name        = None
        , state_field     = "integration_window"
        , validation      = ValidationPolicy.Clamp
        , min_value       = cv_MIN_INTEGRATION_WINDOW
        , max_value       = None
        , default_value   = 1.0
    ),
    CommandSpec(
          id              = CommandID.COINCIDENCE_WINDOW
        , kind            = CommandKind.Param
        , value_type      = ValueType.Int
        , seedable        = True
        , cli_name        = None
        , state_field     = "coincidence_window"
        , validation      = ValidationPolicy.Clamp
        , min_value       = 1.0
        , max_value       = None
        , default_value   = 50_000
    ),
    CommandSpec(
          id              = CommandID.QUIT
        , kind            = CommandKind.Action
        , value_type      = ValueType.NoneType
        , seedable        = False
        , cli_name        = None
        , state_field     = None
        , validation      = ValidationPolicy.Reject
        , min_value       = None
        , max_value       = None
        , default_value   = None
    ),
)

_COMMAND_BY_KEY: Dict[str, CommandSpec]      = { spec.id.value: spec for spec in COMMAND_REGISTRY }
_COMMAND_BY_ID : Dict[CommandID, CommandSpec]= { spec.id: spec for spec in COMMAND_REGISTRY }


#   "Command"
#
@dataclass(frozen=True)
class Command:
    """Typed command parsed from stdin and delivered via the command queue."""
    id                      : CommandID
    value                   : CommandValue


#   "RuntimeState"
#
@dataclass
class RuntimeState:
    """POD-style runtime tunables updated by IPC commands."""
    integration_window      : float
    coincidence_window      : int
    quit_requested          : bool = False



################################################################################
#           0.4.    INTER-PROCESS COMMUNICATION:  TRANSMISSION TYPES.
################################################################################
IPC_SCHEMA_VERSION          : Tuple[int, int, int]    = (0, 1, 0)


#   "JsonKey"
#
class JsonKey(str, Enum):
    """Canonical JSON key vocabulary for IPC records (wire format)."""
    TYPE                = "type"
    VERSION             = "v"
    TIME                = "t"
    CYCLES              = "cycles"
    COUNTS              = "counts"
    MESSAGE             = "message"


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
    , "require"
    , "make_default_runtime_state"
    , "apply_seed_args"
    , "add_seed_arguments"
    , "_INITIALIZED"
    , "_PROTECTED_NAMES"
}

#   PACKETS OF SAMPLE FPGA DATA...
SAMPLE_DATA0            : Optional[List[Tuple[List[int], int]]]   = None
SAMPLE_DATA1            : Optional[List[Tuple[List[int], int]]]   = None
SAMPLE_DATA2            : Optional[List[Tuple[List[int], int]]]   = None



################################################################################
#           1.2.    INITIALIZATION FUNCTIONS.
################################################################################

#   "init"
#
def init(*, allow_overwrite: bool = False, **kwargs: Any) -> None:
    global _INITIALIZED, SAMPLE_DATA0, SAMPLE_DATA1, SAMPLE_DATA2

    _load_data()    #  POPULATE THE "SAMPLE_DATA*" ARRAYS...

    if _INITIALIZED and (not allow_overwrite):
        raise RuntimeError("_fpga_stream.init() called more than once")

    for name, value in kwargs.items():
        # basic sanity checks
        if (not isinstance(name, str)) or (not name.isidentifier()):
            raise ValueError(f"invalid init name: {name!r}")

        # enforce your global naming convention (optional but recommended)
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


#   "make_default_runtime_state"
#
def make_default_runtime_state() -> RuntimeState:
    integration_window  : float = 1.0
    coincidence_window  : int   = 50_000

    for spec in COMMAND_REGISTRY:
        if (spec.kind is not CommandKind.Param) or (spec.state_field is None):
            continue

        if spec.state_field == "integration_window":
            if spec.default_value is not None:
                integration_window = float(spec.default_value)

        elif spec.state_field == "coincidence_window":
            if spec.default_value is not None:
                coincidence_window = int(float(spec.default_value))

    return RuntimeState(
          integration_window      = integration_window
        , coincidence_window      = coincidence_window
    )


#   "add_seed_arguments"
#
def add_seed_arguments(parser: Any) -> None:
    """Register CLI seed args for seedable PARAM commands (registry-driven)."""

    for spec in COMMAND_REGISTRY:
        if (spec.kind is not CommandKind.Param) or (not spec.seedable):
            continue
        if spec.state_field is None:
            continue

        opt                 : str   = spec.cli_name if (spec.cli_name is not None) else spec.id.value
        arg                 : str   = f"--{opt}"
        help_text           : str   = f"Seed {spec.id.value} (overrides process defaults at startup)"

        if spec.value_type is ValueType.Float:
            parser.add_argument(arg, dest=spec.state_field, type=float, default=None, help=help_text)
        elif spec.value_type is ValueType.Int:
            parser.add_argument(arg, dest=spec.state_field, type=int, default=None, help=help_text)
        elif spec.value_type is ValueType.Str:
            parser.add_argument(arg, dest=spec.state_field, type=str, default=None, help=help_text)
        else:
            # NoneType should never be seedable
            continue

    return


#   "apply_seed_args"
#
def apply_seed_args(args: Any, state: RuntimeState) -> None:
    """Apply CLI seed values (if provided) into RuntimeState (registry-driven)."""

    for spec in COMMAND_REGISTRY:
        if (spec.kind is not CommandKind.Param) or (not spec.seedable):
            continue
        if spec.state_field is None:
            continue

        if not hasattr(args, spec.state_field):
            continue

        v = getattr(args, spec.state_field)
        if v is None:
            continue

        # validate/clamp
        if spec.value_type in (ValueType.Float, ValueType.Int):
            v_f     : float             = float(v)
            min_v   : Optional[float]    = spec.min_value
            max_v   : Optional[float]    = spec.max_value

            if spec.validation is ValidationPolicy.Reject:
                if (min_v is not None) and (v_f < min_v):
                    raise ValueError(f"seed value out of range for {spec.id.value}: {v_f} < {min_v}")
                if (max_v is not None) and (v_f > max_v):
                    raise ValueError(f"seed value out of range for {spec.id.value}: {v_f} > {max_v}")

            elif spec.validation is ValidationPolicy.Clamp:
                if (min_v is not None) and (v_f < min_v):
                    v_f = min_v
                if (max_v is not None) and (v_f > max_v):
                    v_f = max_v

            v = int(v_f) if (spec.value_type is ValueType.Int) else float(v_f)

        if not hasattr(state, spec.state_field):
            raise RuntimeError(f"RuntimeState missing field {spec.state_field!r} for command {spec.id.value!r}")

        setattr(state, spec.state_field, v)

    return



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
def measure_raw(session: Any) -> Tuple[List[int], int]:
    counts  : List[int]  = [int(x) for x in session.registers["Counts"].read()]
    cycles  : int        = int(session.registers["CYCLES"].read())

    return counts, cycles


#  "start_measure"
#
def start_measure(session: Any, coincidence_ticks: int = 1) -> None:
    counts  : List[int]     = []
    cycles  : int           = 0
    enable                  = session.registers["ENABLE"]
    clear                   = session.registers["CLEAR"]
    winreg                  = session.registers["Conicidence Window"]

    enable.write(False)
    clear.write(True)
    winreg.write(int(coincidence_ticks))

    counts, cycles = measure_raw(session)
    if cycles or any(counts):
        raise RuntimeError("Clear/Stop failed")

    clear.write(False)
    enable.write(True)

    return


#  "finish_measure"
#
def finish_measure(session: Any) -> Tuple[List[int], int]:
    finish_duration  : float    = float(globals().get("_MEASUREMENT_COMPLETION_DELAY", 0.1))
    enable                      = session.registers["ENABLE"]

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
#           3.1.    IPC OUTPUT (stdout JSON lines).
################################################################################

#   "_utc_timestamp_z_seconds"
#
def _utc_timestamp_z_seconds() -> str:
    ts_utc  : datetime.datetime = datetime.datetime.now(datetime.timezone.utc)
    ts_iso  : str               = ts_utc.isoformat(timespec="seconds")

    if ts_iso.endswith("+00:00"):
        ts_iso = ts_iso[:-6] + "Z"

    return ts_iso


#   "_make_record"
#
def _make_record(msg_type: MessageType, payload: Dict[str, Any]) -> Dict[str, Any]:
    ts      : str               = _utc_timestamp_z_seconds()
    record  : Dict[str, Any]    = {
          JsonKey.TYPE.value        : msg_type.value
        , JsonKey.VERSION.value     : IPC_SCHEMA_VERSION
        , JsonKey.TIME.value        : ts
    }
    record.update(payload)

    return record


#   "_make_data_record"
#
def _make_data_record(counts: List[int], cycles: int) -> Dict[str, Any]:
    payload : Dict[str, Any] = {
          JsonKey.CYCLES.value      : int(cycles)
        , JsonKey.COUNTS.value      : counts
    }
    return _make_record(MessageType.DATA, payload)


#   "emit_data_record"
#
def emit_data_record(counts: List[int], cycles: int) -> None:
    record      : Dict[str, Any]    = _make_data_record(counts, cycles)
    payload     : str               = json.dumps(record)

    sys.stdout.write(payload + "\n")
    sys.stdout.flush()

    return



################################################################################
#           3.2.    IPC INPUT (stdin -> queue).
################################################################################

#   "_parse_command_line"
#
def _parse_command_line(line: str) -> Optional[Command]:
    toks        : List[str]             = line.strip().split()
    cmd_text    : Optional[str]         = None
    spec        : Optional[CommandSpec] = None
    cmd_val     : CommandValue          = None

    if not toks:
        return None

    cmd_text = toks[0].lower()
    spec     = _COMMAND_BY_KEY.get(cmd_text, None)
    if spec is None:
        return None

    if spec.kind is CommandKind.Action:
        if spec.id is CommandID.QUIT:
            return Command(id=CommandID.QUIT, value=None)
        return None

    # PARAM
    if len(toks) != 2:
        return None

    raw = toks[1]

    try:
        if spec.value_type is ValueType.Float:
            cmd_val = float(raw)
        elif spec.value_type is ValueType.Int:
            cmd_val = int(raw, 0)
        elif spec.value_type is ValueType.Str:
            cmd_val = str(raw)
        else:
            return None
    except ValueError:
        return None

    # validate/clamp (numeric only)
    if spec.value_type in (ValueType.Float, ValueType.Int):
        v_f     : float             = float(cmd_val)
        min_v   : Optional[float]    = spec.min_value
        max_v   : Optional[float]    = spec.max_value

        if spec.validation is ValidationPolicy.Reject:
            if (min_v is not None) and (v_f < min_v):
                return None
            if (max_v is not None) and (v_f > max_v):
                return None

        elif spec.validation is ValidationPolicy.Clamp:
            if (min_v is not None) and (v_f < min_v):
                v_f = min_v
            if (max_v is not None) and (v_f > max_v):
                v_f = max_v

        cmd_val = int(v_f) if (spec.value_type is ValueType.Int) else float(v_f)

    return Command(id=spec.id, value=cmd_val)



################################################################################
#           3.3.    IPC UTILITY FUNCTIONS.
################################################################################

#   "drain_and_apply_commands"
#
def drain_and_apply_commands(command_queue: "queue.Queue[Command]", state: RuntimeState) -> None:
    cmd     : Optional[Command]     = None
    spec    : Optional[CommandSpec] = None

    while True:
        try:
            cmd = command_queue.get_nowait()
        except queue.Empty:
            break

        spec = _COMMAND_BY_ID.get(cmd.id, None)
        if spec is None:
            continue

        if spec.kind is CommandKind.Action:
            if cmd.id is CommandID.QUIT:
                state.quit_requested = True
            continue

        # PARAM
        if spec.state_field is None:
            continue
        if not hasattr(state, spec.state_field):
            continue

        if spec.value_type is ValueType.Float:
            setattr(state, spec.state_field, float(cmd.value))
        elif spec.value_type is ValueType.Int:
            setattr(state, spec.state_field, int(cmd.value))
        elif spec.value_type is ValueType.Str:
            setattr(state, spec.state_field, str(cmd.value))

    return



################################################################################
#           3.X.    IPC DAEMON FUNCTIONS.
################################################################################

#  "stdin_reader"
#
def stdin_reader(command_queue: "queue.Queue[Command]") -> None:
    """Accept text commands from the C++ host (one per line)."""

    line    : str               = ""
    cmd     : Optional[Command] = None

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

#   "mock_packets"
#
def mock_packets(data: List[Tuple[List[int], int]]):
    i       : int   = 0
    length  : int   = len(data)

    while True:
        counts, cycles  = data[i]
        jittered        = [
            int(random.poisson(mu) if (mu > 20) else mu)
                if hasattr(random, "poisson") else int(mu)
            for mu in counts
        ]
        yield jittered, cycles
        i = (i + 1) % length

    return


#   "_load_data"
#
def _load_data() -> None:
    global SAMPLE_DATA0, SAMPLE_DATA1, SAMPLE_DATA2

    SAMPLE_DATA1    = [
    #   UNUSED.     D.      C.      CD.     B.      BD.     BC.     BCD.    A.      AD.     AC.     ACD.    AB.     ABD.    ABC.    ABCD.       FPGA CYCLES.    #
    ##############################################################################################################################################################
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

    SAMPLE_DATA2    = [
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

    return



#
#
#
################################################################################
################################################################################    #   END [[ X.  "DATA SIMULATION" ]].
