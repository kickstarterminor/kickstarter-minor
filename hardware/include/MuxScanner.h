#ifndef MUXSCANNER_H
#define MUXSCANNER_H

#include <Arduino.h>

class MuxScanner {
public:
  // s0..s3: select pins for HC4067
  // enPin: optional enable pin (active LOW). Use -1 if not connected.
  // adcPin: analog input pin connected to the MUX common output (SIG)
  // samples: number of ADC samples to average per read
  MuxScanner(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3, int enPin = -1, uint8_t adcPin = 34, uint8_t samples = 8);

  // initialize pins (call in setup)
  void begin();

  // enable/disable MUX (if EN pin provided)
  void enable();
  void disable();

  // change sample count at runtime
  void setSamples(uint8_t samples);
  uint8_t getSamples() const;

  // set the active mux channel (0..15) - does not read ADC
  void setChannel(uint8_t ch);

  // read the specified channel (sets it, waits briefly, averages samples)
  // returns raw ADC (0..4095 when analogReadResolution(12) is used)
  int readChannel(uint8_t ch);

  // convenience: read the currently selected channel without changing selection
  int readCurrentChannel();

  static constexpr uint8_t MAX_CHANNELS = 16;

private:
  uint8_t _s0, _s1, _s2, _s3;
  int _enPin;
  uint8_t _adcPin;
  uint8_t _samples;

  void writeSelectPins(uint8_t ch);
};

#endif