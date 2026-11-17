import unittest

from opendbc.can.packer import CANPacker
from opendbc.car.structs import CarParams
from opendbc.safety.tests.libsafety import libsafety_py

from openpilot.cereal.messaging import new_message
from openpilot.selfdrive.test.up.oracle import check_commands, check_state, load_libsafety, run_safety

ALL_OUTPUT = int(CarParams.SafetyModel.allOutput)
TOYOTA = int(CarParams.SafetyModel.toyota)
TOYOTA_EPS_SCALE = 73
TOYOTA_PACKER = CANPacker("toyota_nodsu_pt_generated")


def pcm_cruise(active):
  return TOYOTA_PACKER.make_can_msg("PCM_CRUISE", 0, {"CRUISE_ACTIVE": active, "GAS_RELEASED": 1})


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
    safety.set_timer(int(t * 1e6))
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
    # an address libsafety blocks, so it would show as a mismatch if it counted as a command
    addr, bus, length = self.commands[-1]
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

  def test_controls_allowed_sampling(self):
    # no heartbeat, so libsafety drops controls at its third tick, the panda's sample just after still shows them
    msgs = [can_msg('can', 0., [])]
    for t, allowed in ((2.5, True), (3.2, True), (3.7, False), (4.2, False)):
      ps = new_message('pandaStates', 1)
      ps.pandaStates[0].controlsAllowed = allowed
      ps.logMonoTime = int(t * 1e9)
      msgs.append(ps)
    self.assertEqual(check_state(msgs, ALL_OUTPUT, 0), [])

  def test_rx_invalid_boundary_skew(self):
    # libsafety rejects an RX frame at 0.6 s, the panda's sample at 0.5 s already counted it
    safety = load_libsafety()
    assert safety.set_safety_hooks(self.mode, 0) == 0
    rx = safety.safety_a.setup.current_safety_config.rx_checks[0].msg[0]
    msgs = [can_msg('can', 0., []), can_msg('can', 0.6, [(rx.addr, bytes(rx.len), rx.bus)])]
    for t, rx_invalid in ((0.25, 0), (0.5, 1), (0.75, 1), (0.9, 1)):
      ps = new_message('pandaStates', 1)
      ps.pandaStates[0].safetyRxInvalid = rx_invalid
      ps.logMonoTime = int(t * 1e9)
      msgs.append(ps)
    self.assertEqual(len(run_safety(msgs, self.mode, 0).rx_rejected), 1)
    self.assertEqual(check_state(msgs, self.mode, 0), [])

  def test_heartbeat_check_before_tick(self):
    # cruise allows controls without an engaged heartbeat, the heartbeat check counts that before
    # the tick drops controls for the RX checks that never got a message
    msgs = [can_msg('can', 0., [pcm_cruise(1)]), can_msg('can', 1., [])]
    run = run_safety(msgs, TOYOTA, TOYOTA_EPS_SCALE)
    self.assertEqual(run.controls_allowed, [(0., False), (0., True), (1., False)])
    self.assertEqual(libsafety_py.libsafety.safety_a.state.heartbeat_engaged_mismatches, 1)

  def test_command_judged_at_sendcan(self):
    # cruise drops between the command and its echo, the panda judged the command before that
    _, dat, _ = TOYOTA_PACKER.make_can_msg("STEERING_LKA", 0, {"STEER_TORQUE_CMD": 10, "STEER_REQUEST": 1})
    msgs = [
      can_msg('can', 0., [pcm_cruise(1)]),
      can_msg('sendcan', 0.01, [(0x2e4, dat, 0)]),
      can_msg('can', 0.015, [pcm_cruise(0)]),
      can_msg('can', 0.02, [(0x2e4, dat, 128)]),
    ]
    run = run_safety(msgs, TOYOTA, TOYOTA_EPS_SCALE)
    self.assertEqual(run.controls_allowed[-1], (0.015, False))
    self.assertEqual(run.commands, [(0.01, 0x2e4, 0, True, True)])


if __name__ == "__main__":
  unittest.main()
