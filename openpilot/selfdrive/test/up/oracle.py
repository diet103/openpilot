import atexit
import bisect
import os
import shutil
import tempfile
from collections import Counter
from dataclasses import dataclass, field
from functools import cache

from opendbc.safety.tests.libsafety import libsafety_py

# panda ticks between openpilot's heartbeat saying disengaged and the panda dropping controls_allowed, safety.h heartbeat_check_copy
HEARTBEAT_LAG = 3
# the panda marks what it sent with src + 128 and what it blocked with src + 192
TX = 128
BLOCKED = 192
# pandad offsets each panda's buses by 4, pandaStates[0] is the panda on buses 0-3
PANDA_BUSES = 4


@dataclass
class SafetyRun:
  commands: list[tuple[float, int, int, bool, bool]] = field(default_factory=list)
  samples: list[tuple[float, bool, bool, int, int]] = field(default_factory=list)
  rx_rejected: list[tuple[float, int, int]] = field(default_factory=list)
  rx_checks_invalid_at: float | None = None
  # libsafety's controls_allowed from the first can message on, (t, value) at each change
  controls_allowed: list[tuple[float, bool]] = field(default_factory=list)


@cache
def _build() -> str:
  path = libsafety_py._build_libsafety()
  atexit.register(os.unlink, path)
  return path


def load_libsafety():
  # a copy of the build is a fresh library, no safety state carries over from another recording
  fd, copy = tempfile.mkstemp(suffix=".so")
  os.close(fd)
  shutil.copy(_build(), copy)
  libsafety_py.load(copy)
  os.unlink(copy)
  return libsafety_py.libsafety


def _first_can(msgs: list) -> int | None:
  return min((m.logMonoTime for m in msgs if m.which() == 'can'), default=None)


def _heartbeat_source(msgs: list) -> str:
  # logs from before selfdriveState have openpilot's engaged state in controlsState
  return 'selfdriveState' if any(m.which() == 'selfdriveState' for m in msgs) else 'controlsState'


def _enabled(m) -> bool:
  return bool(m.selfdriveState.enabled if m.which() == 'selfdriveState' else m.controlsState.deprecated.enabled)


def _engaged(msgs: list) -> list[tuple[float, bool]]:
  first = _first_can(msgs)
  if first is None:
    return []
  source = _heartbeat_source(msgs)
  return [((m.logMonoTime - first) / 1e9, _enabled(m)) for m in sorted(msgs, key=lambda m: m.logMonoTime) if m.which() == source]


def _panda_state(m):
  if m.which() == 'pandaStates' and len(m.pandaStates):
    return m.pandaStates[0]
  if m.which() == 'pandaStateDEPRECATED':
    return m.pandaStateDEPRECATED
  return None


def run_safety(msgs: list, mode: int, param: int) -> SafetyRun:
  run = SafetyRun()
  msgs = sorted(msgs, key=lambda m: m.logMonoTime)
  first = _first_can(msgs)
  if first is None:
    return run

  safety = load_libsafety()
  if safety.set_safety_hooks(mode, param) != 0:
    raise ValueError(f"libsafety has no safety mode {mode} param {param}")
  heartbeat_source = _heartbeat_source(msgs)
  # the panda's own alternative experience, as it logged it before the first can message
  alt_exp = next((int(ps.alternativeExperience) for m in msgs if m.logMonoTime < first and (ps := _panda_state(m)) is not None), 0)
  safety.set_alternative_experience(alt_exp)

  def note_controls(t: float) -> None:
    allowed = bool(safety.get_controls_allowed())
    if not run.controls_allowed or run.controls_allowed[-1][1] != allowed:
      run.controls_allowed.append((t, allowed))

  note_controls(0.)
  pending: Counter[tuple[int, int, bytes]] = Counter()
  # the timer crosses its first whole second one second after the first message
  next_tick = 1
  for m in msgs:
    which = m.which()
    t_ns = m.logMonoTime - first
    t = t_ns / 1e9

    while t_ns >= 0 and next_tick * 1_000_000_000 <= t_ns:
      safety.set_timer(next_tick * 1_000_000 % 2**32)
      # the panda checks the heartbeat before its safety tick, main.c
      safety.safety_heartbeat_check()
      safety.safety_tick()
      if run.rx_checks_invalid_at is None and safety.safety_a.state.safety_rx_checks_invalid:
        run.rx_checks_invalid_at = float(next_tick)
      note_controls(float(next_tick))
      next_tick += 1

    if which == heartbeat_source:
      safety.safety_heartbeat(_enabled(m))
    elif which == 'sendcan':
      for c in m.sendcan:
        pending[(c.src, c.address, bytes(c.dat))] += 1
    elif which == 'can':
      safety.set_timer(t_ns // 1000 % 2**32)
      for c in m.can:
        if c.src >= TX:
          bus = c.src - (BLOCKED if c.src >= BLOCKED else TX)
          key = (bus, c.address, bytes(c.dat))
          # an echo is a command only if openpilot sent it, the rest are the panda's own forwarded frames
          if bus >= PANDA_BUSES or pending[key] == 0:
            continue
          pending[key] -= 1
          allowed = bool(safety.safety_tx_hook(libsafety_py.make_CANPacket(c.address, bus, c.dat)))
          run.commands.append((t, c.address, bus, c.src < BLOCKED, allowed))
        elif c.src < PANDA_BUSES:
          safety.safety_fwd_hook(c.src, c.address)
          if not safety.safety_rx_hook(libsafety_py.make_CANPacket(c.address, c.src, c.dat)):
            run.rx_rejected.append((t, c.address, c.src))
          note_controls(t)
    elif (ps := _panda_state(m)) is not None:
      if ps.alternativeExperience != alt_exp:
        alt_exp = int(ps.alternativeExperience)
        safety.set_alternative_experience(alt_exp)
      if t_ns < 0:
        continue
      run.samples.append((t, bool(ps.controlsAllowed), bool(safety.get_controls_allowed()), int(ps.safetyRxInvalid), len(run.rx_rejected)))
  return run


def _allowed(a: bool) -> str:
  return 'allowed' if a else 'blocked'


def command_mismatches(run: SafetyRun) -> list[str]:
  return [f"t={t:.2f} TX 0x{addr:x} bus {bus}: panda {_allowed(a)}, libsafety {_allowed(b)}" for t, addr, bus, a, b in run.commands if a != b]


def _held(run: SafetyRun, start: float, end: float) -> set[bool]:
  # every value libsafety's controls_allowed had during (start, end]
  times = [t for t, _ in run.controls_allowed]
  i, j = bisect.bisect_right(times, start), bisect.bisect_right(times, end)
  return {v for _, v in run.controls_allowed[max(i - 1, 0):j]}


def state_mismatches(run: SafetyRun, msgs: list) -> list[str]:
  engaged = _engaged(msgs)
  changes = [t for (_, prev), (t, now) in zip(engaged, engaged[1:], strict=False) if prev != now]
  intervals = [(tc, tc + HEARTBEAT_LAG + 1) for tc in changes]
  mismatches = []
  prev = float('-inf')
  for t, panda, lib, _, _ in run.samples:
    start, prev = prev, t
    # pandad reads the panda's health and its CAN in separate loops, so a sample shows the panda at some moment
    # since the previous sample, accepted if libsafety held the panda's value at any point since then
    if panda in _held(run, start, t):
      continue
    # the panda and libsafety tick at different moments, so after openpilot engages or disengages they may drop
    # controls up to HEARTBEAT_LAG + 1 ticks apart, accepted if libsafety reaches the panda's value within that interval
    if any(start <= t <= end and any(start <= ts <= end and v == panda for ts, _, v, _, _ in run.samples) for start, end in intervals):
      continue
    mismatches.append(f"t={t:.2f} controls_allowed: panda {panda}, libsafety {lib}")

  if not run.samples:
    return mismatches
  t0, _, _, panda0, lib0 = run.samples[0]
  counts = [(t, panda - panda0, lib - lib0) for t, _, _, panda, lib in run.samples]
  reported = 0
  # the same health and CAN loop skew moves a rejected frame into a neighboring sample, so a difference counts
  # only if it is still there at the next sample, reported once where it first appeared
  for (t, panda, lib), (_, next_panda, next_lib) in zip(counts, counts[1:], strict=False):
    if panda - lib != reported and next_panda - next_lib == panda - lib:
      reported = panda - lib
      mismatches.append(f"t={t:.2f} RX invalid since t={t0:.2f}: panda {panda}, libsafety {lib}")
  return mismatches


def check_commands(msgs: list, mode: int, param: int) -> list[str]:
  return command_mismatches(run_safety(msgs, mode, param))


def check_state(msgs: list, mode: int, param: int) -> list[str]:
  return state_mismatches(run_safety(msgs, mode, param), msgs)
