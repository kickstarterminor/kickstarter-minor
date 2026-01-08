#include "TimeManager.h"
#include <WiFi.h>
#include <time.h>
#include "MotorController.h"

namespace TimeManager {

static MotorController* _motor = nullptr;
static String _wifiSsid = "YOUR_SSID";
static String _wifiPass = "YOUR_PASSWORD";
static String _tz = "CET-1CEST,M3.5.0/02:00:00,M10.5.0/03:00:00";
static int _schedHour = 8;
static int _schedMinute = 30;
static uint8_t _stopPin = 26;
static bool _stopActiveLow = true;
static unsigned long _debounceMs = 50;

static bool _timeSynced = false;
static int _lastScheduledStartDay = -1;

static int _lastButtonRead = HIGH;
static unsigned long _lastButtonChangeMs = 0;

void configureWiFi(const char* ssid, const char* password) {
  _wifiSsid = (ssid && ssid[0]) ? String(ssid) : String("");
  _wifiPass = (password && password[0]) ? String(password) : String("");
}

void setTimezone(const char* tzEnvString) {
  if (tzEnvString && tzEnvString[0]) _tz = String(tzEnvString);
}

void setSchedule(int hour, int minute) {
  if (hour >= 0 && hour <= 23) _schedHour = hour;
  if (minute >= 0 && minute <= 59) _schedMinute = minute;
}

void setStopButton(uint8_t pin, bool activeLow) {
  _stopPin = pin;
  _stopActiveLow = activeLow;
}

void setDebounceMs(unsigned long ms) {
  _debounceMs = ms;
}

bool isTimeSynced() {
  return _timeSynced;
}

static void attemptNtpSync() {
  setenv("TZ", _tz.c_str(), 1);
  tzset();
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.println("TimeManager: waiting for NTP time...");
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 5000)) {
    Serial.printf("TimeManager: Time synced: %04d-%02d-%02d %02d:%02d:%02d\n",
                  timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                  timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    _timeSynced = true;
    return;
  }

  // fallback: poll time()
  time_t now = time(nullptr);
  int attempts = 0;
  while (now < 100000 && attempts < 20) {
    delay(500);
    now = time(nullptr);
    attempts++;
  }
  if (now < 100000) {
    Serial.println("TimeManager: NTP not available (will not auto-start motors).");
    _timeSynced = false;
  } else {
    struct tm info;
    localtime_r(&now, &info);
    Serial.printf("TimeManager: Time fallback: %04d-%02d-%02d %02d:%02d:%02d\n",
                  info.tm_year + 1900, info.tm_mon + 1, info.tm_mday,
                  info.tm_hour, info.tm_min, info.tm_sec);
    _timeSynced = true;
  }
}

void begin(MotorController* motor) {
  _motor = motor;

  // configure stop button pin
  if (_stopActiveLow) {
    pinMode(_stopPin, INPUT_PULLUP);
  } else {
    pinMode(_stopPin, INPUT_PULLDOWN);
  }
  _lastButtonRead = digitalRead(_stopPin);

  Serial.println("TimeManager: begin()");

  // Try to connect to WiFi if SSID non-empty
  if (_wifiSsid.length() > 0) {
    Serial.print("TimeManager: connecting to WiFi '");
    Serial.print(_wifiSsid);
    Serial.println("' ...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(_wifiSsid.c_str(), _wifiPass.c_str());

    int tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries < 40) {
      delay(500);
      Serial.print('.');
      tries++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("TimeManager: WiFi connected, IP: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("TimeManager: WiFi connect failed or timed out.");
    }
  } else {
    Serial.println("TimeManager: WiFi SSID empty; skipping WiFi connection.");
  }

  // Attempt NTP sync
  attemptNtpSync();
}

void loop() {
  // Scheduled start (once per day)
  if (_timeSynced && _motor != nullptr) {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 1000)) {
      if (timeinfo.tm_yday != _lastScheduledStartDay) {
        if (timeinfo.tm_hour > _schedHour ||
            (timeinfo.tm_hour == _schedHour && timeinfo.tm_min >= _schedMinute)) {
          if (!_motor->isOn()) {
            _motor->on();
            Serial.println("TimeManager: MOTOR ON (scheduled)");
          }
          _lastScheduledStartDay = timeinfo.tm_yday;
        }
      }
    }
  }

  // Stop-button handling (debounced)
  int buttonRead = digitalRead(_stopPin);
  if (buttonRead != _lastButtonRead) {
    _lastButtonChangeMs = millis();
    _lastButtonRead = buttonRead;
  } else if ((millis() - _lastButtonChangeMs) > _debounceMs) {
    bool pressed = _stopActiveLow ? (buttonRead == LOW) : (buttonRead == HIGH);
    if (pressed && _motor != nullptr && _motor->isOn()) {
      _motor->off();
      Serial.println("TimeManager: MOTOR OFF (button pressed)");
      // prevent re-start today
      struct tm tinfo;
      if (getLocalTime(&tinfo, 1000)) {
        _lastScheduledStartDay = tinfo.tm_yday;
      } else {
        time_t tnow = time(nullptr);
        struct tm* pt = localtime(&tnow);
        if (pt) _lastScheduledStartDay = pt->tm_yday;
      }
    }
  }
}

} // namespace TimeManager