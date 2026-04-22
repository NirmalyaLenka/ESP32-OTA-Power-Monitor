# Wiring Diagram

## Schematic (text form)

```
                      3.3V Rail
                         |
          +--------------+--------------+
          |              |              |
    [Voltage Module]  [ACS712]     [SSD1306 OLED]
    VCC --> 3.3V      VCC --> 3.3V  VCC --> 3.3V
    GND --> GND       GND --> GND   GND --> GND
    OUT --> GPIO 34   OUT --> GPIO 35
                                    SDA --> GPIO 21
                                    SCL --> GPIO 22

    Load side:
    [DC Power Source] --> [Voltage Module IN+] --> [ACS712 IP+] --> [Load+]
                          [Voltage Module IN-] --> [Load-] --> [GND]
                                                  [ACS712 IP-] --> [Load-]
```

## Notes

- GPIO 34 and 35 are input-only pins on the ESP32 and have no internal
  pull-up/pull-down resistors. This makes them ideal for clean ADC reads.
- Do not use ADC2 pins (GPIO 0, 2, 4, 12-15, 25-27) for analog reads
  when WiFi is enabled. ADC2 is shared with the WiFi radio.
- The ACS712 output voltage range on a 3.3 V supply spans 0 V to 3.3 V.
  At zero current the output sits at approximately VCC/2 = 1.65 V.
- The voltage divider module attenuates the input to a safe 0-3.3 V range.
  Check the module's maximum input rating before connecting.
- Keep analog signal wires short and away from the ESP32's antenna area
  to reduce noise.

## I2C OLED Address

Most SSD1306 modules use address 0x3C.
If the display does not initialize, try 0x3D in config.h.
You can also scan for the address by running an I2C scanner sketch.

## Power Supply

When testing on a bench supply:
- Connect the bench supply positive to the voltage sensor input.
- Connect bench supply negative to the common GND rail shared with the ESP32.
- Do not exceed the voltage sensor module's rated input voltage (typically 25 V).
