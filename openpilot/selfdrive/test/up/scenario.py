import os
import pickle
import subprocess
import tempfile
from collections.abc import Callable, Iterable
from dataclasses import dataclass
from pathlib import Path

from opendbc.car import structs
from opendbc.car.car_helpers import interfaces
from opendbc.car.logreader import LogReader as OpendbcLogReader
from opendbc.car.tests.routes import CarTestRoute, routes as test_routes
from opendbc.car.tests.test_models import TestCarModelBase, get_cached_segment

from openpilot.common.basedir import BASEDIR
from openpilot.tools.lib.logreader import LogReader

LEAD = 5.
LENGTH = 30.
MIN_ENGAGED = 10.


@dataclass(frozen=True)
class Window:
  start: float
  end: float


@dataclass(frozen=True)
class Scenario:
  route: str
  segment: int
  car_model: str
  mode: int
  param: int
  window: Window
  CP: structs.CarParams


def engaged_samples(msgs: Iterable) -> list[tuple[float, bool]]:
  samples = []
  first = None
  msgs = list(msgs)
  for m in msgs:
    if m.which() == 'can':
      first = m.logMonoTime
      break
  if first is None:
    return []
  for m in msgs:
    which = m.which()
    if which == 'pandaStates' and len(m.pandaStates):
      allowed = m.pandaStates[0].controlsAllowed
    elif which == 'pandaStateDEPRECATED':
      allowed = m.pandaStateDEPRECATED.controlsAllowed
    else:
      continue
    samples.append(((m.logMonoTime - first) / 1e9, bool(allowed)))
  return samples


def can_span(msgs: Iterable) -> float:
  times = [m.logMonoTime for m in msgs if m.which() == 'can']
  return (max(times) - min(times)) / 1e9 if times else 0.


def pick_window(samples: list[tuple[float, bool]], can_end: float) -> Window | None:
  best = None
  best_len = 0.
  # (first not allowed sample before the rise, rise) of the current stretch
  stretch = None
  down = None
  # a stretch starts where the panda goes from not allowed to allowed, the rack's panda starts disengaged
  # and has to engage from the replay, so one engaged since the first sample has no usable start
  # a stretch ends at the first sample that is not engaged, or at the last sample
  for i, (t, allowed) in enumerate(samples):
    if allowed and stretch is None and down is not None:
      stretch = (down, t)
    if not allowed and (i == 0 or samples[i - 1][1]):
      down = t
    last = i == len(samples) - 1
    if stretch is not None and (not allowed or last):
      before, rise = stretch
      length = (can_end if allowed else t) - rise
      # the window starts where the panda is not allowed, after any earlier stretch
      start = max(0., before, min(rise - LEAD, can_end - LENGTH))
      if length >= MIN_ENGAGED and length > best_len and start + LENGTH <= can_end:
        best, best_len = start, length
      stretch = None
  if best is None:
    return None
  return Window(best, best + LENGTH)


def safety_pair(CP: structs.CarParams) -> tuple[int, int]:
  cfg = CP.safetyConfigs[-1]
  return int(cfg.safetyModel.raw), int(cfg.safetyParam)


def car_params(route: CarTestRoute, path: Path) -> structs.CarParams:
  cls = type("R", (TestCarModelBase,), {"test_route": route, "platform": route.car_model})
  car_fw, _, alpha_long = cls.get_testing_data_from_logreader(OpendbcLogReader(str(path), only_union_types=True, sort_by_time=True))
  return interfaces[cls.platform].get_params(cls.platform, cls.fingerprint, car_fw, alpha_long, False, docs=False)


def read_log(path: Path) -> list:
  # a log from the comma API is cached under a name cut from its signed url, so read it by content
  return sorted(LogReader.from_bytes(path.read_bytes()), key=lambda m: m.logMonoTime)


def load_log(s: Scenario) -> list:
  return read_log(get_cached_segment(s.route, s.segment))


def load_route(route: CarTestRoute, params: Callable[[CarTestRoute, Path], structs.CarParams] = car_params) -> tuple[int, structs.CarParams, list]:
  segments = (2, 1, 0) if route.segment is None else (route.segment,)
  errors = []
  for segment in segments:
    try:
      path = get_cached_segment(route.route, segment)
      msgs = read_log(path)
    except (OSError, AssertionError) as e:
      errors.append(f"segment {segment}: {e!r}")
      continue
    try:
      return segment, params(route, path), msgs
    except Exception as e:
      errors.append(f"segment {segment}: can't derive safety config: {e!r}")
  raise OSError("; ".join(errors))


def build_candidates(routes, load: Callable[[CarTestRoute], tuple[int, structs.CarParams, list]] = load_route) \
    -> tuple[dict[tuple[int, int], list[Scenario]], list[str]]:
  # every route's scenario per pair, in routes order, so a pair can fall back to its next route
  scenarios: dict[tuple[int, int], list[Scenario]] = {}
  seen_pairs: set[tuple[int, int]] = set()
  engaged_from_start: set[tuple[int, int]] = set()
  gaps = []
  for route in routes:
    who = f"{route.car_model} {route.route}"
    try:
      segment, CP, msgs = load(route)
    except Exception as e:
      gaps.append(f"{who}: can't load ({e})")
      continue

    mode_param = safety_pair(CP)
    span = can_span(msgs)
    if span < LENGTH:
      gaps.append(f"{who}: segment shorter than {LENGTH:g} s")
      continue
    seen_pairs.add(mode_param)

    samples = engaged_samples(msgs)
    if not samples:
      gaps.append(f"{who}: no panda state logged")
      continue
    window = pick_window(samples, span)
    if window is not None:
      scenarios.setdefault(mode_param, []).append(Scenario(route.route, segment, str(route.car_model), mode_param[0], mode_param[1], window, CP))
    elif samples[0][1]:
      engaged_from_start.add(mode_param)

  for mode, param in sorted(seen_pairs - scenarios.keys()):
    if (mode, param) in engaged_from_start:
      gaps.append(f"mode {mode} param {param}: no engage edge in segment")
    else:
      gaps.append(f"mode {mode} param {param}: no engaged stretch of {MIN_ENGAGED:g} s in any route")
  return scenarios, gaps


def build_scenarios(routes, load: Callable[[CarTestRoute], tuple[int, structs.CarParams, list]] = load_route) -> tuple[list[Scenario], list[str]]:
  candidates, gaps = build_candidates(routes, load)
  return [c[0] for c in candidates.values()], gaps


def opendbc_tree() -> str:
  return subprocess.check_output(["git", "rev-parse", "HEAD:opendbc_repo"], cwd=BASEDIR, text=True).strip()


def cached_scenarios(cache_dir: Path, routes=test_routes, build: Callable = build_scenarios, budget: dict[int, float] | None = None,
                     tree: Callable[[], str] = opendbc_tree) -> tuple[list[Scenario], list[str]]:
  # the list only changes with opendbc or the lane budget it was picked for
  lanes = "-".join(f"{rate:g}" for rate in (budget or {}).values())
  path = cache_dir / f"{tree()}-{lanes}.pkl"
  if path.exists():
    try:
      scenarios, gaps = pickle.loads(path.read_bytes())
      return scenarios, gaps
    except Exception:
      pass

  scenarios, gaps = build(routes)
  cache_dir.mkdir(parents=True, exist_ok=True)
  # write whole or not at all, runs share the cache dir
  fd, tmp = tempfile.mkstemp(dir=cache_dir)
  with os.fdopen(fd, "wb") as f:
    pickle.dump((scenarios, gaps), f)
  os.replace(tmp, path)
  return scenarios, gaps
