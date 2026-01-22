#include "AlarmUI.h"
#include <Wire.h>
#include <string.h>

// LCD width for centering text (most common modules are 16x2)
static const uint8_t S_LCD_COLS = 16;
// File-local display mode: true => show realtime, false => show alarm
static bool s_showRealtime = false;

// debounce and long-press constants
static const unsigned long DEBOUNCE_MS = 30;
static const unsigned long BLINK_MS = 500;

AlarmUI::AlarmUI(LiquidCrystal_I2C& lcd, uint8_t btnConfirm, uint8_t btnCancel, uint8_t btnUp, uint8_t btnDown)
  : _lcd(&lcd), _alarmHour(7), _alarmMinute(0), _enabled(false), _state(0), _prevHour(7), _prevMinute(0), _lastBlink(0), _blinkOn(true), _pendingClockSet(false)
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
  _clock[0] = '\0';
  // ensure pending clock buffer is initialized as well
  _pendingClock[0] = '\0';
  // default view is alarm; realtime can be toggled by the Up button
  s_showRealtime = false;
  for (int i = 0; i < 4; ++i) {
    _pressStart[i] = 0UL;
    _longPressEvent[i] = false;
  }
}

void AlarmUI::begin() {
  for (int i = 0; i < 4; ++i) {
    pinMode(_btn[i], INPUT_PULLUP);
  }
  Serial.print("AlarmUI begin pins: ");
  Serial.print("confirm="); Serial.print(_btn[0]);
  Serial.print(" cancel="); Serial.print(_btn[1]);
  Serial.print(" up="); Serial.print(_btn[2]);
  Serial.print(" down="); Serial.println(_btn[3]);
  // Basic I2C bus probe to help diagnose missing characters on the LCD.
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

void AlarmUI::loop() {
  readButtons();

  // handle inputs
  if (_state == 0) {
    // idle:
    // - Btn 0 (confirm) toggles alarm enabled/disabled
    // - Btn 1 (wrong) toggles between alarm view and realtime view
    // - Btn 2 (up) when showing realtime => enter alarm hour set
    //            when showing alarm => switch to realtime view
    if (consumePressed(0)) { // confirm
      _enabled = !_enabled;
      updateDisplay();
    }
    if (consumePressed(1)) { // wrong -> enter edit (cancel when editing)
      // When idle, Wrong should start editing (enter hour set). If
      // currently editing, Wrong acts as cancel (handled in edit branches).
      _state = 1;
      _prevHour = _alarmHour;
      _prevMinute = _alarmMinute;
      _lastBlink = millis();
      _blinkOn = true;
      // ensure alarm view is active while editing
      s_showRealtime = false;
      updateDisplay();
    }
    if (consumePressed(2)) { // up
      // Toggle between alarm and realtime view when idle.
      // Use showRealtime() so any pending clock is committed when
      // switching into realtime view.
      showRealtime(!s_showRealtime);
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
      // committing pending clock when finishing hour stage
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
      // discard pending clock when cancelling
      _pendingClockSet = false;
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
      // commit pending clock when finishing edit
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
      Serial.print("btn raw change i="); Serial.print(i);
      Serial.print(" pin="); Serial.print(_btn[i]);
      Serial.print(" reading="); Serial.println(reading ? 1 : 0);
    }
    if ((millis() - _lastDebounce[i]) > DEBOUNCE_MS) {
      if (reading != _lastState[i]) {
        _lastState[i] = reading;
        if (reading == LOW) {
          // low = pressed
          _pressedEvent[i] = true;
          _pressStart[i] = millis();
          _longPressEvent[i] = false;
          Serial.print("btn pressedEvent set i="); Serial.print(i);
          Serial.print(" pin="); Serial.println(_btn[i]);
        } else {
          // released: check for long-press
          unsigned long held = millis() - _pressStart[i];
          if (held >= 1000 && !_longPressEvent[i]) {
            _longPressEvent[i] = true;
          }
        }
      } else {
        // If still pressed, check long-press threshold
        if (reading == LOW && !_longPressEvent[i]) {
          if (millis() - _pressStart[i] >= 1000) {
            _longPressEvent[i] = true;
          }
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
    Serial.print("btn consumed idx="); Serial.print(idx);
    Serial.print(" pin="); Serial.println(_btn[idx]);
    return true;
  }
  return false;
}

bool AlarmUI::consumeLongPressed(uint8_t idx) {
  if (idx >= 4) return false;
  if (_longPressEvent[idx]) {
    _longPressEvent[idx] = false;
    Serial.print("btn long consumed idx="); Serial.print(idx);
    Serial.print(" pin="); Serial.println(_btn[idx]);
    return true;
  }
  return false;
}

void AlarmUI::setClock(const char* clockStr) {
  if (_state == 1 || _state == 2) {
    if (clockStr && clockStr[0] != '\0') {
      strncpy(_pendingClock, clockStr, sizeof(_pendingClock)-1);
      _pendingClock[sizeof(_pendingClock)-1] = '\0';
      _pendingClockSet = true;
    } else {
      _pendingClockSet = false;
    }
    return;
  }

  if (!s_showRealtime) {
    if (clockStr && clockStr[0] != '\0') {
      strncpy(_pendingClock, clockStr, sizeof(_pendingClock)-1);
      _pendingClock[sizeof(_pendingClock)-1] = '\0';
      _pendingClockSet = true;
    } else {
      _pendingClockSet = false;
    }
    // Do not update the visible clock while not in realtime view.
    return;
  }

  if (!clockStr || clockStr[0] == '\0') {
    _clock[0] = '\0';
  } else {
    // copy up to 8 chars (HH:MM:SS) + null
    strncpy(_clock, clockStr, sizeof(_clock)-1);
    _clock[sizeof(_clock)-1] = '\0';
  }
  // update display to show the clock immediately
  updateDisplay();
}

void AlarmUI::showRealtime(bool en) {
  // When enabling realtime view, commit any pending clock (from NTP or
  // stored while in alarm/edit modes) so the display immediately shows
  // the most recent time.
  if (en && _pendingClockSet) {
    strncpy(_clock, _pendingClock, sizeof(_clock)-1);
    _clock[sizeof(_clock)-1] = '\0';
    _pendingClockSet = false;
  }
  s_showRealtime = en;
  updateDisplay();
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
  Serial.print("updateDisplay: state="); Serial.print(_state);
  Serial.print(" alarm=");
  Serial.print(_alarmHour); Serial.print(":"); Serial.print(_alarmMinute);
  Serial.print(" enabled="); Serial.println(_enabled ? "1" : "0");

  // Clear once up front to avoid multiple clears which can produce brief
  // intermediate states where both alarm and realtime appear simultaneously.
  _lcd->clear();
  char buf[3];

  if (_state == 0) {
    // Idle: either show realtime centered (when enabled) or the alarm view.
    if (s_showRealtime && _clock[0]) {
      uint8_t start = (S_LCD_COLS - 8) / 2; // center 8-char HH:MM:SS
      _lcd->setCursor(start, 0);
      _lcd->print(_clock);
      _lcd->setCursor(0, 1);
      _lcd->print("[Ok]Toggle [X]Set");
    } else {
      // Alarm view: top line shows alarm status, bottom line shows hints.
      _lcd->setCursor(0, 0);
      _lcd->print("Alarm ");
      two(_alarmHour, buf);
      _lcd->print(buf);
      _lcd->print(":");
      two(_alarmMinute, buf);
      _lcd->print(buf);
      _lcd->print(" ");
      _lcd->print(_enabled ? "ON" : "OFF");
      _lcd->setCursor(0, 1);
      _lcd->print("[Ok]Toggle [X]Set");
    }
  } else if (_state == 1) {
    // Setting hour: show alarm on the top line, editing hint on the bottom.
    _lcd->setCursor(0, 0);
    _lcd->print("Alarm ");
    two(_alarmHour, buf);
    _lcd->print(buf);
    _lcd->print(":");
    two(_alarmMinute, buf);
    _lcd->print(buf);
    _lcd->print(" ");
    _lcd->print(_enabled ? "ON" : "OFF");

    _lcd->setCursor(0, 1);
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
    // Setting minute: same top-line alarm, bottom shows minute editing.
    _lcd->setCursor(0, 0);
    _lcd->print("Alarm ");
    two(_alarmHour, buf);
    _lcd->print(buf);
    _lcd->print(":");
    two(_alarmMinute, buf);
    _lcd->print(buf);
    _lcd->print(" ");
    _lcd->print(_enabled ? "ON" : "OFF");

    _lcd->setCursor(0, 1);
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


