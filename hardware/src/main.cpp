#include <Arduino.h>
#include "MuxScanner.h"
#include "SensorMatrix.h"
#include "Mapper.h"
#include "BinaryOutput.h"
#include <stdlib.h>
#include <LiquidCrystal_I2C.h>
#include "AlarmUI.h"

const int rows[] = {2, 1}; // change this to modify sensor layout
const size_t NUM_ROWS = sizeof(rows) / sizeof(rows[0]);
const uint8_t MAX_MUX_CHANNELS = 16;

const uint8_t MUX_S0 = 14;
const uint8_t MUX_S1 = 27;
const uint8_t MUX_S2 = 26;
const uint8_t MUX_S3 = 25;
const int     MUX_EN = 15; // HC4067 EN (active LOW). Wire MUX EN to this GPIO (change if needed)
const uint8_t ADC_PIN = 34;

const uint8_t SAMPLES = 8;          // ADC samples per channel (averaging)
const int SCAN_INTERVAL_MS = 5000;  // time between full scans (ms) — 5000ms = 5s

// Using an I2C LCD backpack (4 wires: VCC, GND, SDA, SCL).
// Common I2C addresses are 0x27 or 0x3F depending on the adapter.
const uint8_t LCD_I2C_ADDR = 0x27;
const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 2;

// Buttons for alarm UI - active LOW (use INPUT_PULLUP)
const uint8_t BTN_CORRECT = 32; // confirm
const uint8_t BTN_WRONG   = 33; // cancel/back
const uint8_t BTN_UP      = 4;
const uint8_t BTN_DOWN    = 16;

LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);
AlarmUI alarmUi(lcd, BTN_CORRECT, BTN_WRONG, BTN_UP, BTN_DOWN);

MuxScanner* muxScanner = nullptr;
SensorMatrix* sensorMatrix = nullptr;

int* sensorValues = nullptr;
uint8_t totalSensors = 0;

unsigned long lastScanMs = 0;

void performScanAndPrint() {
  if (totalSensors == 0) return;

  // iterate over sensor ids (mapped to mux channels by SensorMatrix)
  for (uint8_t id = 0; id < totalSensors; ++id) {
    int raw = muxScanner->readChannel(id);
    sensorValues[id] = raw;
    // small pause to avoid hammering the ADC/MUX
    delay(2);
  }

  // Build a compact CSV-style string: first the matrix layout (e.g. "210" or "344"),
  // then comma-separated raw values: <matrix>,val0,val1,...
  String matrixStr;
  for (size_t r = 0; r < sensorMatrix->numRows(); ++r) {
    matrixStr += String(sensorMatrix->rowCount(r));
  }
  String csv;
  csv += matrixStr;
  for (uint8_t id = 0; id < totalSensors; ++id) {
    csv += ",";
    csv += String(sensorValues[id]);
  }
  Serial.println(csv);

  // Human-readable output for debugging / terminal monitoring.
  const float vref = 3.3f;
  const float adcMaxF = 4095.0f;

  Serial.println("---- Scan ----");
  for (uint8_t id = 0; id < totalSensors; ++id) {
    int raw = sensorValues[id];
    int row = sensorMatrix->rowOf(id);
    int col = sensorMatrix->colOf(id);

    // Build a simple label: A1, B2, ... falling back to R#C# if row >= 26.
    String label;
    if (row >= 0 && row < 26) {
      label += char('A' + row);
      label += String(col + 1);
    } else if (row >= 0) {
      label = "R" + String(row) + "C" + String(col + 1);
    } else {
      label = "ID" + String(id);
    }

    float voltage = ((float)raw / adcMaxF) * vref;

    Serial.print(label);
    Serial.print(" id="); Serial.print(id);
    Serial.print(" row="); Serial.print(row);
    Serial.print(" col="); Serial.print(col);
    Serial.print(" raw="); Serial.print(raw);
    Serial.print(" V="); Serial.println(voltage, 3);
  }
  Serial.println("--------------");

  // Also keep the compact binary frame for automated hosts that expect it.
  const char* gridId = "A1";
  const char* nodeId = "N03";
  const uint32_t adcMax = 4095u;
  uint8_t sendCount = (totalSensors > 12) ? 12 : totalSensors;
  printBinaryFrame(gridId, nodeId, sensorValues, sendCount, vref, adcMax);
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("FSR HC4067 scanner starting...");

  analogReadResolution(12);
  analogSetPinAttenuation(ADC_PIN, ADC_11db);

  muxScanner = new MuxScanner(MUX_S0, MUX_S1, MUX_S2, MUX_S3, MUX_EN, ADC_PIN, SAMPLES);
  muxScanner->begin();

  sensorMatrix = new SensorMatrix(rows, NUM_ROWS, MAX_MUX_CHANNELS);
  sensorMatrix->printLayout();
  printMappingJson(sensorMatrix);

  totalSensors = sensorMatrix->totalSensors();
  if (totalSensors == 0) {
    Serial.println("No sensors defined. Halting.");
    while (true) delay(1000);
  }

  sensorValues = (int*)malloc(sizeof(int) * totalSensors);
  if (!sensorValues) {
    Serial.println("Failed to allocate sensorValues buffer. Halting.");
    while (true) delay(1000);
  }
  for (uint8_t i = 0; i < totalSensors; ++i) sensorValues[i] = 0;

  lastScanMs = millis();
  Serial.println("Setup complete. Scanning started.");
  // initialize LCD and alarm UI (non-blocking)
  alarmUi.begin();
}


void loop() {
  unsigned long now = millis();
  // run alarm UI so the display updates and buttons are processed
  alarmUi.loop();
  // handle periodic scans without FreeRTOS
  if ((long)(now - lastScanMs) >= SCAN_INTERVAL_MS) {
    performScanAndPrint();
    lastScanMs = now;
  }

  // yield to background tasks and avoid busy loop
  delay(1);
}
