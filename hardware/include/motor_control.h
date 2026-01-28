#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

class MotorControl {
private:
  uint8_t pin;
  bool pulseEnabled;
  unsigned long lastPulseTime;
  bool pulseState;
  uint16_t pulseOnTime;   // ms motor aan
  uint16_t pulseOffTime;  // ms motor uit

public:
  MotorControl(uint8_t motorPin);
  void begin();
  void setPulse(bool enable, uint16_t onTime = 200, uint16_t offTime = 800);
  void setOn();
  void setOff();
  void update();  // Call in loop()
};

#endif
