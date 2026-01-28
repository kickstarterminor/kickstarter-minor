#include "AlarmUI.h"
#include "AlarmUI_internal.h"

// Button handling helpers separated for readability
void AlarmUI::readButtons() {
  for (uint8_t i = 0; i < 4; ++i) {
    bool reading = digitalRead(_btn[i]);
    if (reading != _lastReading[i]) {
      _lastDebounce[i] = millis();
      Serial.print("btn raw change i="); Serial.print(i);
      Serial.print(" pin="); Serial.print(_btn[i]);
      Serial.print(" reading="); Serial.println(reading ? 1 : 0);
    }
    if ((millis() - _lastDebounce[i]) > ALARMUI_DEBOUNCE_MS) {
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

