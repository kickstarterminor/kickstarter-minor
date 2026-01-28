#include "Mapper.h"

void printMappingJson(const SensorMatrix* sensorMatrix) {
  if (sensorMatrix == nullptr) return;
  uint8_t total = sensorMatrix->totalSensors();
  Serial.print(F("{\"mapping\":["));
  for (uint8_t i = 0; i < total; ++i) {
    int r = sensorMatrix->rowOf(i);
    int c = sensorMatrix->colOf(i);
    char rowLetter = 'A' + (char)r;
    Serial.print(F("{\"id\":"));
    Serial.print(i);
    Serial.print(F(",\"label\":\""));
    Serial.print(rowLetter);
    Serial.print(c + 1);
    Serial.print(F("\",\"row\":"));
    Serial.print(r);
    Serial.print(F(",\"col\":"));
    Serial.print(c);
    Serial.print(F("}"));
    if (i + 1 < total) Serial.print(',');
  }
  Serial.println(F("]}"));
}