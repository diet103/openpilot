import subprocess
import time
import unittest
from pathlib import Path

from opendbc.car.structs import CarParams

from openpilot.cereal.messaging import new_message
from openpilot.selfdrive.test.up.drive import Drive, RigProblem, onroad_missing, onroad_timeout, split_preroll
from openpilot.selfdrive.test.up.replay import DT
from openpilot.selfdrive.test.up.scenario import Window, pick_window


def alive(pid: int) -> bool:
  # a killed process is gone or a zombie until init reaps it
  stat = Path(f"/proc/{pid}/stat")
  try:
    return stat.read_text().rsplit(")", 1)[1].split()[0] != "Z"
  except FileNotFoundError:
    return False


class TestDrive(unittest.TestCase):
  def test_split_preroll(self):
    window = Window(10., 13.)
    frames = [[(0x100, bytes([i % 256]), 0)] for i in range(round((window.end - window.start) / DT))]
    # the last not allowed sample 1.5 s into the window
    preroll, full = split_preroll(frames, window, 11.5)
    self.assertEqual(preroll, frames[:150])
    self.assertEqual(full, frames)

  def test_preroll_ends_before_edge(self):
    # 10 Hz samples, not allowed at 9.9 s, allowed from 10.0 s
    window, _, preroll_end = pick_window([(i / 10, i >= 100) for i in range(400)], 40.)
    frames = [[(0x100, bytes([i % 256]), 0)] for i in range(round((window.end - window.start) / DT))]
    preroll, _ = split_preroll(frames, window, preroll_end)
    # only frames before 9.9 s, the window starts at 5 s
    self.assertEqual(len(preroll), 490)
    self.assertEqual(preroll, frames[:490])

  def test_onroad_timeout(self):
    died = onroad_timeout(-11, False, {1}, ["not started"])
    self.assertNotIsInstance(died, RigProblem)
    self.assertEqual(str(died), "manager died: rc -11")
    self.assertEqual(str(onroad_timeout(None, False, {1}, ["no panda"])), "rig problem: panda not answering")
    self.assertEqual(str(onroad_timeout(None, True, {1, 2}, ["carState not canValid"])), "rig problem: no CAN on bus [1, 2]")
    never = onroad_timeout(None, True, set(), ["not started"])
    self.assertIsInstance(never, RigProblem)
    self.assertEqual(str(never), "rig problem: not onroad after 120 s: not started")

  def test_stop_kills_orphans(self):
    # a manager that died and left a child ignoring SIGINT behind
    drive = Drive(None)
    manager = subprocess.Popen(["sh", "-c", "(trap '' INT; sleep 100) & echo $!; exit 3"], stdout=subprocess.PIPE, text=True, start_new_session=True)
    orphan = int(manager.stdout.readline())
    manager.stdout.close()
    manager.wait()
    drive._manager = manager
    with self.assertRaisesRegex(RuntimeError, "manager died: rc 3"):
      drive._stop_manager()
    end = time.monotonic() + 5
    while alive(orphan) and time.monotonic() < end:
      time.sleep(0.01)
    self.assertFalse(alive(orphan))

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
