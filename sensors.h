#pragma once

#include <Arduino.h>
#include "config.h"

// ---------------------------------------------------------------------------
//  SensorData  --  snapshot of one read cycle
// ---------------------------------------------------------------------------
struct SensorData {
    float voltage      = 0.0f;
    float current      = 0.0f;
    float power        = 0.0f;
    bool  shortCircuit = false;
};

// ---------------------------------------------------------------------------
//  readVoltage()
//  Reads the voltage divider sensor on VOLTAGE_PIN and returns the real
//  upstream voltage in Volts.
// ---------------------------------------------------------------------------
inline float readVoltage() {
    int   raw        = analogRead(VOLTAGE_PIN);
    float adcVoltage = (raw / ADC_RESOLUTION) * ADC_REF_VOLTAGE;
    return adcVoltage * VOLTAGE_DIVIDER_RATIO;
}

// ---------------------------------------------------------------------------
//  readCurrent()
//  Averages CURRENT_AVG_SAMPLES reads from the ACS712 on CURRENT_PIN and
//  returns current in Amps (positive = forward, negative = reverse).
// ---------------------------------------------------------------------------
inline float readCurrent() {
    long sum = 0;
    for (int i = 0; i < CURRENT_AVG_SAMPLES; i++) {
        sum += analogRead(CURRENT_PIN);
        delayMicroseconds(200);
    }
    float raw        = (float)sum / CURRENT_AVG_SAMPLES;
    float adcVoltage = (raw / ADC_RESOLUTION) * ADC_REF_VOLTAGE;
    float current    = (adcVoltage - ACS712_ZERO_CURRENT_V) / ACS712_SENSITIVITY;

    // Suppress noise around zero
    if (abs(current) < CURRENT_NOISE_FLOOR) {
        current = 0.0f;
    }
    return current;
}

// ---------------------------------------------------------------------------
//  detectShortCircuit()
//  Returns true when any short-circuit condition is met:
//    1. Current exceeds MAX_CURRENT (overcurrent)
//    2. Voltage collapses below MIN_VOLTAGE_SC while current is flowing
// ---------------------------------------------------------------------------
inline bool detectShortCircuit(float v, float i) {
    if (i > MAX_CURRENT)                        return true;
    if (v < MIN_VOLTAGE_SC && i > 0.1f)         return true;
    return false;
}

// ---------------------------------------------------------------------------
//  buildJSON()
//  Returns a JSON string for the /data endpoint.
// ---------------------------------------------------------------------------
inline String buildJSON(const SensorData& d) {
    String json = "{";
    json += "\"voltage\":"       + String(d.voltage, 3) + ",";
    json += "\"current\":"       + String(d.current, 4) + ",";
    json += "\"power\":"         + String(d.power,   3) + ",";
    json += "\"short_circuit\":" + String(d.shortCircuit ? "true" : "false");
    json += "}";
    return json;
}
