/*
 * ESP32 Voltage & Current Monitoring System
 * ==========================================
 * Author  : Your Name
 * License : MIT
 *
 * Features:
 *   - Real-time voltage and current sensing
 *   - Power calculation (P = V * I)
 *   - Short circuit detection
 *   - SSD1306 OLED display output
 *   - WiFi web dashboard (served from ESP32)
 *
 * Hardware:
 *   - ESP32 Dev Board
 *   - Voltage sensor module (25V DC)
 *   - ACS712 current sensor (5A / 20A / 30A)
 *   - SSD1306 OLED 128x64 (I2C)
 *
 * Wiring:
 *   Voltage Sensor OUT  --> GPIO 34
 *   ACS712 OUT          --> GPIO 35
 *   OLED SDA            --> GPIO 21
 *   OLED SCL            --> GPIO 22
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "sensors.h"
#include "dashboard.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);
AsyncWebServer server(WEB_SERVER_PORT);

SensorData sensorData;
unsigned long lastReadTime = 0;

// ---------------------------------------------------------------------------
// OLED rendering
// ---------------------------------------------------------------------------

void renderOLED() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("POWER MONITOR");
    display.drawLine(0, 9, 127, 9, SSD1306_WHITE);

    if (sensorData.shortCircuit) {
        display.setTextSize(2);
        display.setCursor(4, 16);
        display.print("!! SHORT");
        display.setCursor(4, 36);
        display.print("CIRCUIT!");
        display.setTextSize(1);
        display.setCursor(14, 56);
        display.print("CHECK CONNECTIONS");
    } else {
        // Voltage
        display.setTextSize(1);
        display.setCursor(0, 13);
        display.print("V:");
        display.setTextSize(2);
        display.setCursor(16, 11);
        display.print(sensorData.voltage, 2);
        display.print("V");

        // Current
        display.setTextSize(1);
        display.setCursor(0, 33);
        display.print("I:");
        display.setTextSize(2);
        display.setCursor(16, 31);
        display.print(sensorData.current, 3);
        display.print("A");

        // Power
        display.setTextSize(1);
        display.setCursor(0, 55);
        display.print("P: ");
        display.print(sensorData.power, 2);
        display.print(" W");
    }

    display.display();
}

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("[BOOT] ESP32 Power Monitor starting...");

    // OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
        Serial.println("[ERROR] SSD1306 OLED not found. Check wiring.");
        while (true) { delay(1000); }
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(10, 20);
    display.print("Connecting WiFi...");
    display.display();

    // WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("[WIFI] Connecting");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    display.clearDisplay();
    if (WiFi.status() == WL_CONNECTED) {
        String ip = WiFi.localIP().toString();
        Serial.println("\n[WIFI] Connected. IP: " + ip);
        display.setCursor(0, 10);
        display.print("WiFi Connected");
        display.setCursor(0, 25);
        display.print("Open browser at:");
        display.setCursor(0, 38);
        display.print(ip);
        display.display();
        delay(3000);
    } else {
        Serial.println("\n[WIFI] Failed. Running in offline mode.");
        display.setCursor(0, 20);
        display.print("WiFi Failed");
        display.setCursor(0, 35);
        display.print("Offline Mode");
        display.display();
        delay(2000);
    }

    // Web server routes
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", getDashboardHTML());
    });

    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
        String json = buildJSON(sensorData);
        request->send(200, "application/json", json);
    });

    server.begin();
    Serial.println("[SERVER] Web server started on port " + String(WEB_SERVER_PORT));
}

// ---------------------------------------------------------------------------
// Loop
// ---------------------------------------------------------------------------

void loop() {
    unsigned long now = millis();
    if (now - lastReadTime >= SAMPLE_INTERVAL_MS) {
        lastReadTime = now;

        sensorData.voltage      = readVoltage();
        sensorData.current      = readCurrent();
        sensorData.power        = sensorData.voltage * abs(sensorData.current);
        sensorData.shortCircuit = detectShortCircuit(sensorData.voltage, sensorData.current);

        Serial.printf("[DATA] V=%.2fV  I=%.3fA  P=%.2fW  SC=%s\n",
            sensorData.voltage,
            sensorData.current,
            sensorData.power,
            sensorData.shortCircuit ? "YES" : "No"
        );

        renderOLED();
    }
}
