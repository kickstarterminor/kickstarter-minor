#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <Arduino.h>

class MotorController;

namespace TimeManager {

  // Initialize TimeManager and attempt NTP sync.
  // Pass a pointer to the MotorController instance so TimeManager can start/stop it.
  void begin(MotorController* motor);

  // Must be called regularly from main loop() to handle scheduled starts and button.
  void loop();

  // Optional configuration helpers (can be called before begin()).
  void configureWiFi(const char* ssid, const char* password);
  void setTimezone(const char* tzEnvString); // e.g. "CET-1CEST,M3.5.0/02:00:00,M10.5.0/03:00:00"
  void setSchedule(int hour, int minute);    // local time to auto-start (0-23, 0-59)
  void setStopButton(uint8_t pin, bool activeLow); // configure button pin & polarity
  void setDebounceMs(unsigned long ms);

  // Query status
  bool isTimeSynced();

} // namespace TimeManager

#endif