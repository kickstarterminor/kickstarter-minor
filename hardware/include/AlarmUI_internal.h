// Internal definitions for AlarmUI split implementation
#ifndef ALARMUI_INTERNAL_H
#define ALARMUI_INTERNAL_H

#include <stdint.h>

// Shared display/timing constants used across AlarmUI translation units.
static constexpr uint8_t S_LCD_COLS = 16;
static constexpr unsigned long DEBOUNCE_MS = 30UL;
static constexpr unsigned long BLINK_MS = 500UL;

// Global toggle shared across AlarmUI files (was previously file-local).
extern bool s_showRealtime;

// Expose constants to other translation units
constexpr uint8_t ALARMUI_LCD_COLS = S_LCD_COLS;
constexpr unsigned long ALARMUI_DEBOUNCE_MS = DEBOUNCE_MS;
constexpr unsigned long ALARMUI_BLINK_MS = BLINK_MS;

#endif // ALARMUI_INTERNAL_H

