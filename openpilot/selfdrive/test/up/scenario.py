import argparse
import multiprocessing
import os
from collections import deque
from collections.abc import Callable, Iterable
from concurrent.futures import ProcessPoolExecutor
from dataclasses import dataclass
from pathlib import Path

from opendbc.car import structs
from opendbc.car.car_helpers import interfaces
from opendbc.car.logreader import LogReader as OpendbcLogReader
from opendbc.car.tests.routes import CarTestRoute, routes as test_routes
from opendbc.car.tests.test_models import TestCarModelBase, get_cached_segment

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
  rise = None
  # a stretch ends at the first sample that is not engaged, or at the last sample
  for i, (t, allowed) in enumerate(samples):
    if allowed and rise is None:
      rise = t
    last = i == len(samples) - 1
    if rise is not None and (not allowed or last):
      length = (can_end if allowed else t) - rise
      if length >= MIN_ENGAGED and length > best_len:
        best, best_len = rise, length
      rise = None
  if best is None:
    return None
  start = max(0., min(best - LEAD, can_end - LENGTH))
  return Window(start, start + LENGTH)


def safety_pair(CP: structs.CarParams) -> tuple[int, int]:
  cfg = CP.safetyConfigs[-1]
  return int(cfg.safetyModel.raw), int(cfg.safetyParam)


def car_params(route: CarTestRoute, path: Path) -> structs.CarParams:
  cls = type("R", (TestCarModelBase,), {"test_route": route, "platform": route.car_model})
  car_fw, _, alpha_long = cls.get_testing_data_from_logreader(OpendbcLogReader(str(path), only_union_types=True, sort_by_time=True))
  return interfaces[cls.platform].get_params(cls.platform, cls.fingerprint, car_fw, alpha_long, False, docs=False)


def load_route(route: CarTestRoute, params: Callable[[CarTestRoute, Path], structs.CarParams] = car_params) -> tuple[int, structs.CarParams, list]:
  segments = (2, 1, 0) if route.segment is None else (route.segment,)
  errors = []
  for segment in segments:
    try:
      path = get_cached_segment(route.route, segment)
      # a log from the comma API is cached under a name cut from its signed url, so read it by content
      msgs = sorted(LogReader.from_bytes(path.read_bytes()), key=lambda m: m.logMonoTime)
    except (OSError, AssertionError) as e:
      errors.append(f"segment {segment}: {e!r}")
      continue
    try:
      return segment, params(route, path), msgs
    except Exception as e:
      errors.append(f"segment {segment}: can't derive safety config: {e!r}")
  raise OSError("; ".join(errors))


def build_scenarios(routes, load: Callable[[CarTestRoute], tuple[int, structs.CarParams, list]] = load_route) -> tuple[list[Scenario], list[str]]:
  scenarios: dict[tuple[int, int], Scenario] = {}
  seen_pairs: set[tuple[int, int]] = set()
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
    if mode_param in scenarios:
      continue

    samples = engaged_samples(msgs)
    if not samples:
      gaps.append(f"{who}: no panda state logged")
      continue
    window = pick_window(samples, span)
    if window is not None:
      scenarios[mode_param] = Scenario(route.route, segment, str(route.car_model), mode_param[0], mode_param[1], window, CP)

  for mode, param in sorted(seen_pairs - scenarios.keys()):
    gaps.append(f"mode {mode} param {param}: no engaged stretch of {MIN_ENGAGED:g} s in any route")
  return list(scenarios.values()), gaps


def scenario_report(scenario: Scenario, route: CarTestRoute, budget: dict[int, float]) -> str:
  # replay and oracle import this module
  from openpilot.selfdrive.test.up.oracle import command_mismatches, run_safety, state_mismatches
  from openpilot.selfdrive.test.up.replay import DT, needed_messages, thin, validate, window_frames

  s = scenario
  _, _, msgs = load_route(route._replace(segment=s.segment))
  frames = window_frames(msgs, s.window)
  needed = needed_messages(s.CP, frames)
  frames = thin(s.CP, [[f for f in frame if (f[2], f[0]) in needed] for frame in frames], budget)
  seconds = len(frames) * DT
  rates = " ".join(f"{bus}:{sum(b == bus for f in frames for _, _, b in f) / seconds:.0f}" for bus in sorted(budget))
  problems = validate(s.CP, frames)

  # the oracle sees the same window of the recording, in the same time base
  first = min(m.logMonoTime for m in msgs if m.which() == 'can')
  logged = [m for m in msgs if s.window.start <= (m.logMonoTime - first) / 1e9 < s.window.end]
  run = run_safety(logged, s.mode, s.param)
  where = f"{s.mode} {s.param} {s.car_model} {s.route} {s.window.start:.0f}-{s.window.end:.0f}s"
  checks = f"validate {', '.join(problems) or 'ok'} commands {len(command_mismatches(run))} state {len(state_mismatches(run, logged))}"
  return f"{where} frames/s {rates} {checks}"


def pool() -> ProcessPoolExecutor:
  return ProcessPoolExecutor(os.cpu_count(), mp_context=multiprocessing.get_context("spawn"))


def main() -> None:
  parser = argparse.ArgumentParser(description="list every test route's scenario and coverage gaps")
  parser.add_argument("--budget", default="230,130,60", help="frames/s of the station's lanes for buses 0,1,2")
  args = parser.parse_args()
  budget = dict(enumerate(float(r) for r in args.budget.split(",")))

  with pool() as p:
    loads = deque(p.submit(load_route, r) for r in test_routes)
    scenarios, gaps = build_scenarios(test_routes, load=lambda _: loads.popleft().result())
    by_name = {(r.route, str(r.car_model)): r for r in test_routes}
    for line in p.map(scenario_report, scenarios, [by_name[(s.route, s.car_model)] for s in scenarios], [budget] * len(scenarios)):
      print(line, flush=True)

  pair_gaps = [g for g in gaps if g.startswith("mode ")]
  print(f"{len(scenarios)} scenarios, {len(pair_gaps)} pairs in coverage gaps")
  for gap in gaps:
    print(gap)


if __name__ == "__main__":
  main()
