import tempfile
import unittest
from pathlib import Path
from unittest import mock

import zstandard as zstd

from opendbc.car import structs
from opendbc.car.tests.routes import CarTestRoute, routes
from opendbc.car.tests.test_models import get_cached_segment
from opendbc.car.toyota.values import CAR as TOYOTA

from openpilot.cereal.messaging import new_message
from openpilot.selfdrive.test.up.scenario import (Scenario, Window, build_candidates, build_scenarios, cached_scenarios, can_span, car_params,
                                                  engaged_samples, load_log, load_route, pick_window, safety_pair)

ROUTE_A = CarTestRoute("aaaa|2024-01-01--00-00-00", "CAR_A")  # ty: ignore[invalid-argument-type]
ROUTE_B = CarTestRoute("bbbb|2024-01-01--00-00-00", "CAR_B")  # ty: ignore[invalid-argument-type]


def samples(*stretches, end=70):
  return [(float(t), any(a <= t < b for a, b in stretches)) for t in range(end)]


def can_start(t=0.):
  msg = new_message('can', 0)
  msg.logMonoTime = int(t * 1e9)
  return msg


def panda_states(allowed_by_second, start=0., can_at=0.):
  msgs = [can_start(can_at), can_start(can_at + len(allowed_by_second))]
  for i, allowed in enumerate(allowed_by_second):
    msg = new_message('pandaStates', 1)
    msg.pandaStates[0].controlsAllowed = allowed
    msg.logMonoTime = int((start + i) * 1e9)
    msgs.append(msg)
  return msgs


def fake_car_params(mode, param):
  CP = structs.CarParams()
  cfg = structs.CarParams.SafetyConfig()
  cfg.safetyModel, cfg.safetyParam = mode, param
  CP.safetyConfigs = [cfg]
  return CP


def fake_load(by_route, pairs=None):
  def load(route):
    item = by_route[route.route]
    if isinstance(item, Exception):
      raise item
    return 2, fake_car_params(*(pairs or {}).get(route.route, (1, 2))), item
  return load


class TestScenario(unittest.TestCase):
  def test_window_longest_stretch(self):
    self.assertEqual(pick_window(samples((2, 9), (20, 60)), 70.), (Window(15., 45.), 20.))

  def test_window_exact_lead(self):
    self.assertEqual(pick_window(samples((20, 60)), 70.), (Window(15., 45.), 20.))

  def test_lead_in_too_short(self):
    self.assertIsNone(pick_window(samples((3, 40)), 70.))
    self.assertIsNone(pick_window(samples((0, 40)), 70.))
    scenarios, gaps = build_scenarios([ROUTE_A], fake_load({ROUTE_A.route: panda_states([False] * 3 + [True] * 37)}))
    self.assertEqual(scenarios, [])
    self.assertEqual(gaps, ["mode 1 param 2: no 5 s disengaged lead-in before the engage edge"])

  def test_lead_in_engaged(self):
    # allowed at 17 s, inside the window that would start at 15 s
    self.assertIsNone(pick_window(samples((2, 18), (20, 60)), 70.))
    scenarios, gaps = build_scenarios([ROUTE_A], fake_load({ROUTE_A.route: panda_states([2 <= i < 18 or i >= 20 for i in range(70)])}))
    self.assertEqual(scenarios, [])
    self.assertEqual(gaps, ["mode 1 param 2: no 5 s disengaged lead-in before the engage edge"])

  def test_window_needs_engage_edge(self):
    self.assertIsNone(pick_window(samples((0, 50)), 70.))
    self.assertEqual(pick_window(samples((0, 20), (30, 50)), 70.), (Window(25., 55.), 30.))
    scenarios, gaps = build_scenarios([ROUTE_A], fake_load({ROUTE_A.route: panda_states([True] * 40)}))
    self.assertEqual(scenarios, [])
    self.assertEqual(gaps, ["mode 1 param 2: no engage edge in segment"])

  def test_window_too_short(self):
    self.assertIsNone(pick_window(samples((8, 17)), 70.))

  def test_window_never_engaged(self):
    self.assertIsNone(pick_window(samples(), 70.))

  def test_window_fits_segment(self):
    self.assertEqual(pick_window(samples((30, 60), end=60), 60.), (Window(25., 55.), 30.))
    self.assertIsNone(pick_window(samples((50, 60), end=60), 60.))

  def test_one_scenario_per_pair(self):
    engaged = panda_states([False] * 10 + [True] * 30)
    scenarios, gaps = build_scenarios([ROUTE_A, ROUTE_B], fake_load({ROUTE_A.route: engaged, ROUTE_B.route: engaged}))
    self.assertEqual(len(scenarios), 1)
    self.assertEqual(scenarios[0].route, ROUTE_A.route)
    self.assertEqual(scenarios[0].car_model, "CAR_A")
    self.assertEqual((scenarios[0].mode, scenarios[0].param), (1, 2))
    self.assertEqual(safety_pair(scenarios[0].CP), (1, 2))
    self.assertEqual(scenarios[0].window, Window(5., 35.))
    self.assertEqual(gaps, [])

  def test_gap_when_no_window(self):
    scenarios, gaps = build_scenarios([ROUTE_A], fake_load({ROUTE_A.route: panda_states([False] * 40)}, {ROUTE_A.route: (3, 4)}))
    self.assertEqual(scenarios, [])
    self.assertEqual(len(gaps), 1, gaps)
    self.assertIn("mode 3 param 4", gaps[0])
    self.assertIn("no engaged stretch", gaps[0])

  def test_gap_when_no_panda_state(self):
    scenarios, gaps = build_scenarios([ROUTE_A], fake_load({ROUTE_A.route: [can_start(0.), can_start(40.)]}))
    self.assertEqual(scenarios, [])
    self.assertIn("no panda state logged", gaps[0])
    self.assertIn("CAR_A", gaps[0])

  def test_short_segment_gap(self):
    scenarios, gaps = build_scenarios([ROUTE_A], fake_load({ROUTE_A.route: panda_states([True] * 20)}))
    self.assertEqual(scenarios, [])
    self.assertIn("segment shorter than 30 s", gaps[0])

  def test_times_from_first_can(self):
    early = panda_states([False], start=100.)[2:]
    engaged = panda_states([20 <= i < 60 for i in range(70)], start=200., can_at=200.)
    self.assertEqual(pick_window(engaged_samples(early + engaged), can_span(engaged)), (Window(15., 45.), 20.))

  def test_gap_when_load_fails(self):
    scenarios, gaps = build_scenarios([ROUTE_A], fake_load({ROUTE_A.route: OSError("nope")}))
    self.assertEqual(scenarios, [])
    self.assertIn("can't load", gaps[0])

  def test_load_signed_url_name(self):
    msgs = [can_start(2.), can_start(1.)]
    with tempfile.TemporaryDirectory() as tmp:
      # the cache names a comma API log after the last part of its signed url
      path = Path(tmp) / "0123.zst&sig=abc%3D"
      path.write_bytes(zstd.compress(b"".join(m.to_bytes() for m in msgs)))
      with mock.patch("openpilot.selfdrive.test.up.scenario.get_cached_segment", return_value=path):
        segment, CP, loaded = load_route(CarTestRoute("aaaa|2024-01-01--00-00-00", "CAR_A", 0), params=lambda r, p: fake_car_params(1, 2))  # ty: ignore[invalid-argument-type]
        logged = load_log(Scenario("aaaa|2024-01-01--00-00-00", 0, "CAR_A", 1, 2, Window(0., 30.), 5., CP))
    self.assertEqual((segment, safety_pair(CP)), (0, (1, 2)))
    self.assertEqual([m.logMonoTime for m in loaded], [int(1e9), int(2e9)])
    self.assertEqual([m.logMonoTime for m in logged], [int(1e9), int(2e9)])

  def test_car_params_signed_url_name(self):
    route = next(r for r in routes if r.car_model == TOYOTA.TOYOTA_COROLLA_TSS2)
    try:
      path = get_cached_segment(route.route, 2)
    except OSError as e:
      raise unittest.SkipTest(f"{route.route} not cached: {e}") from e
    with tempfile.TemporaryDirectory() as tmp:
      mangled = Path(tmp) / "0123.zst&sig=abc%3D"
      mangled.write_bytes(path.read_bytes())
      CP = car_params(route, mangled)
    self.assertEqual(CP.to_bytes(), car_params(route, path).to_bytes())

  def test_scenario_has_rise(self):
    engaged = panda_states([False] * 10 + [True] * 30)
    scenarios, _ = build_scenarios([ROUTE_A], fake_load({ROUTE_A.route: engaged}))
    self.assertEqual(scenarios[0].window, Window(5., 35.))
    self.assertEqual(scenarios[0].rise, 10.)

  def test_candidates_in_routes_order(self):
    engaged = panda_states([False] * 10 + [True] * 30)
    candidates, gaps = build_candidates([ROUTE_A, ROUTE_B], fake_load({ROUTE_A.route: engaged, ROUTE_B.route: engaged}))
    self.assertEqual([s.route for s in candidates[(1, 2)]], [ROUTE_A.route, ROUTE_B.route])
    self.assertEqual(gaps, [])


BUDGET = {0: 230., 1: 130., 2: 60.}


class TestCachedScenarios(unittest.TestCase):
  def setUp(self):
    self.calls = 0
    self.tmp = tempfile.TemporaryDirectory()
    self.addCleanup(self.tmp.cleanup)
    self.dir = Path(self.tmp.name) / "up_scenarios"

  def build(self, routes, budget):
    self.calls += 1
    return [Scenario("a|2024-01-01--00-00-00", 0, "CAR", 1, 2, Window(0., 30.), 5., structs.CarParams())], ["gap"]

  def test_cache_hit(self):
    first = cached_scenarios(self.dir, BUDGET, self.build, [], tree=lambda: "t1")
    second = cached_scenarios(self.dir, BUDGET, self.build, [], tree=lambda: "t1")
    self.assertEqual(self.calls, 1)
    self.assertEqual(second[1], ["gap"])
    # the CP comes back as a reader, compare by dict
    self.assertEqual([(s.route, s.window, s.CP.to_dict()) for s in first[0]], [(s.route, s.window, s.CP.to_dict()) for s in second[0]])

  def test_cache_key_changes(self):
    cached_scenarios(self.dir, BUDGET, self.build, [], tree=lambda: "t1")
    cached_scenarios(self.dir, BUDGET, self.build, [], tree=lambda: "t2")
    cached_scenarios(self.dir, {0: 1., 1: 2.}, self.build, [], tree=lambda: "t1")
    self.assertEqual(self.calls, 3)

  def test_cache_key_bus_ids(self):
    cached_scenarios(self.dir, {0: 230., 1: 130.}, self.build, [], tree=lambda: "t1")
    cached_scenarios(self.dir, {1: 230., 2: 130.}, self.build, [], tree=lambda: "t1")
    self.assertEqual(self.calls, 2)

  def test_cache_key_harness_change(self):
    cached_scenarios(self.dir, BUDGET, self.build, [], tree=lambda: "opendbc-harness1")
    cached_scenarios(self.dir, BUDGET, self.build, [], tree=lambda: "opendbc-harness2")
    self.assertEqual(self.calls, 2)

  def test_build_gets_budget(self):
    seen = []
    cached_scenarios(self.dir, BUDGET, lambda routes, budget: seen.append(budget) or ([], []), [], tree=lambda: "t1")
    self.assertEqual(seen, [BUDGET])

  def test_cache_corrupt(self):
    cached_scenarios(self.dir, BUDGET, self.build, [], tree=lambda: "t1")
    (path,) = self.dir.glob("*.pkl")
    path.write_bytes(b"garbage")
    scenarios, gaps = cached_scenarios(self.dir, BUDGET, self.build, [], tree=lambda: "t1")
    self.assertEqual(self.calls, 2)
    self.assertEqual(len(scenarios), 1)
    cached_scenarios(self.dir, BUDGET, self.build, [], tree=lambda: "t1")
    self.assertEqual(self.calls, 2)


if __name__ == "__main__":
  unittest.main()
