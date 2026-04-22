# ESP32 Power Monitor

A real-time voltage and current monitoring system built on the ESP32.
Readings are shown on an SSD1306 OLED display and served as a live web
dashboard over WiFi. The system automatically detects short circuit conditions.

---

## Features

- Real-time voltage, current, and power measurement
- SSD1306 OLED display (128x64, I2C)
- WiFi web dashboard with auto-refresh every 2 seconds
- JSON API endpoint at `/data` for integration with other systems
- Short circuit detection (overcurrent and voltage collapse)
- Reading history table on the dashboard
- Standalone HTML preview file for UI development without hardware

---

## Hardware Required

| Component | Notes |
|---|---|
| ESP32 Dev Board | Any standard 38-pin or 30-pin module |
| Voltage sensor module | 25V DC range (resistor divider type) |
| ACS712 current sensor | 5A, 20A, or 30A variant |
| SSD1306 OLED | 128x64 pixels, I2C interface |
| Jumper wires | |
| Breadboard or PCB | |

---

## Wiring

```
Voltage Sensor OUT  -->  GPIO 34  (ADC1 CH6)
ACS712 OUT          -->  GPIO 35  (ADC1 CH7)
OLED SDA            -->  GPIO 21
OLED SCL            -->  GPIO 22
All sensor GNDs     -->  GND
Sensor VCC          -->  3.3V (check your module's rating)
```

> Use only ADC1 pins (GPIO 32-39) for analog reads. ADC2 is unavailable
> when WiFi is active on the ESP32.

---

## Software Dependencies

Install these libraries through the Arduino IDE Library Manager:

- `Adafruit SSD1306` (by Adafruit)
- `Adafruit GFX Library` (by Adafruit)
- `ESPAsyncWebServer` (by me-no-dev)
- `AsyncTCP` (by me-no-dev)

The `WiFi.h` library is bundled with the ESP32 Arduino core.

---

## Configuration

Open `src/config.h` and edit the values for your setup before flashing.

```cpp
// WiFi credentials
#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"

// ACS712 sensitivity (V/A)
//   5A  module --> 0.185
//   20A module --> 0.100
//   30A module --> 0.066
#define ACS712_SENSITIVITY  0.100f

// Overcurrent trip level (Amps)
#define MAX_CURRENT  5.0f
```

See `config.h` for a full description of every setting.

---

## Calibration

### Voltage

1. Apply a known voltage to the input (e.g. measure with a multimeter).
2. Read the raw value from the Serial monitor.
3. Adjust `VOLTAGE_DIVIDER_RATIO` in `config.h` until the displayed value matches.

### Current (ACS712 zero-point)

1. Connect the ESP32 and sensor but leave the load disconnected.
2. Open the Serial monitor and note the ADC voltage on `CURRENT_PIN`.
3. Set `ACS712_ZERO_CURRENT_V` in `config.h` to that value.
   On a 3.3 V supply the typical value is around 1.65 V.

---

## Flashing

1. Open `src/esp32_power_monitor.ino` in the Arduino IDE.
2. Select board: `ESP32 Dev Module` (or your specific variant).
3. Set upload speed to `115200`.
4. Click Upload.
5. After boot, the OLED displays the assigned IP address.
6. Open a browser and navigate to `http://<IP-ADDRESS>`.

---

## File Structure

```
esp32-power-monitor/
  src/
    esp32_power_monitor.ino   Main sketch
    config.h                  All user-configurable constants
    sensors.h                 Sensor read functions and SensorData struct
    dashboard.h               Web dashboard HTML stored in PROGMEM
  dashboard/
    index.html                Standalone browser preview (no hardware needed)
  docs/
    wiring_diagram.md         Detailed wiring notes
  README.md
  LICENSE
  .gitignore
```

---

## API

When the ESP32 is connected to WiFi, the following HTTP endpoints are available:

| Endpoint | Method | Description |
|---|---|---|
| `/` | GET | Web dashboard (HTML) |
| `/data` | GET | Live sensor data (JSON) |

Example `/data` response:

```json
{
  "voltage": 12.043,
  "current": 0.4820,
  "power": 5.805,
  "short_circuit": false
}
```

---

## Short Circuit Detection

A short circuit is flagged when either of these conditions is true:

1. Current exceeds `MAX_CURRENT` (default 5.0 A)
2. Voltage drops below `MIN_VOLTAGE_SC` (default 0.5 V) while current is above 0.1 A

When triggered, the OLED switches to a full-screen warning and the web
dashboard status bar blinks red. The condition clears automatically on the
next reading cycle if normal values are restored.

---

## License

MIT License. See `LICENSE` for details.
