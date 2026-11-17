import unittest

from opendbc.car.structs import CarParams

from openpilot.cereal.messaging import new_message
from openpilot.selfdrive.test.cars.rules import rule_failures

TOYOTA = int(CarParams.SafetyModel.toyota)


def panda_state(t, controls_allowed):
  msg = new_message('pandaStates', 1)
  msg.logMonoTime = int(t * 1e9)
  msg.pandaStates[0].controlsAllowed = controls_allowed
  msg.pandaStates[0].safetyModel = TOYOTA
  return msg


def selfdrive_state(t, enabled):
  msg = new_message('selfdriveState')
  msg.logMonoTime = int(t * 1e9)
  msg.selfdriveState.enabled = enabled
  return msg


def can_msg(which, t, frames):
  msg = new_message(which, len(frames))
  msg.logMonoTime = int(t * 1e9)
  for c, (addr, src) in zip(getattr(msg, which), frames, strict=True):
    c.address = addr
    c.dat = bytes(8)
    c.src = src
  return msg


def clean_drive(duration=5.):
  # panda allows at 1 s, openpilot enables 0.5 s later, one command a second after that
  msgs = [panda_state(0., False), selfdrive_state(0., False)]
  msgs += [panda_state(1., True), selfdrive_state(1.5, True)]
  for i in range(int(duration)):
    t = 2. + i
    msgs.append(can_msg('sendcan', t, [(0x100, 0)]))
    msgs.append(can_msg('can', t + 0.01, [(0x100, 128)]))
    msgs.append(selfdrive_state(t + 0.02, True))
    msgs.append(panda_state(t + 0.5, True))
  return msgs


def failures(msgs):
  return rule_failures(msgs, TOYOTA, 0)


class TestRules(unittest.TestCase):
  def test_clean_drive(self):
    self.assertEqual(failures(clean_drive()), [])

  def test_brake_disengage(self):
    # the panda drops controls and blocks commands, openpilot disengages right after
    msgs = clean_drive(3.)
    msgs.append(panda_state(5., False))
    msgs.append(can_msg('can', 5., [(0x100, 192)]))
    msgs.append(selfdrive_state(5.02, False))
    self.assertEqual(failures(msgs), [])

  def test_stuck_engaged_after_drop(self):
    msgs = clean_drive(3.)
    msgs.append(panda_state(5., False))
    msgs.append(can_msg('can', 5., [(0x100, 192)]))
    for i in range(3):
      msgs.append(can_msg('sendcan', 5.01 + i, [(0x100, 0)]))
      msgs.append(selfdrive_state(5.02 + i, True))
    self.assertTrue(any("controls_allowed false while engaged" in f for f in failures(msgs)))

  def test_blocked_while_engaged(self):
    msgs = clean_drive()
    msgs.append(can_msg('can', 3.05, [(0x100, 192)]))
    result = failures(msgs)
    self.assertEqual(len(result), 1)
    self.assertIn("blocked while engaged", result[0])

  def test_commands_stop(self):
    msgs = [m for m in clean_drive(8.) if not (m.which() == 'sendcan' and m.logMonoTime >= 5e9)]
    result = failures(msgs)
    self.assertEqual(len(result), 1)
    self.assertIn("commands stopped", result[0])


if __name__ == "__main__":
  unittest.main()
