# eksi6 — to-do

## 📋 Vik to provide

| # | What | Why it matters | Status |
|---|---|---|---|
| 1 | **Wheel diameter**: measure the tyre outside diameter (probably 6.5″ ≈ 165 mm) | Sets the rpm for 2.5 and 5 km/h | ⏳ open |
| 2 | **Controller terminal photo**: labels such as VR/SV, Z/F, GND, EL, BK, P/SC | Confirms the SPEED, DIR and GND pins before power-up | ⏳ open |
| 3 | **Controller model and current limit** (amps, from the label or listing) | The current limit sets the maximum torque | ⏳ open |
| 4 | **Speed-pulse output on the controller?** (P / SC / SPEED OUT) | Needed to hold the speed under heavy load (closed loop) | ⏳ open |
| 5 | **Battery**: voltage (36 V / 10S?), capacity in Ah, number of packs | Fuse size, buck converter, run time | ⏳ open |
| 6 | **Deck size** (length × width), **max payload** in kg, **ground clearance** | Frame design, torque check | ⏳ open |
| 7 | **Concept image** from ChatGPT, uploaded directly | Reference design in `docs/images/` | ⏳ open |
| 8 | **First-test parts on hand?** Buck ≥60 V, 2× 2N7000, resistors, fuse, e-stop, 10 kΩ pot | Needed for the first power-up | ⏳ open |
| 9 | **Which DS-600 switches are CH3 and CH4** (read in the Serial Monitor) | ARM and turtle/rabbit mapping | ⏳ at first test |

## 🔧 Build / test steps

- [ ] Bench test one controller with a potentiometer
- [ ] Receiver + ESP32 on USB, check the channels
- [ ] Left side on stands → fix directions → right side
- [ ] Calibrate `OUT_AT_RABBIT` so full trigger in rabbit = 5 km/h (≈160 rpm on 6.5″)
- [ ] Check the smooth start/stop feel, tune `ACCEL_KMH_S` / `DECEL_KMH_S` / `JERK_KMH_S2`
- [ ] Failsafe test (transmitter off)
- [ ] First drive on the ground, empty, then with load

## 💻 Claude to do (after inputs above)

- [ ] Confirm the wiring against the controller labels, then update the docs, PDF and whiteboard
- [ ] Closed-loop speed control (hall / speed-pulse feedback): holds 2.5 / 5 km/h under load
- [ ] Frame and deck layout drawing (after deck size and payload)
- [ ] Custom remote: ESP-NOW protocol, joystick/button layout, housing concept
- [ ] Use the brake pin for holding on slopes (after checking what the controller supports)

## ✅ Done

- [x] Wiring plan, diagrams, whiteboard, tablet PDF
- [x] Firmware: skid steer, ARM, failsafe, turtle/rabbit (max 5 km/h)
- [x] Smooth S-curve start/stop in both modes
- [x] GitHub repo set up
