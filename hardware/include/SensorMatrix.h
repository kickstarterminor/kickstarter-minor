#ifndef SENSORMATRIX_H
#define SENSORMATRIX_H

#include <Arduino.h>

class SensorMatrix {
public:
  // rows: pointer to an array of ints describing sensors per row
  // numRows: number of elements in rows[]
  // maxChannels: maximum number of mux channels (default 16 for HC4067)
  SensorMatrix(const int rows[], size_t numRows, uint8_t maxChannels = 16);
  ~SensorMatrix();

  // build the internal mapping (called by constructor)
  void build();

  // query
  uint8_t totalSensors() const;
  size_t numRows() const;
  int rowCount(size_t rowIndex) const;
  int rowOf(uint8_t sensorId) const;
  int colOf(uint8_t sensorId) const;
  bool validId(uint8_t sensorId) const;

  // debug print to Serial
  void printLayout() const;

private:
  int* _rows;
  size_t _numRows;
  uint8_t _maxChannels;

  uint8_t _totalSensors;
  int* _sensorRow;       // indexed by sensor id -> row
  int* _sensorCol;       // indexed by sensor id -> column
};

#endif