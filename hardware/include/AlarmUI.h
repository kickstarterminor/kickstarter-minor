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

  // set a pre-formatted clock string (HH:MM:SS). Passing nullptr or empty
  // string clears the clock display.
  void setClock(const char* clockStr);

  // show realtime instead of alarm on the main display
  void showRealtime(bool en);

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
  // press timing and long-press state
  unsigned long _pressStart[4];
  bool _longPressEvent[4];

  // blink for the current field
  unsigned long _lastBlink;
  bool _blinkOn;

  // simple ASCII clock buffer ("HH:MM:SS" + null)
  char _clock[9];
  // pending clock stored while editing so realtime display doesn't update
  char _pendingClock[9];
  bool _pendingClockSet;

  void readButtons();
  bool consumePressed(uint8_t idx);
  bool consumeLongPressed(uint8_t idx);
  void updateDisplay();
};

#endif

