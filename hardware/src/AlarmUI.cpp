#include "AlarmUI.h"
#include "AlarmUI_internal.h"
#include <Wire.h>

bool s_showRealtime = false;

AlarmUI::AlarmUI(LiquidCrystal_I2C& lcd, uint8_t btnConfirm, uint8_t btnCancel, uint8_t btnUp, uint8_t btnDown)
  : _lcd(&lcd), _alarmHour(7), _alarmMinute(0), _enabled(false), _state(0), _prevHour(7), _prevMinute(0), _lastBlink(0), _blinkOn(true), _pendingClockSet(false)
{
  _btn[0] = btnConfirm;
  _btn[1] = btnCancel;
  _btn[2] = btnUp;
  _btn[3] = btnDown;
  for (int i = 0; i < 4; ++i) {
    _lastDebounce[i] = 0;
    // Switch to pulldown: released = LOW, pressed = HIGH (connect button to 3V3)
    _lastReading[i] = LOW;  // [web:5]
    _lastState[i] = LOW;
    _pressedEvent[i] = false;
  }
  _clock[0] = '\0';
  _pendingClock[0] = '\0';
  s_showRealtime = false;
  for (int i = 0; i < 4; ++i) {
    _pressStart[i] = 0UL;
    _longPressEvent[i] = false;
  }
}

void AlarmUI::begin() {
  // Switch to INPUT_PULLDOWN for all buttons (ESP32-S3 supports ~45kΩ internal pulldown) [web:5][web:11][web:20]
  for (int i = 0; i < 4; ++i) {
    pinMode(_btn[i], INPUT_PULLDOWN);  // [web:5]
  }
  Serial.print("AlarmUI begin pins (PULLDOWN): ");
  Serial.print("confirm="); Serial.print(_btn[0]);
  Serial.print(" cancel="); Serial.print(_btn[1]);
  Serial.print(" up="); Serial.print(_btn[2]);
  Serial.print(" down="); Serial.println(_btn[3]);
  // Rest unchanged...
  Serial.println("AlarmUI: scanning I2C bus for devices...");
  uint8_t devCount = 0;
  for (uint8_t addr = 1; addr < 127; ++addr) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("I2C device found at 0x");
      if (addr < 16) Serial.print('0');
      Serial.println(addr, HEX);
      ++devCount;
    }
  }
  if (devCount == 0) Serial.println("No I2C devices found on bus.");
  else {
    Serial.print(devCount); Serial.println(" I2C devices found.");
  }
  _lcd->init();
  _lcd->backlight();
  _lcd->clear();
  _lcd->setBacklight(LOW);
  _lcd->backlight();
  updateDisplay();
}

// readButtons() unchanged, as it reads raw digitalRead (works for both pullup/pulldown)
// but now pressed == HIGH (vs LOW in pullup)
void AlarmUI::loop() {
  readButtons();

  // handle inputs (state machine) - unchanged, since consumePressed detects LOW->HIGH edge
  // (now HIGH on press against pulldown, but logic is edge-based, not level-based)
  if (_state == 0) {
    if (consumePressed(0)) { // confirm
      _enabled = !_enabled;
      updateDisplay();
    }
    if (consumePressed(1)) { // wrong -> enter edit
      _state = 1;
      _prevHour = _alarmHour;
      _prevMinute = _alarmMinute;
      _lastBlink = millis();
      _blinkOn = true;
      s_showRealtime = false;
      updateDisplay();
    }
    if (consumePressed(2)) { // up -> toggle realtime view
      showRealtime(!s_showRealtime);
    }
  } else if (_state == 1) {
    // setting hour
    if (consumePressed(2)) { _alarmHour = (_alarmHour + 1) % 24; updateDisplay(); }
    if (consumePressed(3)) { _alarmHour = (_alarmHour + 23) % 24; updateDisplay(); }
    if (consumePressed(0)) { // confirm -> go to minute
      _state = 2;
      _lastBlink = millis();
      _blinkOn = true;
      if (_pendingClockSet) {
        strncpy(_clock, _pendingClock, sizeof(_clock)-1);
        _clock[sizeof(_clock)-1] = '\0';
        _pendingClockSet = false;
      }
      updateDisplay();
    }
    if (consumePressed(1)) { // wrong -> cancel
      _alarmHour = _prevHour;
      _alarmMinute = _prevMinute;
      _state = 0;
      _pendingClockSet = false;
      updateDisplay();
    }
  } else if (_state == 2) {
    // setting minute
    if (consumePressed(2)) { _alarmMinute = (_alarmMinute + 1) % 60; updateDisplay(); }
    if (consumePressed(3)) { _alarmMinute = (_alarmMinute + 59) % 60; updateDisplay(); }
    if (consumePressed(0)) { // confirm -> finish
      _state = 0;
      _enabled = true;
      if (_pendingClockSet) {
        strncpy(_clock, _pendingClock, sizeof(_pendingClock)-1);
        _clock[sizeof(_clock)-1] = '\0';
        _pendingClockSet = false;
      }
      updateDisplay();
    }
    if (consumePressed(1)) { // wrong -> cancel
      _alarmHour = _prevHour;
      _alarmMinute = _prevMinute;
      _state = 0;
      _pendingClockSet = false;
      updateDisplay();
    }
  }

  // blink logic
  unsigned long now = millis();
  if (now - _lastBlink >= ALARMUI_BLINK_MS) {
    _lastBlink = now;
    _blinkOn = !_blinkOn;
    updateDisplay();
  }
}

uint8_t AlarmUI::alarmHour() const { return _alarmHour; }
uint8_t AlarmUI::alarmMinute() const { return _alarmMinute; }
bool AlarmUI::isAlarmEnabled() const { return _enabled; }
