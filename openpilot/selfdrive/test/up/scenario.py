from collections.abc import Callable, Iterable
from dataclasses import dataclass

from opendbc.car.tests.routes import CarTestRoute
from opendbc.car.tests.test_models import get_cached_segment

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


def engaged_samples(msgs: Iterable) -> list[tuple[float, bool]]:
  samples = []
  first = None
  for m in msgs:
    if first is None:
      first = m.logMonoTime
    which = m.which()
    if which == 'pandaStates' and len(m.pandaStates):
      allowed = m.pandaStates[0].controlsAllowed
    elif which == 'pandaStateDEPRECATED':
      allowed = m.pandaStateDEPRECATED.controlsAllowed
    else:
      continue
    samples.append(((m.logMonoTime - first) / 1e9, bool(allowed)))
  return samples


def pick_window(samples: list[tuple[float, bool]]) -> Window | None:
  best = None
  best_len = 0.
  rise = None
  # a stretch ends at the first sample that is not engaged, or at the last sample
  for i, (t, allowed) in enumerate(samples):
    if allowed and rise is None:
      rise = t
    last = i == len(samples) - 1
    if rise is not None and (not allowed or last):
      length = t - rise
      if length >= MIN_ENGAGED and length > best_len:
        best, best_len = rise, length
      rise = None
  if best is None:
    return None
  start = max(0., best - LEAD)
  return Window(start, start + LENGTH)


def safety_pair(msgs: Iterable) -> tuple[int, int] | None:
  pair = None
  for m in msgs:
    if m.which() == 'carParams' and len(m.carParams.safetyConfigs):
      cfg = m.carParams.safetyConfigs[-1]
      pair = (int(cfg.safetyModel.raw), int(cfg.safetyParam))
  return pair


def load_route(route: CarTestRoute) -> tuple[int, list]:
  segments = (2, 1, 0) if route.segment is None else (route.segment,)
  for segment in segments:
    try:
      path = get_cached_segment(route.route, segment)
      return segment, list(LogReader(str(path), sort_by_time=True))
    except (OSError, AssertionError):
      pass
  raise OSError(f"segments {segments} not found")


def build_scenarios(routes, load: Callable[[CarTestRoute], tuple[int, list]] = load_route) -> tuple[list[Scenario], list[str]]:
  scenarios: dict[tuple[int, int], Scenario] = {}
  seen_pairs: set[tuple[int, int]] = set()
  gaps = []
  for route in routes:
    who = f"{route.car_model} {route.route}"
    try:
      segment, msgs = load(route)
    except Exception as e:
      gaps.append(f"{who}: can't load ({e})")
      continue

    pair = safety_pair(msgs)
    if pair is None:
      gaps.append(f"{who}: no carParams safety config logged")
      continue
    seen_pairs.add(pair)
    if pair in scenarios:
      continue

    samples = engaged_samples(msgs)
    if not samples:
      gaps.append(f"{who}: no panda state logged")
      continue
    window = pick_window(samples)
    if window is not None:
      scenarios[pair] = Scenario(route.route, segment, route.car_model, pair[0], pair[1], window)

  for mode, param in sorted(seen_pairs - scenarios.keys()):
    gaps.append(f"mode {mode} param {param}: no engaged stretch of {MIN_ENGAGED:g} s in any route")
  return list(scenarios.values()), gaps
