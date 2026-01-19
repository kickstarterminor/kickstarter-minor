#ifndef BINARYOUTPUT_H
#define BINARYOUTPUT_H

#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/*
  include/BinaryOutput.h

  Implements a small, fixed binary frame for sending sensor data over Serial
  (compact, no external dependencies / no protobuf).

  Frame layout (all multi-byte values are little-endian on ESP32):

    Offset  Size  Field
    0       2     MAGIC: 0xA5 0x5A
    2       1     version (uint8_t) = 1
    3       1     flags (uint8_t) - bit0: timestamp present (1), else 0. other bits reserved
    4       8     id_block (8 bytes):
                   - bytes 0-1: grid_id (2 ASCII chars) padded with 0x00
                   - bytes 2-4: node_id (3 ASCII chars) padded with 0x00
                   - bytes 5-7: reserved (set to 0)
   12      48     payload: 12 x float32 (IEEE-754) voltages (in volts), little-endian.
                   - If fewer than 12 sensors, remaining entries are 0.0f.
   60      2     CRC16 (uint16_t), little-endian. CRC is CRC-CCITT (poly 0x1021, init 0xFFFF)
                   CRC is calculated over the payload starting at 'version' (offset 2)
                   up to the end of the floats (offset 59), i.e. CRC covers version..floats.

  Total frame size: 62 bytes.

  Example usage (from src/main.cpp):
    // after filling int sensorValues[] with raw ADC readings:
    printBinaryFrame("A1", "N03", sensorValues, totalSensors, 3.3f, 4095u);

  Notes / decoder hints:
   - All floats are written in native IEEE-754 32-bit little-endian (ESP32 is little-endian).
   - Receiver should read the magic (2 bytes), then read the next 58 bytes (version..floats),
     compute CRC over those 58 bytes and compare with the trailing 2 CRC bytes.
   - If you prefer raw ADC integers instead of floats, modify printBinaryFrame to write uint16_t entries.
*/

static inline uint16_t crc16_ccitt(const uint8_t *data, size_t len) {
  uint16_t crc = 0xFFFF;
  while (len--) {
    crc ^= (uint16_t)(*data++) << 8;
    for (uint8_t i = 0; i < 8; ++i) {
      if (crc & 0x8000) crc = (uint16_t)((crc << 1) ^ 0x1021);
      else crc = (uint16_t)(crc << 1);
    }
  }
  return crc;
}

/*
  printBinaryFrame(...)
   - gridId: short ASCII grid id (e.g. "A1") (only up to 2 chars used)
   - nodeId: short ASCII node id (e.g. "N03") (only up to 3 chars used)
   - rawValues: array of raw ADC readings (ints, e.g. 0..4095 for 12-bit)
   - numValues: number of elements in rawValues
   - vref: ADC reference voltage used to convert to volts (e.g. 3.3)
   - adcMax: maximum ADC value (default 4095 for 12-bit)
*/
static inline void printBinaryFrame(const char* gridId, const char* nodeId,
                                    const int* rawValues, uint8_t numValues,
                                    float vref, uint32_t adcMax = 4095u) {
  // payload = version(1) + flags(1) + id_block(8) + 12*float32(48) = 58 bytes
  const size_t PAYLOAD_SZ = 1 + 1 + 8 + (12 * 4);
  uint8_t payload[PAYLOAD_SZ];
  memset(payload, 0, PAYLOAD_SZ);
  size_t p = 0;

  // version
  payload[p++] = 0x01;
  // flags (no timestamp by default)
  payload[p++] = 0x00;

  // id_block: 8 bytes
  char id_block[8];
  memset(id_block, 0, sizeof(id_block));
  if (gridId) {
    // copy up to 2 chars for grid id
    for (uint8_t i = 0; i < 2 && gridId[i]; ++i) id_block[i] = gridId[i];
  }
  if (nodeId) {
    // copy up to 3 chars for node id into id_block[2..4]
    for (uint8_t i = 0; i < 3 && nodeId[i]; ++i) id_block[2 + i] = nodeId[i];
  }
  memcpy(payload + p, id_block, 8);
  p += 8;

  // payload floats: 12 x float32 (voltages)
  for (uint8_t i = 0; i < 12; ++i) {
    float voltage = 0.0f;
    if (i < numValues && rawValues != NULL) {
      voltage = ((float)rawValues[i]) * vref / ((float)adcMax);
    }
    // copy float bytes (native little-endian on ESP32)
    uint8_t fb[4];
    memcpy(fb, &voltage, sizeof(float));
    payload[p++] = fb[0];
    payload[p++] = fb[1];
    payload[p++] = fb[2];
    payload[p++] = fb[3];
  }

  // compute CRC16 over payload (version..floats)
  uint16_t crc = crc16_ccitt(payload, PAYLOAD_SZ);

  // write magic, payload, CRC (CRC little-endian)
  const uint8_t magic[2] = { 0xA5, 0x5A };
  Serial.write(magic, 2);
  Serial.write(payload, PAYLOAD_SZ);
  uint8_t crc_bytes[2];
  crc_bytes[0] = (uint8_t)(crc & 0xFF);
  crc_bytes[1] = (uint8_t)((crc >> 8) & 0xFF);
  Serial.write(crc_bytes, 2);

  // optional: flush to ensure frame sent promptly
  Serial.flush();
}

#endif // BINARYOUTPUT_H