# ESP32 DC Power Monitor

An ESP32-based DC voltage/current monitor with an SSD1306 OLED, a Wi-Fi-hosted browser dashboard, and a JSON readings endpoint. The firmware calculates power from the sampled voltage and current and raises a software warning when its configured short-circuit thresholds are met.

> **Scope note:** despite the repository name, the current sketch does not implement over-the-air (OTA) firmware updates. Upload firmware through USB/serial. The short-circuit logic is an alert only; it does not disconnect the load and is not a protection device.

## Features

- Samples a voltage-divider input and an ACS712 current sensor.
- Calculates power as voltage × absolute current.
- Displays readings and warning status on an SSD1306 128×64 I²C OLED.
- Serves a dashboard at `/` and JSON readings at `/data` over the local Wi-Fi network.
- Includes a standalone HTML dashboard preview for UI inspection without the ESP32.

## Hardware and wiring

| Function | ESP32 connection |
|---|---|
| Voltage sensor output | GPIO 34 (ADC1) |
| ACS712 output | GPIO 35 (ADC1) |
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |
| Ground | Common ground, as required by the sensor circuit |

The detailed diagram is in [`wiring_diagram.md`](wiring_diagram.md). The sketch expects a voltage-divider module intended for a nominal 25 V DC range, an ACS712 breakout, and an SSD1306 I²C OLED. Confirm the ratings of the exact modules you own before powering them.

**Electrical safety:** use this as a low-voltage DC learning project only, not for mains/AC measurement. Never allow an ESP32 ADC pin to exceed its permitted input voltage (normally 3.3 V). Some ACS712 breakout boards require 5 V and can produce an output above 3.3 V; verify the board's supply and output range and add suitable signal conditioning if needed. Check the voltage-divider output with a multimeter before connecting it to the ESP32. Do not rely on the software warning to protect a battery, supply, load, or person.

## Software requirements

- Arduino IDE 2.x and the Espressif ESP32 Arduino board package.
- Libraries: **Adafruit GFX Library**, **Adafruit SSD1306**, **ESPAsyncWebServer**, and **AsyncTCP**.
- A USB data cable and a Wi-Fi network accessible to the ESP32 and browser.

The sketch uses `WiFi.h` from the ESP32 Arduino core. Library versions are not pinned in the repository.

## Configure, build, and upload

1. Inspect the wiring diagram and verify the sensor/module ratings before connecting power.
2. Open the root-level `config.h` and set `WIFI_SSID` and `WIFI_PASSWORD`. Keep real credentials out of public commits.
3. Match the calibration constants to your hardware: `VOLTAGE_DIVIDER_RATIO`, `ACS712_SENSITIVITY`, and `ACS712_ZERO_CURRENT_V`. Start with a known, safe low-voltage DC source and compare readings with a multimeter.
4. Open `esp32_power_monitor.ino` in Arduino IDE. Select **ESP32 Dev Module** (or the exact board), select its serial port, and upload.
5. Open Serial Monitor at **115200 baud**. If Wi-Fi connects, note the printed IP address and open `http://<ESP32-IP>/` on the same network.
6. Request `http://<ESP32-IP>/data` to inspect the current JSON response.

Do not connect a load until the voltage/current sensor wiring and calibration have been checked independently. The repository's build and hardware behavior have not been verified as part of this documentation pass.

## Configuration reference

| Setting | Default | Purpose |
|---|---:|---|
| `VOLTAGE_PIN` / `CURRENT_PIN` | GPIO 34 / 35 | ADC input pins |
| `VOLTAGE_DIVIDER_RATIO` | 5.128 | Converts ADC voltage to source voltage |
| `ACS712_SENSITIVITY` | 0.100 V/A | Must match the ACS712 variant |
| `ACS712_ZERO_CURRENT_V` | 1.65 V | Zero-current calibration point |
| `CURRENT_NOISE_FLOOR` | 0.05 A | Suppresses small readings |
| `MAX_CURRENT` | 5.0 A | Software overcurrent warning threshold |
| `MIN_VOLTAGE_SC` | 0.5 V | Low-voltage condition used by warning logic |
| `SAMPLE_INTERVAL_MS` | 500 | Reading interval |
| `CURRENT_AVG_SAMPLES` | 50 | ADC samples averaged for current |

The warning condition is evaluated in `sensors.h`. It flags current above `MAX_CURRENT`, or voltage below `MIN_VOLTAGE_SC` while positive current is above 0.1 A. It does **not** turn power off, and reverse-current over-threshold behavior is not covered by the current condition.

## HTTP interface

| Path | Method | Purpose |
|---|---|---|
| `/` | GET | Dashboard HTML embedded in `dashboard.h` |
| `/data` | GET | JSON snapshot: `voltage`, `current`, `power`, and `short_circuit` |

The standalone `index.html` is a separate browser preview; it is not the firmware-served page and does not itself read sensor hardware.

## Repository map

| Path | Role |
|---|---|
| `esp32_power_monitor.ino` | Firmware entry point, Wi-Fi setup, HTTP routes, OLED rendering, sampling loop |
| `config.h` | Pin map, calibration, thresholds, sampling, Wi-Fi credentials |
| `sensors.h` | Sensor calculations, warning condition, JSON serialization |
| `dashboard.h` | HTML served from ESP32 flash |
| `index.html` | Standalone dashboard preview |
| `wiring_diagram.md` | Wiring and electrical notes |
| `LICENSE (1)` | License file currently present under this filename |

## Troubleshooting

- **OLED does not start:** check SDA/SCL, ground, power, and I²C address (`0x3C` or `0x3D` in `config.h`).
- **Wi-Fi does not connect:** verify credentials, 2.4 GHz network compatibility, and Serial Monitor messages.
- **Readings are offset:** calibrate the divider ratio and ACS712 zero point against a multimeter with a safe, known source.
- **Current is clipped or implausible:** check the exact ACS712 variant, supply, output range, wiring, and sensitivity constant.
- **Dashboard is unreachable:** confirm the ESP32 and browser are on the same LAN and use the printed IP address.
- **False short-circuit warning:** verify sensor calibration and the configured thresholds; the warning is not a validated protection function.

## Verification status

Documentation was reviewed against the current root-level sketch, configuration, sensor helpers, dashboard header, and wiring notes. No firmware compilation, calibration, network test, or physical hardware test is claimed.

## Maintenance & audit

- **Audit date:** 13 November 2026
- **Scope:** documentation-only review; source, pin assignments, and hardware behavior were not changed.
- **Verification:** build and hardware/runtime behavior remain unverified.
