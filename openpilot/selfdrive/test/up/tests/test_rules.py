import unittest

from opendbc.car.structs import CarParams

from openpilot.cereal.messaging import new_message
from openpilot.selfdrive.test.up.rules import ENGAGE_LIMIT, rule_failures

TOYOTA = int(CarParams.SafetyModel.toyota)


def panda_state(t, controls_allowed=True, mode=TOYOTA, param=0, faults=()):
  msg = new_message('pandaStates', 1)
  msg.logMonoTime = int(t * 1e9)
  state = msg.pandaStates[0]
  state.controlsAllowed = controls_allowed
  state.safetyModel = mode
  state.safetyParam = param
  fault_list = state.init('faults', len(faults))
  for i, fault in enumerate(faults):
    fault_list[i] = fault
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
    msgs.append(panda_state(t + 0.5, True))
  return msgs


class TestRules(unittest.TestCase):
  def test_clean_drive(self):
    self.assertEqual(rule_failures(clean_drive(), TOYOTA, 0), [])

  def test_not_engaged(self):
    msgs = [panda_state(0., False), selfdrive_state(0., False), panda_state(1., True)]
    msgs.append(selfdrive_state(1. + ENGAGE_LIMIT + 1., False))
    failures = rule_failures(msgs, TOYOTA, 0)
    self.assertEqual(len(failures), 1)
    self.assertIn("not engaged", failures[0])

  def test_blocked_while_engaged(self):
    msgs = clean_drive()
    msgs.append(can_msg('can', 3.05, [(0x100, 192)]))
    failures = rule_failures(msgs, TOYOTA, 0)
    self.assertEqual(len(failures), 1)
    self.assertIn("blocked while engaged", failures[0])

  def test_no_commands_while_engaged(self):
    msgs = [m for m in clean_drive() if m.which() != 'sendcan']
    failures = rule_failures(msgs, TOYOTA, 0)
    self.assertEqual(len(failures), 1)
    self.assertIn("no commands while engaged", failures[0])

  def test_controls_allowed_dropped(self):
    msgs = clean_drive()
    msgs.append(panda_state(1.5 + 3., False))
    failures = rule_failures(msgs, TOYOTA, 0)
    self.assertEqual(len(failures), 1)
    self.assertIn("controls_allowed false while engaged", failures[0])

  def test_safety_pair_mismatch(self):
    msgs = clean_drive()
    msgs[2] = panda_state(1., True, param=1)
    failures = rule_failures(msgs, TOYOTA, 0)
    self.assertEqual(len(failures), 1)
    self.assertIn(f"safety pair ({TOYOTA}, 1) != ({TOYOTA}, 0)", failures[0])

  def test_new_fault(self):
    msgs = clean_drive()
    msgs.append(panda_state(4., True, faults=['interruptRateCan1']))
    failures = rule_failures(msgs, TOYOTA, 0)
    self.assertEqual(len(failures), 1)
    self.assertIn("new panda fault interruptRateCan1", failures[0])


if __name__ == "__main__":
  unittest.main()
