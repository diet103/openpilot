import bisect
from dataclasses import dataclass

from opendbc.car.structs import CarParams

# seconds after the panda's engage edge by which openpilot must be enabled
ENGAGE_LIMIT = 2.
# the panda marks what it sent with src + 128 and what it blocked with src + 192
BLOCKED = 192
BUSES = 3
NO_CAR_MODES = (
  int(CarParams.SafetyModel.noOutput),
  int(CarParams.SafetyModel.silent),
  int(CarParams.SafetyModel.elm327),
)


@dataclass
class Recording:
  msgs: list
  start: int
  # message index and enabled of each selfdriveState
  sd_idx: list[int]
  sd_enabled: list[bool]

  def t(self, i: int) -> float:
    return (self.msgs[i].logMonoTime - self.start) / 1e9

  def next_sd(self, i: int) -> int | None:
    # position in sd_idx of the first selfdriveState after message i
    pos = bisect.bisect_right(self.sd_idx, i)
    return pos if pos < len(self.sd_idx) else None

  def enabled_at(self, i: int) -> bool:
    pos = bisect.bisect_right(self.sd_idx, i) - 1
    return pos >= 0 and self.sd_enabled[pos]

  def stays_enabled(self, i: int) -> bool:
    # the next selfdriveState after message i is still enabled, or there is none
    pos = self.next_sd(i)
    return pos is None or self.sd_enabled[pos]

  def spans(self) -> list[tuple[int, int]]:
    # (first, last) positions in sd_idx of each run of enabled samples
    spans = []
    first = None
    for pos, enabled in enumerate(self.sd_enabled):
      if enabled and first is None:
        first = pos
      if not enabled and first is not None:
        spans.append((first, pos - 1))
        first = None
    if first is not None:
      spans.append((first, len(self.sd_enabled) - 1))
    return spans


def _load(msgs: list) -> Recording:
  msgs = sorted(msgs, key=lambda m: m.logMonoTime)
  sd_idx = []
  sd_enabled = []
  for i, msg in enumerate(msgs):
    if msg.which() == 'selfdriveState':
      sd_idx.append(i)
      sd_enabled.append(msg.selfdriveState.enabled)
  return Recording(msgs, msgs[0].logMonoTime if msgs else 0, sd_idx, sd_enabled)


def _panda_states(rec: Recording):
  for i, msg in enumerate(rec.msgs):
    if msg.which() == 'pandaStates' and len(msg.pandaStates) > 0:
      yield i, msg.pandaStates[0]


def _span_number(rec: Recording, i: int) -> int:
  # which engaged span message i is in, by the number of enable edges before it
  edges = 0
  prev = False
  for pos, idx in enumerate(rec.sd_idx):
    if idx > i:
      break
    if rec.sd_enabled[pos] and not prev:
      edges += 1
    prev = rec.sd_enabled[pos]
  return edges


def _not_engaged(rec: Recording) -> list[tuple[float, str]]:
  failures = []
  prev_allowed = None
  for i, state in _panda_states(rec):
    allowed = state.controlsAllowed
    if allowed and prev_allowed is False:
      edge = rec.t(i)
      engaged = rec.enabled_at(i)
      for pos, idx in enumerate(rec.sd_idx):
        if idx > i and rec.t(idx) <= edge + ENGAGE_LIMIT and rec.sd_enabled[pos]:
          engaged = True
      if not engaged:
        failures.append((edge, f"t={edge:.2f} not engaged, openpilot not enabled within {ENGAGE_LIMIT:.0f}s of controls_allowed"))
    prev_allowed = allowed
  return failures


def _blocked_while_engaged(rec: Recording) -> list[tuple[float, str]]:
  failures = []
  reported = set()
  for i, msg in enumerate(rec.msgs):
    if msg.which() != 'can' or not rec.enabled_at(i) or not rec.stays_enabled(i):
      continue
    for frame in msg.can:
      bus = frame.src - BLOCKED
      key = (_span_number(rec, i), frame.address, bus)
      if bus in range(BUSES) and key not in reported:
        reported.add(key)
        t = rec.t(i)
        failures.append((t, f"t={t:.2f} blocked while engaged, addr {frame.address:#x} bus {bus}"))
  return failures


def _controls_allowed_dropped(rec: Recording) -> list[tuple[float, str]]:
  failures = []
  reported = set()
  for i, state in _panda_states(rec):
    if state.controlsAllowed or not rec.enabled_at(i) or not rec.stays_enabled(i):
      continue
    span = _span_number(rec, i)
    # enabled for longer than the engage limit
    pos = bisect.bisect_right(rec.sd_idx, i) - 1
    while pos > 0 and rec.sd_enabled[pos - 1]:
      pos -= 1
    t = rec.t(i)
    if t - rec.t(rec.sd_idx[pos]) > ENGAGE_LIMIT and span not in reported:
      reported.add(span)
      failures.append((t, f"t={t:.2f} controls_allowed false while engaged"))
  return failures


def _commands_stopped(rec: Recording) -> list[tuple[float, str]]:
  failures = []
  sendcan_idx = [i for i, msg in enumerate(rec.msgs) if msg.which() == 'sendcan' and len(msg.sendcan) > 0]
  for first, last in rec.spans():
    for pos in range(first + 1, last + 1):
      lo = rec.sd_idx[pos - 1]
      hi = rec.sd_idx[pos]
      if bisect.bisect_left(sendcan_idx, hi) - bisect.bisect_right(sendcan_idx, lo) == 0:
        t = rec.t(hi)
        failures.append((t, f"t={t:.2f} commands stopped while engaged"))
        break
  return failures


def _safety_pair(rec: Recording, mode: int, param: int) -> list[tuple[float, str]]:
  failures = []
  car_mode_seen = False
  reported = None
  for i, state in _panda_states(rec):
    logged = state.safetyModel.raw
    if logged not in NO_CAR_MODES:
      car_mode_seen = True
    pair = (logged, state.safetyParam)
    if car_mode_seen and pair != (mode, param) and pair != reported:
      reported = pair
      t = rec.t(i)
      failures.append((t, f"t={t:.2f} safety pair {pair} != {(mode, param)}"))
  return failures


def _faults(rec: Recording) -> list[tuple[float, str]]:
  failures = []
  reported = set()
  for i, state in _panda_states(rec):
    for fault in state.faults:
      name = str(fault)
      if name not in reported:
        reported.add(name)
        t = rec.t(i)
        failures.append((t, f"t={t:.2f} new panda fault {name}"))
  return failures


def rule_failures(msgs: list, mode: int, param: int) -> list[str]:
  rec = _load(msgs)
  failures = []
  failures += _not_engaged(rec)
  failures += _blocked_while_engaged(rec)
  failures += _controls_allowed_dropped(rec)
  failures += _commands_stopped(rec)
  failures += _safety_pair(rec, mode, param)
  failures += _faults(rec)
  return [line for _, line in sorted(failures, key=lambda f: f[0])]
