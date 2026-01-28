#include "AlarmUI.h"
#include "AlarmUI_internal.h"
#include <string.h>

// Clock / pending-clock handling moved out of the main file for readability.
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
    return;
  }

  if (!clockStr || clockStr[0] == '\0') {
    _clock[0] = '\0';
  } else {
    strncpy(_clock, clockStr, sizeof(_clock)-1);
    _clock[sizeof(_clock)-1] = '\0';
  }
  updateDisplay();
}

void AlarmUI::showRealtime(bool en) {
  if (en && _pendingClockSet) {
    strncpy(_clock, _pendingClock, sizeof(_pendingClock)-1);
    _clock[sizeof(_clock)-1] = '\0';
    _pendingClockSet = false;
  }
  s_showRealtime = en;
  updateDisplay();
}

