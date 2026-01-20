#ifndef ALARMUI_H
#define ALARMUI_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class AlarmUI {
public:
  // lcd reference and pins: confirm, cancel, up, down (active LOW)
  AlarmUI(LiquidCrystal_I2C& lcd, uint8_t btnConfirm, uint8_t btnCancel, uint8_t btnUp, uint8_t btnDown);

  // call from setup()
  void begin();

  // call frequently from loop() - non-blocking
  void loop();

  // accessors
  uint8_t alarmHour() const;
  uint8_t alarmMinute() const;
  bool isAlarmEnabled() const;

private:
  LiquidCrystal_I2C* _lcd;
  uint8_t _btn[4];

  uint8_t _alarmHour;
  uint8_t _alarmMinute;
  bool _enabled;

  // 0 = idle, 1 = setting hour, 2 = setting minute
  uint8_t _state;
  uint8_t _prevHour;
  uint8_t _prevMinute;

  // debounce / input state
  unsigned long _lastDebounce[4];
  bool _lastReading[4];
  bool _lastState[4];
  bool _pressedEvent[4];

  // blink for the current field
  unsigned long _lastBlink;
  bool _blinkOn;

  void readButtons();
  bool consumePressed(uint8_t idx);
  void updateDisplay();
};

#endif

