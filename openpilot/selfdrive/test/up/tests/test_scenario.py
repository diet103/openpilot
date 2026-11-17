import tempfile
import unittest
from pathlib import Path
from unittest import mock

import zstandard as zstd

from opendbc.car import structs
from opendbc.car.tests.routes import CarTestRoute

from openpilot.cereal.messaging import new_message
from openpilot.selfdrive.test.up.scenario import Window, build_scenarios, can_span, engaged_samples, load_route, pick_window, safety_pair

ROUTE_A = CarTestRoute("aaaa|2024-01-01--00-00-00", "CAR_A")
ROUTE_B = CarTestRoute("bbbb|2024-01-01--00-00-00", "CAR_B")


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


def car_params(mode, param):
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
    return 2, car_params(*(pairs or {}).get(route.route, (1, 2))), item
  return load


class TestScenario(unittest.TestCase):
  def test_window_longest_stretch(self):
    self.assertEqual(pick_window(samples((2, 9), (20, 60)), 70.), Window(15., 45.))

  def test_window_clamped_at_start(self):
    self.assertEqual(pick_window(samples((2, 40)), 70.), Window(0., 30.))
    self.assertIsNone(pick_window(samples((0, 40)), 70.))

  def test_window_needs_engage_edge(self):
    self.assertIsNone(pick_window(samples((0, 50)), 70.))
    self.assertEqual(pick_window(samples((0, 50), (55, 70)), 70.), Window(40., 70.))
    scenarios, gaps = build_scenarios([ROUTE_A], fake_load({ROUTE_A.route: panda_states([True] * 40)}))
    self.assertEqual(scenarios, [])
    self.assertEqual(gaps, ["mode 1 param 2: no engage edge in segment"])

  def test_window_too_short(self):
    self.assertIsNone(pick_window(samples((3, 12)), 70.))

  def test_window_never_engaged(self):
    self.assertIsNone(pick_window(samples(), 70.))

  def test_window_fits_segment(self):
    self.assertEqual(pick_window(samples((50, 60), end=60), 60.), Window(30., 60.))

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
    self.assertEqual(len(gaps), 1)
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
    self.assertEqual(pick_window(engaged_samples(early + engaged), can_span(engaged)), Window(15., 45.))

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
        segment, CP, loaded = load_route(CarTestRoute("aaaa|2024-01-01--00-00-00", "CAR_A", 0), params=lambda r, p: car_params(1, 2))
    self.assertEqual((segment, safety_pair(CP)), (0, (1, 2)))
    self.assertEqual([m.logMonoTime for m in loaded], [int(1e9), int(2e9)])


if __name__ == "__main__":
  unittest.main()
