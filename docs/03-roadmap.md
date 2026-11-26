# Roadmap

```
 Phase 1            Phase 2               Phase 3                  Phase 4
 First test   ──►   Custom remote   ──►   Industrial housing  ──►  Companion computer
 HOTRC DS-600       ESP32 + ESP-NOW       3D-printed remote        Pi Zero: camera,
 → ESP32            joysticks, buttons    + chassis covers         dashboard, logging
```

## Phase 1 — First test (now)
- Wire per [01-first-test-wiring.md](01-first-test-wiring.md).
- Confirm controller terminal labels and logic levels.
- Bench test → one side → both sides → on the ground.

## Phase 2 — Custom remote
- Second ESP32 in the remote reads joysticks and buttons.
- ESP-NOW link to the chassis ESP32 (low latency, a few hundred metres).
- Heartbeat every 20 ms; the chassis stops when the heartbeat is missing for 100 ms.
- Telemetry back to the remote: battery voltage, link quality.

## Phase 3 — Industrial-style housing
- 3D-printed remote shell (crane / industrial pendant style).
- Mushroom e-stop, key switch, guarded toggles.
- Chassis electronics box with cable glands.

## Phase 4 — Companion computer (optional)
- Raspberry Pi Zero W over UART to the ESP32.
- Camera / FPV, web dashboard, data logging.
- The ESP32 keeps the final say over the motors.
