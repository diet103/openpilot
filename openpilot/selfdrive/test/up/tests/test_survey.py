import argparse
import unittest
from functools import partial
from unittest import mock

from opendbc.car.car_helpers import interfaces
from opendbc.car.toyota.values import CAR as TOYOTA

from openpilot.cereal.messaging import new_message
from openpilot.selfdrive.test.up.replay import BUDGET, Replay
from openpilot.selfdrive.test.up.scenario import Scenario, Window, safety_pair
from openpilot.selfdrive.test.up.survey import pair_report, parse_budget, run_scenarios

PLATFORM = str(TOYOTA.TOYOTA_COROLLA_TSS2)


def can_start(t):
  msg = new_message('can', 0)
  msg.logMonoTime = int(t * 1e9)
  return msg.as_reader()


def scenario(name, CP):
  return Scenario(f"{name}|2024-01-01--00-00-00", 0, PLATFORM, *safety_pair(CP), Window(0., 30.), CP)


class TestSurvey(unittest.TestCase):
  def test_parse_budget(self):
    self.assertEqual(parse_budget("230,130,60"), {0: 230., 1: 130., 2: 60.})
    for bad in ("230,130", "230,130,60,10", "230,x,60", "", "230,0,60"):
      with self.assertRaises(argparse.ArgumentTypeError, msg=bad):
        parse_budget(bad)

  def test_scenario_error_is_a_gap(self):
    good = interfaces[PLATFORM].get_non_essential_params(PLATFORM)
    bad = good.copy()
    bad.safetyConfigs[-1].safetyModel = 0xFFFF
    a, b = scenario("a", bad), scenario("b", good)
    msgs = [can_start(i * 0.01) for i in range(3100)]
    report = partial(pair_report, load=lambda s: msgs)
    lines, gaps, _ = run_scenarios({(0, 0): [a], (1, 0): [b]}, BUDGET, report=report)
    self.assertEqual(len(gaps), 1, gaps)
    self.assertTrue(gaps[0].startswith(f"{PLATFORM} {a.route}: ValueError("), gaps)
    self.assertEqual(len(lines), 1)
    self.assertIn(b.route, lines[0])

  def test_lane_budget_gap(self):
    CP = interfaces[PLATFORM].get_non_essential_params(PLATFORM)
    a, b = scenario("a", CP), scenario("b", CP)
    with mock.patch("openpilot.selfdrive.test.up.replay.build_replay", return_value=Replay([], [], ["bus 0 300 frames/s > 230"])):
      report = pair_report([a, b], BUDGET, load=lambda s: [])
    mode, param = safety_pair(CP)
    self.assertEqual((report.line, report.gap), (None, f"mode {mode} param {param}: lane budget: bus 0 300 frames/s > 230"))


if __name__ == "__main__":
  unittest.main()
