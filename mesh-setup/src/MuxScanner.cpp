#include "MuxScanner.h"

MuxScanner::MuxScanner(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3, int enPin, uint8_t adcPin, uint8_t samples)
  : _s0(s0), _s1(s1), _s2(s2), _s3(s3), _enPin(enPin), _adcPin(adcPin), _samples(samples)
{
  if (_samples == 0) _samples = 1;
}

void MuxScanner::begin() {
  pinMode(_s0, OUTPUT);
  pinMode(_s1, OUTPUT);
  pinMode(_s2, OUTPUT);
  pinMode(_s3, OUTPUT);
  if (_enPin >= 0) {
    pinMode(_enPin, OUTPUT);
    digitalWrite(_enPin, HIGH); // disable while idle (EN is active LOW)
  }
  writeSelectPins(0);
}

void MuxScanner::enable() {
  if (_enPin >= 0) digitalWrite(_enPin, LOW);
}

void MuxScanner::disable() {
  if (_enPin >= 0) digitalWrite(_enPin, HIGH);
}

void MuxScanner::setSamples(uint8_t samples) {
  _samples = (samples == 0) ? 1 : samples;
}

uint8_t MuxScanner::getSamples() const {
  return _samples;
}

void MuxScanner::writeSelectPins(uint8_t ch) {
  digitalWrite(_s0, (ch >> 0) & 0x1);
  digitalWrite(_s1, (ch >> 1) & 0x1);
  digitalWrite(_s2, (ch >> 2) & 0x1);
  digitalWrite(_s3, (ch >> 3) & 0x1);
}

void MuxScanner::setChannel(uint8_t ch) {
  ch &= 0x0F;
  writeSelectPins(ch);
}

int MuxScanner::readChannel(uint8_t ch) {
  ch &= 0x0F;
  writeSelectPins(ch);

  if (_enPin >= 0) digitalWrite(_enPin, LOW);
  delayMicroseconds(50); // allow MUX output to settle

  uint32_t sum = 0;
  for (uint8_t i = 0; i < _samples; ++i) {
    sum += analogRead((int)_adcPin);
    delayMicroseconds(20);
  }

  if (_enPin >= 0) digitalWrite(_enPin, HIGH);
  return (int)(sum / _samples);
}

int MuxScanner::readCurrentChannel() {
  if (_enPin >= 0) digitalWrite(_enPin, LOW);
  delayMicroseconds(50);

  uint32_t sum = 0;
  for (uint8_t i = 0; i < _samples; ++i) {
    sum += analogRead((int)_adcPin);
    delayMicroseconds(20);
  }

  if (_enPin >= 0) digitalWrite(_enPin, HIGH);
  return (int)(sum / _samples);
}