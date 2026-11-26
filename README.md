# ESP32-Based Smart Home Automation

Wi-Fi and manual appliance control using ESP32 DevKit V1, a four-channel relay module, four physical switches, four demonstration bulbs, and a 16x2 I2C LCD.

## Architecture

Physical switches connect to ESP32 GPIO inputs. A Wi-Fi dashboard sends commands to the ESP32. The ESP32 is the single source of truth for relay states and drives the same four relay outputs for both methods. State changes will be synchronized to connected dashboard clients.

## Proposed pin map

| Function | GPIO |
|---|---:|
| Relay IN1 / IN2 / IN3 / IN4 | 23 / 19 / 18 / 27 |
| Switch S1 / S2 / S3 / S4 | 13 / 14 / 32 / 33 |
| LCD SDA / SCL | 21 / 22 |

Switches use active-low `INPUT_PULLUP` wiring to GND. Verify relay input polarity on the physical module before use. Pin assignments remain provisional until tested.

## Planned features

- Four independent relays and physical switch inputs
- Wi-Fi Station mode and Access Point mode
- Responsive dashboard with editable appliance name, type, icon and color
- WebSocket state synchronization
- LCD Wi-Fi status and IP address
- LittleFS dashboard and configuration storage

## Repository layout

```text
.
├── README.md
├── firmware/
│   ├── SmartHome_Automation/
│   │   └── SmartHome_Automation.ino
│   └── component_tests/
│       ├── relay_test/relay_test.ino
│       ├── switch_test/switch_test.ino
│       ├── lcd_test/lcd_test.ino
│       ├── wifi_test/wifi_test.ino
│       └── littlefs_test/littlefs_test.ino
└── dashboard/
    ├── index.html
    ├── css/style.css
    ├── js/app.js
    └── README.md
```

## Development stages

- [ ] Stage 1: Relay and switch validation
- [ ] Stage 2: LCD and Wi-Fi STA/AP validation
- [ ] Stage 3: LittleFS and configuration persistence
- [ ] Stage 4: Dashboard and appliance editor
- [ ] Stage 5: WebSocket state synchronization
- [x] Stage 6: Integrated firmware initial implementation (hardware validation still pending)

Final firmware stays separate from individual component test sketches. Dashboard source remains separate and will be copied to LittleFS for deployment.

## Safety

Mains AC can cause fatal electric shock. Begin with low-voltage DC test loads. AC wiring must be completed and checked by a qualified person using suitable relay ratings, enclosed terminals, fuse protection, insulated wiring and a protective enclosure. Never handle exposed live conductors.

**Status:** In development; verify pin assignments and relay polarity against the actual hardware.