#include <Arduino.h>
#include "MuxScanner.h"
#include "SensorMatrix.h"
#include "Mapper.h"
#include "BinaryOutput.h"
#include <stdlib.h>

const int rows[] = {4, 3, 3}; // change this to modify sensor layout
const size_t NUM_ROWS = sizeof(rows) / sizeof(rows[0]);
const uint8_t MAX_MUX_CHANNELS = 16;

const uint8_t MUX_S0 = 12;
const uint8_t MUX_S1 = 13;
const uint8_t MUX_S2 = 14;
const uint8_t MUX_S3 = 27;
const int     MUX_EN = -1; // set to pin number if EN (active LOW) is connected, otherwise -1
const uint8_t ADC_PIN = 34;

const uint8_t SAMPLES = 8;          // ADC samples per channel (averaging)
const int SCAN_INTERVAL_MS = 200;   // time between full scans

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

  // Send compact binary frame (no timestamp by default).
  // Frame layout and decoding notes are documented in include/BinaryOutput.h.
  // We use grid_id "A1" and node_id "N03" (see id_block in the header).
  const char* gridId = "A1";
  const char* nodeId = "N03";
  const float vref = 3.3f;
  const uint32_t adcMax = 4095u;

  // Only send up to 12 channels (frame defines 12 float slots). If fewer sensors
  // exist, the remaining floats are padded with 0.0. If more exist, we send the
  // first 12 values.
  uint8_t sendCount = (totalSensors > 12) ? 12 : totalSensors;
  printBinaryFrame(gridId, nodeId, sensorValues, sendCount, vref, adcMax);
}

// Print mapping once at startup as JSON so the host can map sensor ids to labels.
// Example output:
// {"mapping":[{"id":0,"label":"A1","row":0,"col":0}, {"id":1,"label":"A2","row":0,"col":1}, ...]}
/* Mapping JSON printing moved to src/Mapper.cpp.
   Use printMappingJson(sensorMatrix) declared in include/Mapper.h */

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("FSR HC4067 scanner starting...");

  analogReadResolution(12);
  analogSetPinAttenuation(ADC_PIN, ADC_11db);

  // create objects after Serial is available (SensorMatrix may print errors)
  muxScanner = new MuxScanner(MUX_S0, MUX_S1, MUX_S2, MUX_S3, MUX_EN, ADC_PIN, SAMPLES);
  muxScanner->begin();

  sensorMatrix = new SensorMatrix(rows, NUM_ROWS, MAX_MUX_CHANNELS);
  sensorMatrix->printLayout();
  // print mapping as JSON once at startup so the host can map channel ids to labels
  printMappingJson(sensorMatrix);

  totalSensors = sensorMatrix->totalSensors();
  if (totalSensors == 0) {
    Serial.println("No sensors defined. Halting.");
    while (true) delay(1000);
  }

  // allocate values buffer
  sensorValues = (int*)malloc(sizeof(int) * totalSensors);
  if (!sensorValues) {
    Serial.println("Failed to allocate sensorValues buffer. Halting.");
    while (true) delay(1000);
  }
  for (uint8_t i = 0; i < totalSensors; ++i) sensorValues[i] = 0;

  lastScanMs = millis();
  Serial.println("Setup complete. Scanning started.");
}

void loop() {
  unsigned long now = millis();
  // handle periodic scans without FreeRTOS
  if ((long)(now - lastScanMs) >= SCAN_INTERVAL_MS) {
    performScanAndPrint();
    lastScanMs = now;
  }

  // yield to background tasks and avoid busy loop
  delay(1);
}