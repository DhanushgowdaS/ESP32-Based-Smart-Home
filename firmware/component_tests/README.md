# Component Test Firmware

Upload **one test sketch at a time** to the ESP32 DevKit V1. These tests are independent of the final integration firmware.

## Planned pin map

| Component | ESP32 GPIO |
|---|---|
| Relay IN1–IN4 | 23, 19, 18, 27 |
| Switch S1–S4 | 13, 14, 32, 33 |
| I2C LCD SDA | 21 |
| I2C LCD SCL | 22 |

Verify these pin assignments against the physical wiring before connecting. ESP32 GPIOs are 3.3 V logic; do not apply 5 V to GPIO pins.

## Tests

### 1. Relay
Path: `relay_test/relay_test.ino`

- Keep mains AC disconnected for initial testing.
- Confirm relay board input polarity; code currently assumes active-LOW.
- Serial Monitor: 115200 baud, newline.
- Commands: `1ON`, `1OFF` ... `4ON`, `4OFF`, `ALLON`, `ALLOFF`, `STATUS`.

### 2. Physical switches
Path: `switch_test/switch_test.ino`

- GPIO 13, 14, 32, 33 are configured with `INPUT_PULLUP`.
- Each test switch should connect its GPIO input to GND.
- Serial output reports press/release with debounce.

### 3. LCD
Path: `lcd_test/lcd_test.ino`

- SDA GPIO21, SCL GPIO22.
- LCD backpack address default is 0x27. If blank, run the I2C scanner and change address to the detected value.
- Install the LiquidCrystal_I2C library compatible with the sketch.

### 4. I2C scanner
Path: `i2c_scanner/i2c_scanner.ino`

- Scans addresses 0x01–0x7E and prints detected devices.
- Use to confirm the LCD backpack address.

### 5. Wi-Fi
Path: `wifi_test/wifi_test.ino`

- Fill in router/hotspot SSID and password before upload.
- Tests STA connection and creates a separate ESP32 AP.
- AP credentials in the test: `SmartHome-ESP32` / `smarthome123`.

### 6. LittleFS
Path: `littlefs_test/littlefs_test.ino`

- Tests mount, file write, and file read-back.
- Does not auto-format on mount failure, preserving existing filesystem data.

## Safety

For initial relay validation, disconnect AC loads and test the relay indicators only. Mains wiring is hazardous and must be handled by a qualified person using properly rated components, enclosure, insulation, and protection. Never work with exposed live wiring.