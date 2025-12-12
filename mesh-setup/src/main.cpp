#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "MuxScanner.h"
#include "SensorMatrix.h"
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

SemaphoreHandle_t readingsMutex = NULL;
TaskHandle_t serialTaskHandle = NULL;

int* sensorValues = nullptr;
uint8_t totalSensors = 0;


void sensorTask(void* pvParameters) {
  (void)pvParameters;
  for (;;) {
    // iterate over sensor ids (mapped to mux channels by SensorMatrix)
    for (uint8_t id = 0; id < totalSensors; ++id) {
      int raw = muxScanner->readChannel(id);

      if (xSemaphoreTake(readingsMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        sensorValues[id] = raw;
        xSemaphoreGive(readingsMutex);
      }
      // small pause to avoid hammering the ADC/MUX
      vTaskDelay(pdMS_TO_TICKS(2));
    }

    // notify serial printer that a new frame is ready
    if (serialTaskHandle) {
      xTaskNotifyGive(serialTaskHandle);
    }

    vTaskDelay(pdMS_TO_TICKS(SCAN_INTERVAL_MS));
  }
}

void serialTask(void* pvParameters) {
  (void)pvParameters;
  for (;;) {
    // wait until sensorTask signals new data
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    if (totalSensors == 0) continue;

    // snapshot values quickly while holding the mutex
    int* snapshot = (int*)malloc(sizeof(int) * totalSensors);
    if (!snapshot) {
      Serial.println("serialTask: snapshot malloc failed");
      continue;
    }

    if (xSemaphoreTake(readingsMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
      for (uint8_t i = 0; i < totalSensors; ++i) snapshot[i] = sensorValues[i];
      xSemaphoreGive(readingsMutex);
    } else {
      free(snapshot);
      continue;
    }

    unsigned long t = millis();
    // print CSV lines: timestamp_ms,id,row,col,raw_adc
    for (uint8_t i = 0; i < totalSensors; ++i) {
      int r = sensorMatrix->rowOf(i);
      int c = sensorMatrix->colOf(i);
      Serial.printf("%lu,%u,%d,%d,%d\n", t, (unsigned int)i, r, c, snapshot[i]);
    }
    Serial.flush();
    free(snapshot);
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("FSR HC4067 scanner (clean main) starting...");

  analogReadResolution(12);
  analogSetPinAttenuation(ADC_PIN, ADC_11db);

  // create objects after Serial is available (SensorMatrix may print errors)
  muxScanner = new MuxScanner(MUX_S0, MUX_S1, MUX_S2, MUX_S3, MUX_EN, ADC_PIN, SAMPLES);
  muxScanner->begin();

  sensorMatrix = new SensorMatrix(rows, NUM_ROWS, MAX_MUX_CHANNELS);
  sensorMatrix->printLayout();

  totalSensors = sensorMatrix->totalSensors();
  if (totalSensors == 0) {
    Serial.println("No sensors defined. Halting.");
    while (1) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  // allocate values buffer
  sensorValues = (int*)malloc(sizeof(int) * totalSensors);
  if (!sensorValues) {
    Serial.println("Failed to allocate sensorValues buffer. Halting.");
    while (1) vTaskDelay(pdMS_TO_TICKS(1000));
  }
  for (uint8_t i = 0; i < totalSensors; ++i) sensorValues[i] = 0;

  // create mutex
  readingsMutex = xSemaphoreCreateMutex();
  if (readingsMutex == NULL) {
    Serial.println("Failed to create mutex. Halting.");
    while (1) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  // create tasks
  BaseType_t ok;
  ok = xTaskCreatePinnedToCore(sensorTask, "SensorTask", 4096, NULL, 1, NULL, 1);
  if (ok != pdPASS) {
    Serial.println("Failed to create SensorTask. Halting.");
    while (1) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  ok = xTaskCreatePinnedToCore(serialTask, "SerialTask", 4096, NULL, 1, &serialTaskHandle, 0);
  if (ok != pdPASS) {
    Serial.println("Failed to create SerialTask. Halting.");
    while (1) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  Serial.println("Setup complete. Scanning started.");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}