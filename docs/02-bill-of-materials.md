# Bill of materials

## Have

| Item | Qty | Use |
|---|---|---|
| Hoverboard hub motors (hall sensors) | 6 | Drive |
| BLDC hall controllers (bought separately) | 6 | One per motor, model to be confirmed |
| HOTRC DS-600 + F-06A receiver | 1 | First tests |
| ESP32-WROOM-32 DevKit 30-pin, USB-C, CP2102 (3-pack) + expansion boards | 3 | Chassis, custom remote, spare |
| ESP32-S3-DevKitC-1 + expansion board | 1 | Fallback / experiments |
| Raspberry Pi Zero W, Orange Pi Zero | 1 each | Later: companion computer (camera, dashboard) |
| Arduino | 1 | Spare / experiments |

## Needed for the first test

| Item | Qty | Note |
|---|---|---|
| 5 V buck converter, ≥60 V input (e.g. XL7015) | 1 | Powers ESP32 + receiver |
| 2N7000 N-MOSFET | 2 | DIR switches |
| Resistor 1 kΩ | 2 | Speed lines |
| Resistor 100 Ω | 2 | MOSFET gates |
| Resistor 100 kΩ | 2 | Gate pull-downs |
| Fuse 40–60 A + holder | 1 | Battery + line |
| Main switch / emergency stop | 1 | Battery + line |
| 10 kΩ potentiometer | 1 | Bench test |
| Small perfboard, wire, terminals | – | Interface board |
| Multimeter | 1 | Every step |

## Later (custom remote)

| Item | Qty | Note |
|---|---|---|
| Analog joysticks (industrial style or thumb) | 2 | Drive + auxiliary |
| Momentary buttons, toggle switches, key switch | – | ARM, modes, horn, lights |
| Mushroom e-stop button | 1 | Remote kill |
| Li-ion cell + charger board | 1 | Remote power |
| Small OLED display | 1 | Battery, link status |
