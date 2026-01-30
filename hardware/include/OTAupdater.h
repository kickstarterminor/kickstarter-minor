#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

// Define your firmware version here - increment this with each release
#define FIRMWARE_VERSION "1.0.0"

class OTAUpdater {
public:
    OTAUpdater(const char* serverUrl, const char* binPath, const char* versionPath);
    
    // Call this in setup() to check for updates
    void begin();
    
    // Check if update is available and perform it
    bool checkAndUpdate();
    
    // Get current firmware version
    const char* getCurrentVersion() const { return FIRMWARE_VERSION; }
    
private:
    const char* _serverUrl;      // e.g., "http://192.168.1.100:3000"
    const char* _binPath;        // e.g., "/firmware.bin"
    const char* _versionPath;    // e.g., "/version.txt"
    
    // Check server for latest version
    String getServerVersion();
    
    // Compare version strings (returns true if server version is newer)
    bool isNewerVersion(const String& serverVersion);
    
    // Perform the actual OTA update
    bool performUpdate();
    
    // Parse version string to comparable format
    uint32_t parseVersion(const String& version);
};

#endif // OTA_UPDATER_H
