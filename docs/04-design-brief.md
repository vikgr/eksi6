# Design brief — eksi6 six-wheeled cargo platform

## Requirements

| Item | Decision |
|---|---|
| Purpose | Cargo platform: carry all kinds of goods |
| Top | **Completely flat deck**, nothing sticking out above it |
| Wheels | 6× hoverboard hub-motor wheels, 3 per side, skid steer |
| Colours | **Grey** body, **orange** accents |
| Look | Industrial, like professional material-handling equipment |
| Speed | **Max 5 km/h.** Two modes: turtle ~2.5 km/h, rabbit ~5 km/h (see [05-speed-and-torque.md](05-speed-and-torque.md)) |
| Priority | **Maximum torque at low speed** |
| Concept image | First impression created in ChatGPT ("eksi6 Six-Wheeled Cargo Platform"), to be added to `docs/images/` |

## Side view (concept)

```
            FLAT CARGO DECK (grey, anti-slip)
   ┌───────────────────────────────────────────────┐  ◄ deck = highest point
   │▓▓▓▓▓▓▓▓▓▓▓▓▓▓ orange edge stripe ▓▓▓▓▓▓▓▓▓▓▓▓▓│
   │   battery · ESP32 · 6 controllers (inside)    │
   └──╮   ╭─────────╮   ╭─────────╮   ╭────────╮ ╭─┘
      (O)          (O)            (O)          ◄ hoverboard wheels, 3 per side
```

## Top view (concept)

```
   front
   ┌───────────────────────────────────┐
 (O)  orange corner      orange corner (O)
   │                                   │
 (O)        flat grey deck            (O)
   │      (tie-down points at edges)   │
 (O)  orange corner      orange corner (O)
   └───────────────────────────────────┘
   rear
```

## Design rules that follow from the flat top

- Everything (battery, ESP32, controllers, buck converter, wiring) lives **under** the deck.
- E-stop, main switch, charge port and status LED go on the **front or side panels**, never on top.
- The receiver antenna and, later, the ESP-NOW antenna also go on a side panel.
- Wheels stay inside the deck footprint or flush with it, so goods can overhang without touching the tyres.
- The deck needs tie-down points or rails along the edges.

## Open points

- Deck size (length × width) and target payload in kg
- Deck material (aluminium sheet, plywood with an anti-slip coating, or a 3D-printed/composite top)
- Ground clearance
