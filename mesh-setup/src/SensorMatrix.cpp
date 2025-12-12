#include "SensorMatrix.h"

SensorMatrix::SensorMatrix(const int rows[], size_t numRows, uint8_t maxChannels)
  : _rows(nullptr), _numRows(numRows), _maxChannels(maxChannels),
    _totalSensors(0), _sensorRow(nullptr), _sensorCol(nullptr)
{
  if (_numRows == 0) {
    _rows = nullptr;
  } else {
    _rows = (int*)malloc(sizeof(int) * _numRows);
    if (_rows == nullptr) {
      Serial.println("SensorMatrix: memory allocation failed for rows");
      _numRows = 0;
    } else {
      for (size_t i = 0; i < _numRows; ++i) {
        _rows[i] = rows[i];
      }
    }
  }

  // allocate mapping arrays sized to maxChannels
  _sensorRow = (int*)malloc(sizeof(int) * _maxChannels);
  _sensorCol = (int*)malloc(sizeof(int) * _maxChannels);
  if (_sensorRow == nullptr || _sensorCol == nullptr) {
    Serial.println("SensorMatrix: memory allocation failed for mapping arrays");
    if (_sensorRow) free(_sensorRow);
    if (_sensorCol) free(_sensorCol);
    _sensorRow = _sensorCol = nullptr;
    _maxChannels = 0;
  }

  build();
}

SensorMatrix::~SensorMatrix() {
  if (_rows) free(_rows);
  if (_sensorRow) free(_sensorRow);
  if (_sensorCol) free(_sensorCol);
}

void SensorMatrix::build() {
  _totalSensors = 0;
  if (_rows == nullptr || _sensorRow == nullptr || _sensorCol == nullptr) return;

  for (size_t r = 0; r < _numRows; ++r) {
    int count = _rows[r];
    if (count <= 0) continue;
    for (int c = 0; c < count; ++c) {
      if (_totalSensors >= _maxChannels) {
        Serial.print("SensorMatrix: reached maxChannels=");
        Serial.println(_maxChannels);
        return;
      }
      _sensorRow[_totalSensors] = (int)r;
      _sensorCol[_totalSensors] = c;
      _totalSensors++;
    }
  }
}

uint8_t SensorMatrix::totalSensors() const {
  return _totalSensors;
}

size_t SensorMatrix::numRows() const {
  return _numRows;
}

int SensorMatrix::rowCount(size_t rowIndex) const {
  if (_rows == nullptr) return 0;
  if (rowIndex >= _numRows) return 0;
  return _rows[rowIndex];
}

int SensorMatrix::rowOf(uint8_t sensorId) const {
  if (!validId(sensorId)) return -1;
  return _sensorRow[sensorId];
}

int SensorMatrix::colOf(uint8_t sensorId) const {
  if (!validId(sensorId)) return -1;
  return _sensorCol[sensorId];
}

bool SensorMatrix::validId(uint8_t sensorId) const {
  return sensorId < _totalSensors;
}

void SensorMatrix::printLayout() const {
  Serial.print("SensorMatrix rows: ");
  for (size_t r = 0; r < _numRows; ++r) {
    if (r) Serial.print(", ");
    Serial.print(_rows ? _rows[r] : 0);
  }
  Serial.println();

  Serial.print("Total sensors: ");
  Serial.println(_totalSensors);

  Serial.println("Mapping (id -> row,col):");
  for (uint8_t i = 0; i < _totalSensors; ++i) {
    Serial.print("  ");
    Serial.print(i);
    Serial.print(" -> (");
    Serial.print(_sensorRow[i]);
    Serial.print(",");
    Serial.print(_sensorCol[i]);
    Serial.println(")");
  }
}