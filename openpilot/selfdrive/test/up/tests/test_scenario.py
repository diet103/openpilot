import unittest

from opendbc.car.tests.routes import CarTestRoute

from openpilot.cereal.messaging import new_message
from openpilot.selfdrive.test.up.scenario import Window, build_scenarios, pick_window

ROUTE_A = CarTestRoute("aaaa|2024-01-01--00-00-00", "CAR_A")
ROUTE_B = CarTestRoute("bbbb|2024-01-01--00-00-00", "CAR_B")


def samples(*stretches, end=70):
  return [(float(t), any(a <= t < b for a, b in stretches)) for t in range(end)]


def car_params(mode=1, param=2):
  msg = new_message('carParams')
  cfg = msg.carParams.init('safetyConfigs', 1)
  cfg[0].safetyModel = mode
  cfg[0].safetyParam = param
  msg.logMonoTime = 0
  return msg


def panda_states(allowed_by_second):
  msgs = []
  for i, allowed in enumerate(allowed_by_second):
    msg = new_message('pandaStates', 1)
    msg.pandaStates[0].controlsAllowed = allowed
    msg.logMonoTime = int(i * 1e9)
    msgs.append(msg)
  return msgs


def fake_load(by_route):
  def load(route):
    item = by_route[route.route]
    if isinstance(item, Exception):
      raise item
    return 2, item
  return load


class TestScenario(unittest.TestCase):
  def test_window_longest_stretch(self):
    self.assertEqual(pick_window(samples((2, 9), (20, 60))), Window(15., 45.))

  def test_window_clamped_at_start(self):
    self.assertEqual(pick_window(samples((0, 40))), Window(0., 30.))

  def test_window_too_short(self):
    self.assertIsNone(pick_window(samples((3, 12))))

  def test_window_never_engaged(self):
    self.assertIsNone(pick_window(samples()))

  def test_one_scenario_per_pair(self):
    engaged = [car_params()] + panda_states([False] * 10 + [True] * 30)
    scenarios, gaps = build_scenarios([ROUTE_A, ROUTE_B], fake_load({ROUTE_A.route: engaged, ROUTE_B.route: engaged}))
    self.assertEqual(len(scenarios), 1)
    self.assertEqual(scenarios[0].route, ROUTE_A.route)
    self.assertEqual((scenarios[0].mode, scenarios[0].param), (1, 2))
    self.assertEqual(scenarios[0].window, Window(5., 35.))
    self.assertEqual(gaps, [])

  def test_gap_when_no_window(self):
    msgs = [car_params(3, 4)] + panda_states([False] * 20)
    scenarios, gaps = build_scenarios([ROUTE_A], fake_load({ROUTE_A.route: msgs}))
    self.assertEqual(scenarios, [])
    self.assertEqual(len(gaps), 1)
    self.assertIn("mode 3 param 4", gaps[0])
    self.assertIn("no engaged stretch", gaps[0])

  def test_gap_when_no_panda_state(self):
    scenarios, gaps = build_scenarios([ROUTE_A], fake_load({ROUTE_A.route: [car_params()]}))
    self.assertEqual(scenarios, [])
    self.assertIn("no panda state logged", gaps[0])
    self.assertIn("CAR_A", gaps[0])

  def test_gap_when_load_fails(self):
    scenarios, gaps = build_scenarios([ROUTE_A], fake_load({ROUTE_A.route: OSError("nope")}))
    self.assertEqual(scenarios, [])
    self.assertIn("can't load", gaps[0])


if __name__ == "__main__":
  unittest.main()
