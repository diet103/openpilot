from opendbc.car.structs import CarParams

# seconds after the panda's engage edge by which openpilot must be enabled
ENGAGE_LIMIT = 2.
# the panda marks what it sent with src + 128 and what it blocked with src + 192
BLOCKED = 192
BUSES = 3
NO_CAR_MODES = (
  int(CarParams.SafetyModel.noOutput),
  int(CarParams.SafetyModel.silent),
  int(CarParams.SafetyModel.elm327),
)


def rule_failures(msgs: list, mode: int, param: int) -> list[str]:
  msgs = sorted(msgs, key=lambda m: m.logMonoTime)
  if len(msgs) == 0:
    return []
  start = msgs[0].logMonoTime

  failures = []
  enabled = False
  enabled_since = None
  prev_allowed = None
  edges = []
  enable_times = []
  commands_while_engaged = 0
  engaged_at = None
  drop_reported = False
  car_mode_seen = False
  reported_pair = None
  reported_faults = set()

  for msg in msgs:
    t = (msg.logMonoTime - start) / 1e9
    which = msg.which()

    if which == 'selfdriveState':
      now_enabled = msg.selfdriveState.enabled
      if now_enabled and not enabled:
        enabled_since = t
        enable_times.append(t)
        if engaged_at is None:
          engaged_at = t
      if not now_enabled:
        enabled_since = None
        drop_reported = False
      enabled = now_enabled

    elif which == 'sendcan':
      if enabled and len(msg.sendcan) > 0:
        commands_while_engaged += 1

    elif which == 'can':
      for frame in msg.can:
        if frame.src - BLOCKED in range(BUSES) and enabled:
          failures.append((t, f"t={t:.2f} blocked while engaged, addr {frame.address:#x} bus {frame.src - BLOCKED}"))

    elif which == 'pandaStates' and len(msg.pandaStates) > 0:
      state = msg.pandaStates[0]
      allowed = state.controlsAllowed
      if allowed and prev_allowed is False:
        edges.append((t, enabled))
      prev_allowed = allowed

      if enabled and not allowed and not drop_reported and t - enabled_since > ENGAGE_LIMIT:
        drop_reported = True
        failures.append((t, f"t={t:.2f} controls_allowed false while engaged"))

      logged_mode = state.safetyModel.raw
      if logged_mode not in NO_CAR_MODES:
        car_mode_seen = True
      if car_mode_seen:
        pair = (logged_mode, state.safetyParam)
        if pair != (mode, param) and pair != reported_pair:
          reported_pair = pair
          failures.append((t, f"t={t:.2f} safety pair {pair} != {(mode, param)}"))

      for fault in state.faults:
        name = str(fault)
        if name not in reported_faults:
          reported_faults.add(name)
          failures.append((t, f"t={t:.2f} new panda fault {name}"))

  for edge, was_enabled in edges:
    engaged = was_enabled or any(edge <= t_enable <= edge + ENGAGE_LIMIT for t_enable in enable_times)
    if not engaged:
      failures.append((edge, f"t={edge:.2f} not engaged, openpilot not enabled within {ENGAGE_LIMIT:.0f}s of controls_allowed"))

  if engaged_at is not None and commands_while_engaged == 0:
    failures.append((engaged_at, f"t={engaged_at:.2f} no commands while engaged"))

  return [line for _, line in sorted(failures, key=lambda f: f[0])]

