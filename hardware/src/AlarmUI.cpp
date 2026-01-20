#include "AlarmUI.h"

// debounce and long-press constants
static const unsigned long DEBOUNCE_MS = 30;
static const unsigned long BLINK_MS = 500;

AlarmUI::AlarmUI(LiquidCrystal_I2C& lcd, uint8_t btnConfirm, uint8_t btnCancel, uint8_t btnUp, uint8_t btnDown)
  : _lcd(&lcd), _alarmHour(7), _alarmMinute(0), _enabled(false), _state(0), _prevHour(7), _prevMinute(0), _lastBlink(0), _blinkOn(true)
{
  _btn[0] = btnConfirm;
  _btn[1] = btnCancel;
  _btn[2] = btnUp;
  _btn[3] = btnDown;
  for (int i = 0; i < 4; ++i) {
    _lastDebounce[i] = 0;
    _lastReading[i] = HIGH;
    _lastState[i] = HIGH;
    _pressedEvent[i] = false;
  }
}

void AlarmUI::begin() {
  for (int i = 0; i < 4; ++i) {
    pinMode(_btn[i], INPUT_PULLUP);
  }
  _lcd->init();
  _lcd->backlight();
  updateDisplay();
}

void AlarmUI::loop() {
  readButtons();

  // handle inputs
  if (_state == 0) {
    // idle: confirm toggles enabled, wrong enters setting
    if (consumePressed(0)) { // confirm
      _enabled = !_enabled;
      updateDisplay();
    }
    if (consumePressed(1)) { // wrong -> enter hour set
      _state = 1;
      _prevHour = _alarmHour;
      _prevMinute = _alarmMinute;
      _lastBlink = millis();
      _blinkOn = true;
      updateDisplay();
    }
  } else if (_state == 1) {
    // setting hour
    if (consumePressed(2)) { // up
      _alarmHour = (_alarmHour + 1) % 24;
      updateDisplay();
    }
    if (consumePressed(3)) { // down
      _alarmHour = (_alarmHour + 23) % 24;
      updateDisplay();
    }
    if (consumePressed(0)) { // confirm -> go to minute
      _state = 2;
      _lastBlink = millis();
      _blinkOn = true;
      updateDisplay();
    }
    if (consumePressed(1)) { // wrong -> cancel
      _alarmHour = _prevHour;
      _alarmMinute = _prevMinute;
      _state = 0;
      updateDisplay();
    }
  } else if (_state == 2) {
    // setting minute
    if (consumePressed(2)) { // up
      _alarmMinute = (_alarmMinute + 1) % 60;
      updateDisplay();
    }
    if (consumePressed(3)) { // down
      _alarmMinute = (_alarmMinute + 59) % 60;
      updateDisplay();
    }
    if (consumePressed(0)) { // confirm -> finish
      _state = 0;
      _enabled = true;
      updateDisplay();
    }
    if (consumePressed(1)) { // wrong -> cancel
      _alarmHour = _prevHour;
      _alarmMinute = _prevMinute;
      _state = 0;
      updateDisplay();
    }
  }

  // blink logic
  unsigned long now = millis();
  if (now - _lastBlink >= BLINK_MS) {
    _lastBlink = now;
    _blinkOn = !_blinkOn;
    updateDisplay();
  }
}

uint8_t AlarmUI::alarmHour() const { return _alarmHour; }
uint8_t AlarmUI::alarmMinute() const { return _alarmMinute; }
bool AlarmUI::isAlarmEnabled() const { return _enabled; }

void AlarmUI::readButtons() {
  for (uint8_t i = 0; i < 4; ++i) {
    bool reading = digitalRead(_btn[i]);
    if (reading != _lastReading[i]) {
      _lastDebounce[i] = millis();
    }
    if ((millis() - _lastDebounce[i]) > DEBOUNCE_MS) {
      if (reading != _lastState[i]) {
        _lastState[i] = reading;
        if (reading == LOW) {
          _pressedEvent[i] = true;
        }
      }
    }
    _lastReading[i] = reading;
  }
}

bool AlarmUI::consumePressed(uint8_t idx) {
  if (idx >= 4) return false;
  if (_pressedEvent[idx]) {
    _pressedEvent[idx] = false;
    return true;
  }
  return false;
}

// Write a zero-padded two-digit decimal string into the caller buffer.
// buf must be at least 3 bytes long (two digits + null terminator).
static void two(uint8_t v, char* buf) {
  buf[0] = '0' + (uint8_t)((v / 10) % 10);
  buf[1] = '0' + (uint8_t)(v % 10);
  buf[2] = '\0';
}

void AlarmUI::updateDisplay() {
  // display: line1 show Alarm HH:MM [ON/OFF], line2 show hints
  _lcd->clear();
  _lcd->setCursor(0, 0);
  _lcd->print("Alarm ");
  char buf[3];
  two(_alarmHour, buf);
  _lcd->print(buf);
  _lcd->print(":");
  two(_alarmMinute, buf);
  _lcd->print(buf);
  _lcd->print(" ");
  _lcd->print(_enabled ? "ON" : "OFF");

  _lcd->setCursor(0, 1);
  if (_state == 0) {
    _lcd->print("[Ok]Toggle [Wr]Set");
  } else if (_state == 1) {
    // blink hour
    if (_blinkOn) {
      _lcd->print("Set Hour: ");
      two(_alarmHour, buf);
      _lcd->print(buf);
      _lcd->print(":");
      two(_alarmMinute, buf);
      _lcd->print(buf);
    } else {
      _lcd->print("Set Hour:   ");
    }
  } else if (_state == 2) {
    if (_blinkOn) {
      _lcd->print("Set Min : ");
      two(_alarmHour, buf);
      _lcd->print(buf);
      _lcd->print(":");
      two(_alarmMinute, buf);
      _lcd->print(buf);
    } else {
      _lcd->print("Set Min :   ");
    }
  }
}

