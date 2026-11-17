import unittest

from opendbc.car.tests.routes import routes
from opendbc.car.toyota.values import CAR as TOYOTA

from openpilot.cereal.messaging import new_message
from openpilot.selfdrive.test.up.oracle import load_libsafety
from openpilot.selfdrive.test.up.replay import DT, needed_messages, thin, validate, window_frames
from openpilot.selfdrive.test.up.scenario import Window, load_route

SECONDS = 20


def can_msg(t, frames):
  msg = new_message('can', len(frames))
  msg.logMonoTime = int(t * 1e9)
  for c, (addr, dat, src) in zip(msg.can, frames, strict=True):
    c.address, c.dat, c.src = addr, dat, src
  return msg


def synthetic(seconds=2):
  # bus 0 with 0x100 at 100 Hz and 0x200 at 50 Hz
  frames = []
  for i in range(int(seconds / DT)):
    frame = [(0x100, bytes(8), 0)]
    if i % 2 == 0:
      frame.append((0x200, bytes(8), 0))
    frames.append(frame)
  return frames


def positions(frames, addr):
  return [i for i, f in enumerate(frames) if any(a == addr for a, _, _ in f)]


def route_frames(car):
  route = next(r for r in routes if r.car_model == car)
  try:
    _, (mode, param), msgs = load_route(route)
  except OSError as e:
    raise unittest.SkipTest(f"{route.route} not cached: {e}") from e
  return str(car), mode, param, window_frames(msgs, Window(5., 5. + SECONDS))


class TestReplay(unittest.TestCase):
  def test_thin_keeps_constant_rate(self):
    out = thin(synthetic(), {0: 60.})
    for addr, gap in ((0x100, 3), (0x200, 6)):
      pos = positions(out, addr)
      self.assertGreater(len(pos), 2)
      self.assertEqual({b - a for a, b in zip(pos, pos[1:], strict=False)}, {gap})

  def test_thin_within_budget(self):
    frames = thin(synthetic(), {0: 60.})
    self.assertLessEqual(sum(len(f) for f in frames) / (len(frames) * DT), 60.)

  def test_thin_no_change_when_fits(self):
    frames = synthetic()
    self.assertEqual(thin(frames, {0: 1000.}), frames)

  def test_window_frames_drops_echoes(self):
    msgs = [can_msg(0., [(0x10, b'a', 0), (0x11, b'b', 128), (0x12, b'c', 192)]), can_msg(0.015, [(0x10, b'd', 1)])]
    frames = window_frames(msgs, Window(0., 0.02))
    self.assertEqual(frames, [[(0x10, b'a', 0)], [(0x10, b'd', 1)]])

  def test_needed_includes_safety_rx_checks(self):
    car, mode, param, frames = route_frames(TOYOTA.TOYOTA_COROLLA_TSS2)
    safety = load_libsafety()
    self.assertEqual(safety.set_safety_hooks(mode, param), 0)
    cfg = safety.safety_a.setup.current_safety_config
    checks = {(cfg.rx_checks[i].msg[0].bus, cfg.rx_checks[i].msg[0].addr) for i in range(cfg.rx_checks_len)}
    self.assertTrue(checks)
    self.assertLessEqual(checks, needed_messages(car, frames))

  def test_validate_flags_starved_message(self):
    car, mode, param, frames = route_frames(TOYOTA.TOYOTA_COROLLA_TSS2)
    problems = validate(car, mode, param, thin(frames, {0: 5.}))
    self.assertTrue(any("panda" in p or "can't parse" in p for p in problems), problems)

  def test_validate_clean(self):
    car, mode, param, frames = route_frames(TOYOTA.TOYOTA_COROLLA_TSS2)
    self.assertEqual(validate(car, mode, param, frames), [])
    needed = needed_messages(car, frames)
    frames = [[f for f in frame if (f[2], f[0]) in needed] for frame in frames]
    self.assertEqual(validate(car, mode, param, frames), [])


if __name__ == "__main__":
  unittest.main()
