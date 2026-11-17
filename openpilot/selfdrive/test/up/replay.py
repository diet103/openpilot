import math
from collections import Counter
from collections.abc import Iterable

from opendbc.can.parser import MAX_BAD_COUNTER
from opendbc.car.can_definitions import CanData
from opendbc.car.car_helpers import interfaces

from openpilot.cereal.messaging import new_message
from openpilot.selfdrive.pandad import can_capnp_to_list
from openpilot.selfdrive.test.up.oracle import PANDA_BUSES, load_libsafety, run_safety
from openpilot.selfdrive.test.up.scenario import Window

# the UP's can_replay frame period
DT = 0.01
# (addr, data, bus) of each message in a 10 ms frame
Frame = list[tuple[int, bytes, int]]
# the first seconds are for the parsers to learn rates and the panda's RX checks to fill
STARTUP = 2.


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


def _parsers(platform: str) -> list:
  interface = interfaces[platform]
  CP = interface.get_non_essential_params(platform)
  car = interface(CP)
  radar = interface.RadarInterface(CP)
  return [car, radar]


def _packets(frames: list[Frame]) -> list[tuple[int, list[CanData]]]:
  return [(round(i * DT * 1e9), [CanData(a, d, b) for a, d, b in f]) for i, f in enumerate(frames)]


def needed_messages(platform: str, mode: int, param: int, frames: list[Frame]) -> set[tuple[int, int]]:
  car, radar = _parsers(platform)
  for packet in _packets(frames):
    car.update([packet])
    radar.update([packet])
  # the parsers only add the messages they read once they run
  parsers = [p for p in car.can_parsers.values() if p is not None]
  if radar.rcp is not None:
    parsers.append(radar.rcp)
  needed = {(p.bus, addr) for p in parsers for addr in p.message_states}

  safety = load_libsafety()
  if safety.set_safety_hooks(mode, param) == 0:
    cfg = safety.safety_a.setup.current_safety_config
    needed |= {(cfg.rx_checks[i].msg[j].bus, cfg.rx_checks[i].msg[j].addr) for i in range(cfg.rx_checks_len) for j in range(len(cfg.rx_checks[i].msg))
               if cfg.rx_checks[i].msg[j].addr}
  return needed


def thin(frames: list[Frame], budget: dict[int, float]) -> list[Frame]:
  seconds = len(frames) * DT
  load = Counter(bus for f in frames for _, _, bus in f)
  keep = {bus: max(1, math.ceil(load[bus] / seconds / rate)) for bus, rate in budget.items() if load[bus]}
  seen: Counter[tuple[int, int]] = Counter()
  out: list[Frame] = []
  for frame in frames:
    kept: Frame = []
    for addr, dat, bus in frame:
      n = seen[(bus, addr)]
      seen[(bus, addr)] += 1
      if n % keep.get(bus, 1) == 0:
        kept.append((addr, dat, bus))
    out.append(kept)
  return out


def validate(platform: str, mode: int, param: int, frames: list[Frame]) -> list[str]:
  problems = []
  msgs = []
  for i, frame in enumerate(frames):
    msg = new_message('can', len(frame))
    msg.logMonoTime = i * round(DT * 1e9)
    for c, (addr, dat, bus) in zip(msg.can, frame, strict=True):
      c.address, c.dat, c.src = addr, dat, bus
    msgs.append(msg)
  run = run_safety(msgs, mode, param)
  problems += sorted({f"panda rejects 0x{addr:x} bus {bus}" for _, addr, bus in run.rx_rejected})
  if run.rx_checks_invalid_at is not None and run.rx_checks_invalid_at > STARTUP:
    problems.append("panda RX checks invalid")

  car, radar = _parsers(platform)
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
