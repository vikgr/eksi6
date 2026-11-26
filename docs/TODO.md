# eksi6 — to-do

## 📋 Vik to provide

| # | What | Why it matters | Status |
|---|---|---|---|
| 1 | **Wheel diameter**: measure the tyre outside diameter (probably 6.5″ ≈ 165 mm) | Sets the rpm for 2.5 and 5 km/h | ⏳ open |
| 2 | **Controller terminal photo**: labels such as VR/SV, Z/F, GND, EL, BK, P/SC | Confirms the SPEED, DIR and GND pins before power-up | ⏳ open |
| 3 | **Controller model and current limit** (amps, from the label or listing) | The current limit sets the maximum torque | ⏳ open |
| 4 | **Speed-pulse output on the controller?** (P / SC / SPEED OUT) | Needed to hold the speed under heavy load (closed loop) | ⏳ open |
| 5 | ~~Battery~~ → 2× Makita 18 V 5 Ah in series (later 4, Milwaukee on a new model); has adapters for both | Power system designed ([06](06-power-system.md)) | ✅ done |
| 6 | ~~Deck size~~ 400 × 800 mm ✅ · still open: **max payload** in kg, **ground clearance** | Frame design, torque check | ⏳ partly |
| 7 | ~~Concept image~~ | Added as `docs/images/eksi6-concept.png` | ✅ done |
| 8 | **First-test parts on hand?** Buck ≥60 V, 2× 2N7000, resistors (incl. 2× 100 kΩ + 2× 6.8 kΩ battery dividers), 100 nF caps, 40 A fuse, e-stop, 10 kΩ pot | Needed for the first power-up | ⏳ open |
| 10 | **Controller undervoltage cutoff** (if on the label) | Should be below 30 V so the firmware stops first | ⏳ open |
| 11 | **Wheel spacing decision**: close together (190 mm pitch, turns easily) or spread like the concept? | Skid-steer turning ([04](04-design-brief.md)) | ⏳ open |
| 9 | **Which DS-600 switches are CH3 and CH4** (read in the Serial Monitor) | ARM and turtle/rabbit mapping | ⏳ at first test |

## 🔧 Build / test steps

- [ ] Bench test one controller with a potentiometer
- [ ] Receiver + ESP32 on USB, check the channels
- [ ] Left side on stands → fix directions → right side
- [ ] Calibrate `OUT_AT_RABBIT` so full trigger in rabbit = 5 km/h (≈160 rpm on 6.5″)
- [ ] Check the smooth start/stop feel, tune `ACCEL_KMH_S` / `DECEL_KMH_S` / `JERK_KMH_S2`
- [ ] Check the battery voltage reading against a multimeter (`DIV_RATIO`)
- [ ] Failsafe test (transmitter off)
- [ ] First drive on the ground, empty, then with load

## 💻 Claude to do (after inputs above)

- [ ] Confirm the wiring against the controller labels, then update the docs, PDF and whiteboard
- [ ] Closed-loop speed control (hall / speed-pulse feedback): holds 2.5 / 5 km/h under load
- [ ] Frame and deck layout drawing (after deck size and payload)
- [ ] Custom remote: ESP-NOW protocol, joystick/button layout, housing concept
- [ ] Use the brake pin for holding on slopes (after checking what the controller supports)
- [ ] 3D-printed battery bay with swappable Makita / Milwaukee adapter plates
- [ ] Stage 2 batteries: ideal-diode modules for 2S2P
- [ ] Compile check of the firmware (arduino-cli in CI / GitHub Actions)

## ✅ Done

- [x] Wiring plan, diagrams, whiteboard, tablet PDF
- [x] Firmware: skid steer, ARM, failsafe, turtle/rabbit (max 5 km/h)
- [x] Smooth S-curve start/stop in both modes
- [x] GitHub repo set up
- [x] Concept image, deck 400 × 800 mm, top layout and skid-steer geometry
- [x] Power system: Makita 2S / 2S2P, per-pack low-voltage protection in firmware
