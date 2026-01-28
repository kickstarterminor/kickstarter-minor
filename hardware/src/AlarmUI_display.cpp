#include "AlarmUI.h"
#include "AlarmUI_internal.h"

// Small helper to write zero-padded two-digit numbers.
static void two_local(uint8_t v, char* buf) {
  buf[0] = '0' + (uint8_t)((v / 10) % 10);
  buf[1] = '0' + (uint8_t)(v % 10);
  buf[2] = '\0';
}

void AlarmUI::updateDisplay() {
  Serial.print("updateDisplay: state="); Serial.print(_state);
  Serial.print(" alarm=");
  Serial.print(_alarmHour); Serial.print(":"); Serial.print(_alarmMinute);
  Serial.print(" enabled="); Serial.println(_enabled ? "1" : "0");

  _lcd->clear();
  char buf[3];

  if (_state == 0) {
    if (s_showRealtime && _clock[0]) {
      uint8_t start = (ALARMUI_LCD_COLS - 8) / 2;
      _lcd->setCursor(start, 0);
      _lcd->print(_clock);
      _lcd->setCursor(0, 1);
      _lcd->print("[Ok]Toggle [X]St");
    } else {
      _lcd->setCursor(0, 0);
      _lcd->print("Alarm ");
      two_local(_alarmHour, buf);
      _lcd->print(buf);
      _lcd->print(":");
      two_local(_alarmMinute, buf);
      _lcd->print(buf);
      _lcd->print(" ");
      _lcd->print(_enabled ? "ON" : "OFF");
      _lcd->setCursor(0, 1);
      _lcd->print("[Ok]Toggle [X]St");
    }
  } else if (_state == 1) {
    _lcd->setCursor(0, 0);
    _lcd->print("Alarm ");
    two_local(_alarmHour, buf);
    _lcd->print(buf);
    _lcd->print(":");
    two_local(_alarmMinute, buf);
    _lcd->print(buf);
    _lcd->print(" ");
    _lcd->print(_enabled ? "ON" : "OFF");

    _lcd->setCursor(0, 1);
    if (_blinkOn) {
      _lcd->print("Set Hour: ");
      two_local(_alarmHour, buf);
      _lcd->print(buf);
      _lcd->print(":");
      two_local(_alarmMinute, buf);
      _lcd->print(buf);
    } else {
      _lcd->print("Set Hour:   ");
    }
  } else if (_state == 2) {
    _lcd->setCursor(0, 0);
    _lcd->print("Alarm ");
    two_local(_alarmHour, buf);
    _lcd->print(buf);
    _lcd->print(":");
    two_local(_alarmMinute, buf);
    _lcd->print(buf);
    _lcd->print(" ");
    _lcd->print(_enabled ? "ON" : "OFF");

    _lcd->setCursor(0, 1);
    if (_blinkOn) {
      _lcd->print("Set Min : ");
      two_local(_alarmHour, buf);
      _lcd->print(buf);
      _lcd->print(":");
      two_local(_alarmMinute, buf);
      _lcd->print(buf);
    } else {
      _lcd->print("Set Min :   ");
    }
  }
}

