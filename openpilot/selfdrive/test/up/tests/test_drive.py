import unittest

from opendbc.car.structs import CarParams

from openpilot.cereal.messaging import new_message
from openpilot.selfdrive.test.up.drive import onroad_missing, split_preroll
from openpilot.selfdrive.test.up.replay import DT
from openpilot.selfdrive.test.up.scenario import Window


class TestDrive(unittest.TestCase):
  def test_split_preroll(self):
    window = Window(10., 13.)
    frames = [[(0x100, bytes([i % 256]), 0)] for i in range(round((window.end - window.start) / DT))]
    # the engage edge 1.5 s into the window
    preroll, full = split_preroll(frames, window, 11.5)
    self.assertEqual(preroll, frames[:150])
    self.assertEqual(full, frames)

  def test_split_preroll_no_frames_before_edge(self):
    frames = [[(0x100, bytes(1), 0)] for _ in range(300)]
    with self.assertRaises(ValueError):
      split_preroll(frames, Window(10., 13.), 10.)

  def test_onroad_missing(self):
    sm = {
      'deviceState': new_message('deviceState').deviceState,
      'carState': new_message('carState').carState,
      'managerState': new_message('managerState').managerState,
      'pandaStates': new_message('pandaStates', 1).pandaStates,
    }
    sm['deviceState'].started = True
    sm['carState'].canValid = True
    procs = sm['managerState'].init('processes', 2)
    procs[0].name, procs[0].shouldBeRunning, procs[0].running = "selfdrived", True, True
    procs[1].name, procs[1].shouldBeRunning, procs[1].running = "soundd", False, False
    sm['pandaStates'][0].safetyModel = CarParams.SafetyModel.toyota
    self.assertEqual(onroad_missing(sm), [])

    procs[0].running = False
    sm['pandaStates'][0].safetyModel = CarParams.SafetyModel.elm327
    sm['carState'].canValid = False
    self.assertEqual(onroad_missing(sm), ["carState not canValid", "not running: selfdrived", "panda in elm327"])
    sm['deviceState'].started = False
    sm['pandaStates'] = new_message('pandaStates', 0).pandaStates
    self.assertEqual(onroad_missing(sm)[0], "not started")
    self.assertEqual(onroad_missing(sm)[-1], "no panda")


if __name__ == "__main__":
  unittest.main()
