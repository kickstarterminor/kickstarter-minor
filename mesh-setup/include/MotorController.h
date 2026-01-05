#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>

/*
  MotorController — only on/off control through a single MOSFET
  using ESP32 LEDC PWM (low-side switching). Intended for a serial bank of
  vibrating motors that share VCC/GND and have their negatives tied to a MOSFET.
*/

class MotorController {
public:
  MotorController(uint8_t pwmPin, uint8_t pwmChannel = 0, uint32_t freqHz = 5000,
                  uint8_t resolutionBits = 8, bool activeHigh = true);
  ~MotorController();

  // must be called once from setup()
  void begin();

  // optional: call from loop() (no-op for now)
  void update();

  // simple on/off API
  void on();
  void off();
  bool isOn() const;

private:
  uint8_t _pin;
  uint8_t _channel;
  uint32_t _freq;
  uint8_t  _resolution;
  uint32_t _maxDuty;
  bool     _activeHigh;
  bool     _state; // true = on

  inline uint32_t onDuty() const;
  inline void applyDuty(uint32_t duty);
};

inline MotorController::MotorController(uint8_t pwmPin, uint8_t pwmChannel, uint32_t freqHz,
                                        uint8_t resolutionBits, bool activeHigh)
  : _pin(pwmPin), _channel(pwmChannel), _freq(freqHz), _resolution(resolutionBits),
    _maxDuty((resolutionBits >= 32) ? 0xFFFFFFFFu : ((1u << resolutionBits) - 1u)),
    _activeHigh(activeHigh), _state(false)
{}

inline MotorController::~MotorController() {}

inline void MotorController::begin() {
  // configure LEDC (ESP32 PWM)
  ledcSetup(_channel, _freq, _resolution);
  ledcAttachPin(_pin, _channel);
  // ensure off initially
  applyDuty(0);
  _state = false;
}

inline void MotorController::update() {
  // no-op for now; API kept for compatibility
  (void)0;
}

inline uint32_t MotorController::onDuty() const {
  return _maxDuty;
}

inline void MotorController::applyDuty(uint32_t duty) {
  if (_maxDuty == 0) return;
  if (duty > _maxDuty) duty = _maxDuty;
  uint32_t hwDuty = duty;
  if (!_activeHigh) hwDuty = _maxDuty - duty;
  ledcWrite(_channel, hwDuty);
}

inline void MotorController::on() {
  _state = true;
  applyDuty(onDuty());
}

inline void MotorController::off() {
  _state = false;
  applyDuty(0);
}

inline bool MotorController::isOn() const {
  return _state;
}

#endif // MOTORCONTROLLER_H