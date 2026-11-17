import math
from collections import Counter
from collections.abc import Callable, Iterable
from dataclasses import dataclass

from opendbc.can.dbc import DBC, Msg, SignalType
from opendbc.can.packer import CANPacker
from opendbc.can.parser import MAX_BAD_COUNTER
from opendbc.car import structs
from opendbc.car.can_definitions import CanData
from opendbc.car.car_helpers import interfaces

from openpilot.cereal.messaging import new_message
from openpilot.selfdrive.pandad import can_capnp_to_list
from openpilot.selfdrive.test.up.oracle import PANDA_BUSES, load_libsafety, run_safety
from openpilot.selfdrive.test.up.scenario import Scenario, Window, build_candidates, load_log, safety_pair

# the UP's can_replay frame period
DT = 0.01
# (addr, data, bus) of each message in a 10 ms frame
Frame = list[tuple[int, bytes, int]]
# the first seconds are for the parsers to learn rates and the panda's RX checks to fill
STARTUP = 2.
# frames/s of a station's lanes for buses 0-2, measured on AGNOS 20.1
BUDGET = {0: 230., 1: 130., 2: 60.}


def _bytes(msg) -> bytes:
  return (msg.as_builder() if hasattr(msg, 'as_builder') else msg).to_bytes()


def window_frames(msgs: Iterable, window: Window) -> list[Frame]:
  msgs = [m for m in msgs if m.which() == 'can']
  if not msgs:
    return []
  first = min(m.logMonoTime for m in msgs)
  frames: list[Frame] = [[] for _ in range(round((window.end - window.start) / DT))]
  parsed = can_capnp_to_list([_bytes(m) for m in msgs])
  for m, (_, can) in zip(msgs, parsed, strict=True):
    i = int(((m.logMonoTime - first) / 1e9 - window.start) // DT)
    if not 0 <= i < len(frames):
      continue
    for addr, dat, src in can:
      # the first panda's buses 0-2, not its tx echoes (src + 128) or another panda's buses (src + 4)
      if src < PANDA_BUSES - 1:
        frames[i].append((addr, dat, src))
  return frames


def _parsers(CP: structs.CarParams) -> list:
  interface = interfaces[CP.carFingerprint]
  car = interface(CP)
  radar = interface.RadarInterface(CP)
  return [car, radar]


def _packets(frames: list[Frame]) -> list[tuple[int, list[CanData]]]:
  return [(round(i * DT * 1e9), [CanData(a, d, b) for a, d, b in f]) for i, f in enumerate(frames)]


def needed_messages(CP: structs.CarParams, frames: list[Frame]) -> set[tuple[int, int]]:
  car, radar = _parsers(CP)
  for packet in _packets(frames):
    car.update([packet])
    radar.update([packet])
  # the parsers only add the messages they read once they run
  parsers = [p for p in car.can_parsers.values() if p is not None]
  if radar.rcp is not None:
    parsers.append(radar.rcp)
  needed = set(_rx_checks(CP))
  for p in parsers:
    for addr in p.message_states:
      needed.add((p.bus, addr))
  return needed


def needed_frames(CP: structs.CarParams, frames: list[Frame]) -> list[Frame]:
  needed = needed_messages(CP, frames)
  return [[f for f in frame if (f[2], f[0]) in needed] for frame in frames]


def _rx_checks(CP: structs.CarParams) -> dict[tuple[int, int], bool]:
  # (bus, addr) of every message the panda checks -> whether it checks its counter
  mode, param = safety_pair(CP)
  safety = load_libsafety()
  if safety.set_safety_hooks(mode, param) != 0:
    raise ValueError(f"libsafety has no safety mode {mode} param {param}")
  cfg = safety.safety_a.setup.current_safety_config
  checks = {}
  for i in range(cfg.rx_checks_len):
    for m in cfg.rx_checks[i].msg:
      if m.addr:
        checks[(m.bus, m.addr)] = m.max_counter > 0 and not m.ignore_counter
  return checks


def _dbcs(CP: structs.CarParams) -> dict[tuple[int, int], str]:
  # (bus, addr) -> name of the parser DBC that describes it
  car, radar = _parsers(CP)
  parsers = [p for p in car.can_parsers.values() if p is not None]
  if radar.rcp is not None:
    parsers.append(radar.rcp)
  dbcs: dict[tuple[int, int], str] = {}
  for p in parsers:
    for addr in p.dbc.addr_to_msg:
      dbcs.setdefault((p.bus, addr), p.dbc_name)
  return dbcs


def _is_counter(sig) -> bool:
  return sig.type == SignalType.COUNTER or sig.name == "COUNTER"


def _checked(msg: Msg) -> bool:
  return any(_is_counter(sig) or sig.calc_checksum is not None for sig in msg.sigs.values())


def _values(msg: Msg, dat: bytes) -> dict[str, float]:
  values = {}
  for sig in msg.sigs.values():
    if _is_counter(sig) or sig.calc_checksum is not None:
      continue
    raw = sig.get_raw_value(dat)
    if sig.is_signed:
      raw -= ((raw >> (sig.size - 1)) & 0x1) * (1 << sig.size)
    values[sig.name] = raw * sig.factor + sig.offset
  return values


def thin(CP: structs.CarParams, frames: list[Frame], budget: dict[int, float]) -> list[Frame]:
  seconds = len(frames) * DT
  load = Counter(bus for f in frames for _, _, bus in f)
  # every k-th frame of each message on a bus
  keep = {}
  for bus, rate in budget.items():
    if load[bus]:
      keep[bus] = max(1, math.ceil(load[bus] / seconds / rate))
  dbcs = _dbcs(CP)
  parsed = {name: DBC(name) for name in set(dbcs.values())}
  rx_checks = _rx_checks(CP)
  # one packer per bus and DBC, so each bus continues its own counters
  packers: dict[tuple[int, str], CANPacker] = {}
  seen: Counter[tuple[int, int]] = Counter()
  out: list[Frame] = []
  for frame in frames:
    kept: Frame = []
    for addr, dat, bus in frame:
      n = seen[(bus, addr)]
      seen[(bus, addr)] += 1
      k = keep.get(bus, 1)
      dbc = dbcs.get((bus, addr))
      msg = parsed[dbc].addr_to_msg[addr] if dbc is not None else None
      # the panda checks this message and no DBC can repack it or its counter, so it keeps every frame
      if (bus, addr) in rx_checks:
        if msg is None:
          k = 1
        elif rx_checks[(bus, addr)] and not any(_is_counter(sig) for sig in msg.sigs.values()):
          k = 1
      if n % k:
        continue
      # a kept frame skips k - 1 counter steps, repack it so the counter and checksum stay consecutive
      if k > 1 and msg is not None and dbc is not None and _checked(msg):
        packer = packers.setdefault((bus, dbc), CANPacker(dbc))
        addr, dat, bus = packer.make_can_msg(addr, bus, _values(msg, dat))
      kept.append((addr, dat, bus))
    out.append(kept)
  return out


def over_budget(frames: list[Frame], budget: dict[int, float]) -> list[str]:
  seconds = len(frames) * DT
  load = Counter(bus for f in frames for _, _, bus in f)
  over = []
  for bus, rate in sorted(budget.items()):
    if load[bus] / seconds > rate:
      over.append(f"bus {bus} {load[bus] / seconds:.0f} frames/s > {rate:.0f}")
  return over


def validate(CP: structs.CarParams, frames: list[Frame]) -> list[str]:
  problems = []
  msgs = []
  for i, frame in enumerate(frames):
    msg = new_message('can', len(frame))
    msg.logMonoTime = i * round(DT * 1e9)
    for c, (addr, dat, bus) in zip(msg.can, frame, strict=True):
      c.address, c.dat, c.src = addr, dat, bus
    msgs.append(msg)
  run = run_safety(msgs, *safety_pair(CP))
  problems += sorted({f"panda rejects 0x{addr:x} bus {bus}" for _, addr, bus in run.rx_rejected})
  if any(t > STARTUP for t in run.rx_checks_invalid):
    problems.append("panda RX checks invalid")
  # the route's own cruise and buttons engage the rack's panda, without a heartbeat it drops controls again after a few ticks
  if not any(allowed for _, allowed in run.controls_allowed):
    problems.append("panda never engages")

  car, radar = _parsers(CP)
  can_valid = True
  radar_errors: set[str] = set()
  radar_seen = False
  for i, packet in enumerate(_packets(frames)):
    state = car.update([packet])
    data = radar.update([packet])
    if i * DT > STARTUP:
      can_valid = can_valid and state.canValid
      if data is not None:
        radar_seen = True
        radar_errors |= {name for name in data.errors.schema.fieldnames if getattr(data.errors, name)}
  if not can_valid:
    problems.append(f"openpilot can't parse: {_invalid(car)}")
  # a radar that reads CAN publishes nothing once its trigger message is gone
  if radar.rcp is not None and not radar_seen:
    radar_errors.add("no radar data")
  if radar_errors:
    problems.append(f"openpilot radar errors: {sorted(radar_errors)}")
  return problems


def _invalid(car) -> list[str]:
  invalid = []
  for p in car.can_parsers.values():
    if p is None:
      continue
    bus_timeout = p.bus_timeout
    for addr, st in p.message_states.items():
      if st.counter_fail >= MAX_BAD_COUNTER or not st.valid(p._last_update_nanos, bus_timeout):
        invalid.append(f"0x{addr:x} bus {p.bus}")
  return invalid


@dataclass
class Replay:
  # what a station sends, one frame per DT
  frames: list[Frame]
  # what validate finds wrong with it
  problems: list[str]
  # why a station's lanes can't carry it, empty if they can
  lane_problems: list[str]


def build_replay(CP: structs.CarParams, msgs: list, window: Window, budget: dict[int, float]) -> Replay:
  needed = needed_frames(CP, window_frames(msgs, window))
  frames = thin(CP, needed, budget)
  problems = validate(CP, frames)
  # a replay over the lanes' rate, or one only thinning breaks, can't run on the station
  lane_problems = over_budget(frames, budget)
  if not lane_problems and problems and not validate(CP, needed):
    lane_problems = problems
  return Replay(frames, problems, lane_problems)


def pick_replay(candidates: list[Scenario], budget: dict[int, float],
                load: Callable[[Scenario], list] = load_log) -> tuple[Scenario, list, Replay]:
  # a pair's scenarios in routes order, the first whose replay fits the station's lanes, or else the last
  for s in candidates:
    msgs = load(s)
    replay = build_replay(s.CP, msgs, s.window, budget)
    if not replay.lane_problems:
      break
  return s, msgs, replay


def pick_scenarios(routes, budget: dict[int, float], build: Callable = build_candidates, pick: Callable = pick_replay) -> tuple[list[Scenario], list[str]]:
  # what a rack run drives: each pair's scenario after the lane budget fallback, and the gaps
  candidates, gaps = build(routes)
  scenarios = []
  for pair_candidates in candidates.values():
    try:
      s, _, replay = pick(pair_candidates, budget)
    except Exception as e:
      first = pair_candidates[0]
      gaps.append(f"{first.car_model} {first.route}: {e!r}")
      continue
    if replay.lane_problems:
      gaps.append(f"mode {s.mode} param {s.param}: lane budget: {', '.join(replay.lane_problems)}")
    else:
      scenarios.append(s)
  return scenarios, gaps
