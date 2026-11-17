# one scenario on a DUT in an ultimate provisioning (UP) station, the UP plays the scenario's car
import os
import signal
import subprocess
import time

import openpilot.cereal.messaging as messaging
from openpilot.common.basedir import BASEDIR
from openpilot.selfdrive.test.helpers import set_params_enabled
from openpilot.selfdrive.test.up.replay import DT, Frame
from openpilot.selfdrive.test.up.rules import NO_CAR_MODES
from openpilot.selfdrive.test.up.scenario import LENGTH, Scenario, Window

ONROAD_LIMIT = 120.
# after onroad, a car that doesn't get engageable fails, the rig is fine
ENGAGEABLE_LIMIT = 30.
RECORD = ('can', 'sendcan', 'pandaStates', 'selfdriveState', 'carState', 'onroadEvents')
# the panda marks its own tx echoes with src + 128
ECHO = 128


class RigProblem(Exception):
  pass


def split_preroll(frames: list[Frame], window: Window, rise: float) -> tuple[list[Frame], list[Frame]]:
  # the frames before the engage edge loop until openpilot is engageable, then the whole window plays once
  # rounded like window_frames counts frames, a float floor drops a frame at an exact boundary
  n = round((rise - window.start) / DT)
  if n <= 0:
    raise ValueError(f"no frames before the engage edge at {rise:.2f} s, window starts at {window.start:.2f} s")
  return frames[:n], frames


def onroad_missing(sm) -> list[str]:
  # what keeps openpilot from being onroad, empty once it is
  missing = []
  if not sm['deviceState'].started:
    missing.append("not started")
  if not sm['carState'].canValid:
    missing.append("carState not canValid")
  stopped = [p.name for p in sm['managerState'].processes if p.shouldBeRunning and not p.running]
  if stopped:
    missing.append(f"not running: {', '.join(stopped)}")
  if not len(sm['pandaStates']):
    missing.append("no panda")
  elif sm['pandaStates'][0].safetyModel.raw in NO_CAR_MODES:
    missing.append(f"panda in {sm['pandaStates'][0].safetyModel}")
  return missing


class Drive:
  def __init__(self, up):
    self.up = up
    self._manager: subprocess.Popen | None = None

  def _up(self, call: str, *args):
    # the UP not answering is the rack's problem
    try:
      return getattr(self.up, call)(*args)
    except Exception as e:
      raise RigProblem(f"rig problem: UP {call} failed: {e!r}") from e

  def _start_manager(self, scenario: Scenario) -> None:
    set_params_enabled()
    env = os.environ | {
      'FINGERPRINT': scenario.car_model,
      'SKIP_FW_QUERY': '1',
      'TESTING_CLOSET': '1',
      'BLOCK': 'soundd',
    }
    self._manager = subprocess.Popen(["python", os.path.join(BASEDIR, "openpilot/system/manager/manager.py")], env=env)

  def _stop_manager(self) -> None:
    if self._manager is None:
      return
    manager, self._manager = self._manager, None
    manager.send_signal(signal.SIGINT)
    try:
      manager.wait(60)
    except subprocess.TimeoutExpired:
      manager.kill()
      manager.wait()

  def _check_manager(self) -> None:
    assert self._manager is not None
    code = self._manager.poll()
    if code is not None:
      raise RuntimeError(f"manager exited with {code}")

  def _wait_onroad(self, sm, can_sock, buses: set[int]) -> None:
    seen: set[int] = set()
    panda_seen = False
    end = time.monotonic() + ONROAD_LIMIT
    while True:
      self._check_manager()
      sm.update(10)
      for m in messaging.drain_sock(can_sock):
        seen |= {c.src for c in m.can if c.src < ECHO}
      panda_seen = panda_seen or len(sm['pandaStates']) > 0
      missing = onroad_missing(sm)
      if not missing:
        break
      if time.monotonic() > end:
        if not panda_seen:
          raise RigProblem("rig problem: panda not answering")
        if buses - seen:
          raise RigProblem(f"rig problem: no CAN on bus {sorted(buses - seen)}")
        raise RigProblem(f"rig problem: not onroad after {ONROAD_LIMIT:g} s: {', '.join(missing)}")
    if buses - seen:
      raise RigProblem(f"rig problem: no CAN on bus {sorted(buses - seen)}")

  def _wait_engageable(self, sm) -> None:
    end = time.monotonic() + ENGAGEABLE_LIMIT
    while not sm['selfdriveState'].engageable:
      if time.monotonic() > end:
        events = [str(e.name) for e in sm['onroadEvents']]
        raise TimeoutError(f"not engageable {ENGAGEABLE_LIMIT:g} s after onroad, events: {events}")
      self._check_manager()
      sm.update(10)

  def _record(self, window: list[Frame]) -> list:
    socks = [messaging.sub_sock(s, conflate=False) for s in RECORD]
    # the window plays once from the call, stop before it loops
    end = time.monotonic() + LENGTH
    self._up('can_replay', window)
    # what came in while the UP switched over is still the preroll
    for sock in socks:
      messaging.drain_sock_raw(sock)
    msgs = []
    while time.monotonic() < end:
      self._check_manager()
      for sock in socks:
        msgs += messaging.drain_sock(sock)
      time.sleep(DT)
    for sock in socks:
      msgs += messaging.drain_sock(sock)
    return msgs

  def run(self, scenario: Scenario, frames: list[Frame]) -> list:
    preroll, window = split_preroll(frames, scenario.window, scenario.rise)
    buses = {bus for frame in preroll for _, _, bus in frame}
    sm = messaging.SubMaster(['deviceState', 'carState', 'managerState', 'pandaStates', 'selfdriveState', 'onroadEvents'])
    can_sock = messaging.sub_sock('can', conflate=False)
    self._start_manager(scenario)
    try:
      self._up('reset')
      self._up('set_ignition', True)
      self._up('can_replay', preroll)
      self._wait_onroad(sm, can_sock, buses)
      self._wait_engageable(sm)
      return self._record(window)
    finally:
      try:
        self._up('reset')
      finally:
        self._stop_manager()

  def close(self) -> None:
    try:
      self._stop_manager()
    finally:
      self._up('reset')
