import argparse
import multiprocessing
import os
import time
from collections import Counter, deque
from collections.abc import Callable
from concurrent.futures import ProcessPoolExecutor
from dataclasses import dataclass

from opendbc.car import structs
from opendbc.car.tests.routes import CarTestRoute, routes as test_routes

from openpilot.selfdrive.test.up.oracle import command_mismatches, run_safety, state_mismatches
from openpilot.selfdrive.test.up.replay import BUDGET, DT, Frame, pick_replay
from openpilot.selfdrive.test.up.scenario import Scenario, build_candidates, load_log, load_route

WORKERS = os.cpu_count() or 1


@dataclass
class Report:
  # a scenario line, or the coverage gap it became
  line: str | None
  gap: str | None
  times: Counter[str]


def pair_report(candidates: list[Scenario], budget: dict[int, float], load: Callable[[Scenario], list] = load_log) -> Report:
  times: Counter[str] = Counter()
  tried: list[Scenario] = []
  try:
    return _pair_report(candidates, budget, load, times, tried)
  except Exception as e:
    return Report(None, f"{tried[-1].car_model} {tried[-1].route}: {e!r}", times)


def _rates(frames: list[Frame], budget: dict[int, float]) -> str:
  seconds = len(frames) * DT
  load = Counter(bus for f in frames for _, _, bus in f)
  return " ".join(f"{bus}:{load[bus] / seconds:.0f}" for bus in sorted(budget))


def _pair_report(candidates: list[Scenario], budget: dict[int, float], load: Callable[[Scenario], list],
                 times: Counter[str], tried: list[Scenario]) -> Report:
  def timed_load(s: Scenario) -> list:
    tried.append(s)
    t = time.monotonic()
    msgs = load(s)
    times["load"] += time.monotonic() - t
    return msgs

  t = time.monotonic()
  s, msgs, replay = pick_replay(candidates, budget, timed_load)
  times["replay"] += time.monotonic() - t - times["load"]
  if replay.lane_problems:
    return Report(None, f"mode {s.mode} param {s.param}: lane budget: {', '.join(replay.lane_problems)}", times)

  t = time.monotonic()
  # the oracle sees the same window of the recording, in the same time base
  first = min(m.logMonoTime for m in msgs if m.which() == 'can')
  logged = [m for m in msgs if s.window.start <= (m.logMonoTime - first) / 1e9 < s.window.end]
  run = run_safety(logged, s.mode, s.param)
  commands = command_mismatches(run)
  state = state_mismatches(run, logged)
  times["oracle"] += time.monotonic() - t

  where = f"{s.mode} {s.param} {s.car_model} {s.route} {s.window.start:.0f}-{s.window.end:.0f}s"
  checks = f"validate {', '.join(replay.problems) or 'ok'} commands {len(commands)} state {len(state)}"
  return Report(f"{where} frames/s {_rates(replay.frames, budget)} {checks}", None, times)


def run_scenarios(candidates: dict[tuple[int, int], list[Scenario]], budget: dict[int, float],
                  map_fn: Callable = map, report: Callable[..., Report] = pair_report) -> tuple[list[str], list[str], Counter[str]]:
  pairs = list(candidates.values())
  lines, gaps = [], []
  times: Counter[str] = Counter()
  for r in map_fn(report, pairs, [budget] * len(pairs)):
    times.update(r.times)
    if r.line is not None:
      lines.append(r.line)
    else:
      gaps.append(r.gap)
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
    lines, report_gaps, report_times = run_scenarios(candidates, budget, p.map)
  times.update(report_times)
  gaps += report_gaps
  for line in lines:
    print(line)

  pair_gaps = [g for g in gaps if g.startswith("mode ")]
  print(f"{len(lines)} scenarios, {len(pair_gaps)} pairs in coverage gaps")
  for gap in gaps:
    print(gap)
  # build_scenarios (build_candidates) is wall time in this process and includes waiting on loads, the other stages are summed over workers
  stages = ", ".join(f"{name} {times[name]:.1f} s" for name in ("load", "build_scenarios", "replay", "oracle"))
  print(f"timing: {WORKERS} workers, run {time.monotonic() - start:.1f} s, {stages}")


if __name__ == "__main__":
  main()
