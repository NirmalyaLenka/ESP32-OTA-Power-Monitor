#pragma once

// ===========================================================================
//  config.h  --  User-configurable settings
//  Edit this file to match your hardware and network.
// ===========================================================================

// ---------------------------------------------------------------------------
// WiFi
// ---------------------------------------------------------------------------
#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"

// ---------------------------------------------------------------------------
// Pin assignments
// ---------------------------------------------------------------------------
#define VOLTAGE_PIN         34      // ADC1 channel  (GPIO 34)
#define CURRENT_PIN         35      // ADC1 channel  (GPIO 35)
#define OLED_RESET_PIN      -1      // -1 = share reset with ESP32 EN pin

// ---------------------------------------------------------------------------
// OLED
// ---------------------------------------------------------------------------
#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       64
#define OLED_I2C_ADDRESS    0x3C    // Use 0x3D if 0x3C does not respond

// ---------------------------------------------------------------------------
// ADC reference
// ---------------------------------------------------------------------------
#define ADC_REF_VOLTAGE     3.3f
#define ADC_RESOLUTION      4095.0f

// ---------------------------------------------------------------------------
// Voltage sensor calibration
// Vout = Vin * R2 / (R1 + R2)  -->  Vin = Vout * (R1 + R2) / R2
// Default module ratio is ~5.128 for a 25V range module.
// Measure with a multimeter and adjust until readings match.
// ---------------------------------------------------------------------------
#define VOLTAGE_DIVIDER_RATIO   5.128f

// ---------------------------------------------------------------------------
// ACS712 current sensor calibration
//   5A  module  --> sensitivity = 0.185 V/A
//   20A module  --> sensitivity = 0.100 V/A
//   30A module  --> sensitivity = 0.066 V/A
//
// ACS712_ZERO_CURRENT_V:
//   With no load connected, read the raw ADC value, convert to voltage,
//   and set this to that value. Ideally VCC/2 = 1.65V on a 3.3V supply.
// ---------------------------------------------------------------------------
#define ACS712_SENSITIVITY      0.100f   // V/A  -- change for your module
#define ACS712_ZERO_CURRENT_V   1.65f    // Volts -- calibrate to your board
#define CURRENT_NOISE_FLOOR     0.05f    // Amps below which reading is zeroed

// ---------------------------------------------------------------------------
// Short circuit thresholds
//   Adjust MAX_CURRENT to just above your normal operating current.
// ---------------------------------------------------------------------------
#define MAX_CURRENT         5.0f    // Amps  -- overcurrent trip level
#define MIN_VOLTAGE_SC      0.5f    // Volts -- low voltage floor for SC check

// ---------------------------------------------------------------------------
// Sampling
// ---------------------------------------------------------------------------
#define SAMPLE_INTERVAL_MS  500     // milliseconds between sensor reads
#define CURRENT_AVG_SAMPLES 50      // number of ADC samples averaged per read

// ---------------------------------------------------------------------------
// Web server
// ---------------------------------------------------------------------------
#define WEB_SERVER_PORT     80
