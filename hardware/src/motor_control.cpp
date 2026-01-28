#include "motor_control.h"

MotorControl::MotorControl(uint8_t motorPin) {
  pin = motorPin;
  pulseEnabled = false;
  lastPulseTime = 0;
  pulseState = false;
  pulseOnTime = 200;
  pulseOffTime = 800;
}

void MotorControl::begin() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void MotorControl::setPulse(bool enable, uint16_t onTime, uint16_t offTime) {
  pulseEnabled = enable;
  pulseOnTime = onTime;
  pulseOffTime = offTime;
  
  if (!enable) {
    digitalWrite(pin, LOW);
    pulseState = false;
  }
}

void MotorControl::setOn(bool state) {
  pulseEnabled = false;
  digitalWrite(pin, state);
}

void MotorControl::update() {
  if (!pulseEnabled) return;
  
  unsigned long currentTime = millis();
  uint16_t interval = pulseState ? pulseOnTime : pulseOffTime;
  
  if (currentTime - lastPulseTime >= interval) {
    lastPulseTime = currentTime;
    pulseState = !pulseState;
    digitalWrite(pin, pulseState);
  }
}
