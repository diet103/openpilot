import argparse
import multiprocessing
import os
import time
from collections import Counter, deque
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
WORKERS = os.cpu_count() or 1


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


@dataclass
class Report:
  # a scenario line, or the coverage gap it became
  line: str | None
  gap: str | None
  times: Counter[str]
  # the pair's next route may still fit the lanes
  retry: bool = False


def scenario_report(scenario: Scenario, route: CarTestRoute, budget: dict[int, float],
                    load: Callable[[CarTestRoute], tuple[int, structs.CarParams, list]] = load_route) -> Report:
  times: Counter[str] = Counter()
  try:
    return _scenario_report(scenario, route, budget, load, times)
  except Exception as e:
    return Report(None, f"{scenario.car_model} {scenario.route}: {e!r}", times)


def _scenario_report(s: Scenario, route: CarTestRoute, budget: dict[int, float], load: Callable, times: Counter[str]) -> Report:
  # replay and oracle import this module
  from openpilot.selfdrive.test.up.oracle import command_mismatches, run_safety, state_mismatches
  from openpilot.selfdrive.test.up.replay import DT, needed_messages, over_budget, thin, validate, window_frames

  t = time.monotonic()
  _, _, msgs = load(route._replace(segment=s.segment))
  times["load"] += time.monotonic() - t

  t = time.monotonic()
  frames = window_frames(msgs, s.window)
  needed = needed_messages(s.CP, frames)
  needed_frames = [[f for f in frame if (f[2], f[0]) in needed] for frame in frames]
  frames = thin(s.CP, needed_frames, budget)
  times["frames"] += time.monotonic() - t
  seconds = len(frames) * DT
  rates = " ".join(f"{bus}:{sum(b == bus for f in frames for _, _, b in f) / seconds:.0f}" for bus in sorted(budget))

  t = time.monotonic()
  problems = validate(s.CP, frames)
  # a replay over the lane's rate or one only thinning breaks can't run on the station
  lane_problems = over_budget(frames, budget) or (problems if problems and not validate(s.CP, needed_frames) else [])
  times["validate"] += time.monotonic() - t
  if lane_problems:
    return Report(None, f"mode {s.mode} param {s.param}: lane budget: {', '.join(lane_problems)}", times, retry=True)

  t = time.monotonic()
  # the oracle sees the same window of the recording, in the same time base
  first = min(m.logMonoTime for m in msgs if m.which() == 'can')
  logged = [m for m in msgs if s.window.start <= (m.logMonoTime - first) / 1e9 < s.window.end]
  run = run_safety(logged, s.mode, s.param)
  commands, state = command_mismatches(run), state_mismatches(run, logged)
  times["oracle"] += time.monotonic() - t

  where = f"{s.mode} {s.param} {s.car_model} {s.route} {s.window.start:.0f}-{s.window.end:.0f}s"
  checks = f"validate {', '.join(problems) or 'ok'} commands {len(commands)} state {len(state)}"
  return Report(f"{where} frames/s {rates} {checks}", None, times)


def run_scenarios(candidates: dict[tuple[int, int], list[Scenario]], routes: list[CarTestRoute], budget: dict[int, float],
                  map_fn: Callable = map, report: Callable[..., Report] = scenario_report) -> tuple[list[str], list[str], Counter[str]]:
  by_name = {(r.route, str(r.car_model)): r for r in routes}
  lines, gaps = [], []
  times: Counter[str] = Counter()
  # each round tries every open pair's next route, a pair gets the gap of its last route only when none fits
  tries = dict.fromkeys(candidates, 0)
  while tries:
    todo = [(pair, candidates[pair][i]) for pair, i in tries.items()]
    results = map_fn(report, [s for _, s in todo], [by_name[(s.route, s.car_model)] for _, s in todo], [budget] * len(todo))
    next_tries = {}
    for (pair, _), r in zip(todo, results, strict=True):
      times.update(r.times)
      if r.line is not None:
        lines.append(r.line)
      elif r.retry and tries[pair] + 1 < len(candidates[pair]):
        next_tries[pair] = tries[pair] + 1
      else:
        gaps.append(r.gap)
    tries = next_tries
  return lines, gaps, times


def timed_load(route: CarTestRoute) -> tuple[float, tuple[int, structs.CarParams, list] | Exception]:
  t = time.monotonic()
  try:
    loaded: tuple[int, structs.CarParams, list] | Exception = load_route(route)
  except Exception as e:
    loaded = e
  return time.monotonic() - t, loaded


def parse_budget(text: str) -> dict[int, float]:
  # frames/s of the station's lanes, one number each for buses 0, 1 and 2
  try:
    rates = [float(r) for r in text.split(",")]
  except ValueError as e:
    raise argparse.ArgumentTypeError(f"{text!r}: expected three numbers for buses 0,1,2") from e
  if len(rates) != 3 or not all(r > 0 for r in rates):
    raise argparse.ArgumentTypeError(f"{text!r}: expected three positive numbers for buses 0,1,2")
  return dict(enumerate(rates))


def pool() -> ProcessPoolExecutor:
  return ProcessPoolExecutor(WORKERS, mp_context=multiprocessing.get_context("spawn"))


def main() -> None:
  from openpilot.selfdrive.test.up.replay import BUDGET

  parser = argparse.ArgumentParser(description="list every test route's scenario and coverage gaps")
  parser.add_argument("--budget", type=parse_budget, default=BUDGET,
                      help="frames/s of the station's lanes for buses 0,1,2, default " + ",".join(f"{r:g}" for r in BUDGET.values()))
  budget = parser.parse_args().budget

  start = time.monotonic()
  times: Counter[str] = Counter()

  def load(_: CarTestRoute) -> tuple[int, structs.CarParams, list]:
    seconds, loaded = loads.popleft().result()
    times["load"] += seconds
    if isinstance(loaded, Exception):
      raise loaded
    return loaded

  with pool() as p:
    loads = deque(p.submit(timed_load, r) for r in test_routes)
    t = time.monotonic()
    candidates, gaps = build_candidates(test_routes, load=load)
    times["build_scenarios"] += time.monotonic() - t
    lines, report_gaps, report_times = run_scenarios(candidates, test_routes, budget, p.map)
  times.update(report_times)
  gaps += report_gaps
  for line in lines:
    print(line)

  pair_gaps = [g for g in gaps if g.startswith("mode ")]
  print(f"{len(lines)} scenarios, {len(pair_gaps)} pairs in coverage gaps")
  for gap in gaps:
    print(gap)
  # build_scenarios (build_candidates) is wall time in this process and includes waiting on loads, the other stages are summed over workers
  stages = ", ".join(f"{name} {times[name]:.1f} s" for name in ("load", "build_scenarios", "frames", "validate", "oracle"))
  print(f"timing: {WORKERS} workers, run {time.monotonic() - start:.1f} s, {stages}")


if __name__ == "__main__":
  main()
