import unittest

from opendbc.car.structs import CarParams
from opendbc.safety.tests.libsafety import libsafety_py

from openpilot.cereal.messaging import new_message
from openpilot.selfdrive.test.up.oracle import check_commands, check_state, load_libsafety, run_safety

ALL_OUTPUT = int(CarParams.SafetyModel.allOutput)


def can_msg(which, t, frames):
  msg = new_message(which, len(frames))
  msg.logMonoTime = int(t * 1e9)
  for c, (addr, dat, src) in zip(getattr(msg, which), frames, strict=True):
    c.address, c.dat, c.src = addr, dat, src
  return msg


def tx_mode():
  # the first safety mode with TX messages, and those messages plus one it doesn't allow
  safety = load_libsafety()
  for mode in sorted(CarParams.SafetyModel.schema.enumerants.values()):
    if safety.set_safety_hooks(mode, 0) != 0:
      continue
    cfg = safety.safety_a.setup.current_safety_config
    if cfg.tx_msgs_len > 0:
      msgs = [(cfg.tx_msgs[i].addr, cfg.tx_msgs[i].bus, cfg.tx_msgs[i].len) for i in range(cfg.tx_msgs_len)]
      return mode, msgs + [(max(addr for addr, _, _ in msgs) + 1, 0, 8)]
  raise AssertionError("no safety mode with TX messages")


def command_recording(mode, commands):
  # sendcan plus the panda's echo of each, allowed or blocked as libsafety decides
  safety = load_libsafety()
  assert safety.set_safety_hooks(mode, 0) == 0
  msgs = [can_msg('can', 0., [])]
  for i, (addr, bus, length) in enumerate(commands):
    t = 0.01 + i * 0.02
    dat = bytes(length)
    msgs.append(can_msg('sendcan', t, [(addr, dat, bus)]))
    safety.set_timer(int((t + 0.01) * 1e6))
    allowed = safety.safety_tx_hook(libsafety_py.make_CANPacket(addr, bus, dat))
    msgs.append(can_msg('can', t + 0.01, [(addr, dat, bus + (128 if allowed else 192))]))
  return msgs


def flip(msg):
  c = msg.can[0]
  return can_msg('can', msg.logMonoTime / 1e9, [(c.address, c.dat, c.src + 64)])


def heartbeat_recording(panda_allowed_until, change=5.5, end=14., rx_invalid_from=None, alt_exp=0):
  msgs = [can_msg('can', 0., [])]
  for i in range(int(end * 10)):
    t = i / 10
    msg = new_message('selfdriveState')
    msg.selfdriveState.enabled = t < change
    msg.logMonoTime = int(t * 1e9)
    msgs.append(msg)
    if i % 5 == 2:
      ps = new_message('pandaStates', 1)
      ps.pandaStates[0].controlsAllowed = t < panda_allowed_until
      ps.pandaStates[0].safetyRxInvalid = int(rx_invalid_from is not None and t >= rx_invalid_from)
      ps.pandaStates[0].alternativeExperience = alt_exp
      ps.logMonoTime = int(t * 1e9) + 1
      msgs.append(ps)
  return msgs


class TestOracle(unittest.TestCase):
  @classmethod
  def setUpClass(cls):
    cls.mode, cls.commands = tx_mode()
    cls.recording = command_recording(cls.mode, cls.commands)

  def test_commands_match(self):
    self.assertEqual(check_commands(self.recording, self.mode, 0), [])

  def test_commands_flipped(self):
    echoes = [i for i, m in enumerate(self.recording) if m.which() == 'can' and len(m.can) and m.can[0].src < 192]
    i = echoes[0]
    msgs = self.recording[:i] + [flip(self.recording[i])] + self.recording[i + 1:]
    mismatches = check_commands(msgs, self.mode, 0)
    self.assertEqual(len(mismatches), 1)
    self.assertIn(f"TX 0x{self.recording[i].can[0].address:x} ", mismatches[0])

  def test_forwarded_echo_ignored(self):
    addr, bus, length = self.commands[0]
    msgs = self.recording + [can_msg('can', 0.9, [(addr, bytes(length), bus + 128)])]
    self.assertEqual(check_commands(msgs, self.mode, 0), [])

  def test_state_heartbeat_transition(self):
    # panda drops controls 2 ticks after the change, libsafety 3 ticks after
    self.assertEqual(check_state(heartbeat_recording(7.5), ALL_OUTPUT, 0), [])

  def test_state_stuck_engaged(self):
    self.assertGreater(len(check_state(heartbeat_recording(11.5), ALL_OUTPUT, 0)), 0)

  def test_rx_invalid_reported_once(self):
    mismatches = check_state(heartbeat_recording(7.5, rx_invalid_from=3.), ALL_OUTPUT, 0)
    self.assertEqual(len(mismatches), 1)
    self.assertIn("RX invalid", mismatches[0])

  def test_alternative_experience_applied(self):
    # libsafety has no getter, run_safety leaves its library loaded so the setup can be read back
    run_safety(heartbeat_recording(7.5, alt_exp=16), ALL_OUTPUT, 0)
    self.assertEqual(libsafety_py.libsafety.safety_a.setup.alternative_experience, 16)


if __name__ == "__main__":
  unittest.main()
