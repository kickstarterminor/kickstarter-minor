#include "OTAUpdater.h"

OTAUpdater::OTAUpdater(const char* serverUrl, const char* binPath, const char* versionPath)
    : _serverUrl(serverUrl), _binPath(binPath), _versionPath(versionPath) {
}

void OTAUpdater::begin() {
    Serial.println("=== OTA Updater Initialized ===");
    Serial.print("Current firmware version: ");
    Serial.println(FIRMWARE_VERSION);
    Serial.print("Update server: ");
    Serial.println(_serverUrl);
}

String OTAUpdater::getServerVersion() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected - skipping version check");
        return "";
    }
    
    HTTPClient http;
    String versionUrl = String(_serverUrl) + String(_versionPath);
    
    Serial.print("Checking version at: ");
    Serial.println(versionUrl);
    
    http.begin(versionUrl);
    http.setTimeout(5000); // 5 second timeout
    
    int httpCode = http.GET();
    String serverVersion = "";
    
    if (httpCode == HTTP_CODE_OK) {
        serverVersion = http.getString();
        serverVersion.trim(); // Remove whitespace/newlines
        Serial.print("Server version: ");
        Serial.println(serverVersion);
    } else {
        Serial.print("Version check failed, HTTP code: ");
        Serial.println(httpCode);
    }
    
    http.end();
    return serverVersion;
}

uint32_t OTAUpdater::parseVersion(const String& version) {
    // Parse "X.Y.Z" into comparable uint32: (X << 16) | (Y << 8) | Z
    // Example: "1.2.3" -> 0x00010203
    
    int major = 0, minor = 0, patch = 0;
    int firstDot = version.indexOf('.');
    int secondDot = version.indexOf('.', firstDot + 1);
    
    if (firstDot > 0) {
        major = version.substring(0, firstDot).toInt();
        if (secondDot > firstDot) {
            minor = version.substring(firstDot + 1, secondDot).toInt();
            patch = version.substring(secondDot + 1).toInt();
        } else {
            minor = version.substring(firstDot + 1).toInt();
        }
    }
    
    // Clamp to 8 bits each
    major = constrain(major, 0, 255);
    minor = constrain(minor, 0, 255);
    patch = constrain(patch, 0, 255);
    
    return ((uint32_t)major << 16) | ((uint32_t)minor << 8) | (uint32_t)patch;
}

bool OTAUpdater::isNewerVersion(const String& serverVersion) {
    if (serverVersion.length() == 0) {
        return false;
    }
    
    uint32_t currentVer = parseVersion(String(FIRMWARE_VERSION));
    uint32_t serverVer = parseVersion(serverVersion);
    
    Serial.print("Version comparison - Current: 0x");
    Serial.print(currentVer, HEX);
    Serial.print(" Server: 0x");
    Serial.println(serverVer, HEX);
    
    return serverVer > currentVer;
}

bool OTAUpdater::performUpdate() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected - cannot perform update");
        return false;
    }
    
    HTTPClient http;
    String binUrl = String(_serverUrl) + String(_binPath);
    
    Serial.println("=== Starting OTA Update ===");
    Serial.print("Downloading from: ");
    Serial.println(binUrl);
    
    http.begin(binUrl);
    http.setTimeout(30000); // 30 second timeout for binary download
    
    int httpCode = http.GET();
    
    if (httpCode != HTTP_CODE_OK) {
        Serial.print("Failed to download firmware, HTTP code: ");
        Serial.println(httpCode);
        http.end();
        return false;
    }
    
    int contentLength = http.getSize();
    if (contentLength <= 0) {
        Serial.println("Invalid content length");
        http.end();
        return false;
    }
    
    Serial.print("Firmware size: ");
    Serial.print(contentLength);
    Serial.println(" bytes");
    
    // Check if there's enough space
    if (!Update.begin(contentLength)) {
        Serial.println("Not enough space for OTA update");
        http.end();
        return false;
    }
    
    // Get the stream
    WiFiClient* stream = http.getStreamPtr();
    
    uint8_t buff[512];
    size_t written = 0;
    int lastPercent = 0;
    
    Serial.println("Flashing firmware...");
    
    while (http.connected() && (written < contentLength)) {
        size_t available = stream->available();
        
        if (available) {
            int bytesToRead = ((available > sizeof(buff)) ? sizeof(buff) : available);
            int bytesRead = stream->readBytes(buff, bytesToRead);
            
            size_t bytesWritten = Update.write(buff, bytesRead);
            if (bytesWritten != bytesRead) {
                Serial.println("Write error during OTA update");
                Update.abort();
                http.end();
                return false;
            }
            
            written += bytesRead;
            
            // Print progress every 10%
            int percent = (written * 100) / contentLength;
            if (percent >= lastPercent + 10) {
                Serial.print("Progress: ");
                Serial.print(percent);
                Serial.println("%");
                lastPercent = percent;
            }
        }
        delay(1);
    }
    
    http.end();
    
    if (written != contentLength) {
        Serial.print("Download incomplete. Expected: ");
        Serial.print(contentLength);
        Serial.print(" Got: ");
        Serial.println(written);
        Update.abort();
        return false;
    }
    
    if (Update.end()) {
        Serial.println("OTA update completed successfully!");
        if (Update.isFinished()) {
            Serial.println("Update properly finished. Rebooting...");
            delay(1000);
            ESP.restart();
            return true; // Won't reach here
        } else {
            Serial.println("Update not finished properly");
            return false;
        }
    } else {
        Serial.print("Update error: ");
        Serial.println(Update.getError());
        return false;
    }
}

bool OTAUpdater::checkAndUpdate() {
    Serial.println("\n--- Checking for firmware updates ---");
    
    String serverVersion = getServerVersion();
    
    if (serverVersion.length() == 0) {
        Serial.println("Could not retrieve server version");
        return false;
    }
    
    if (isNewerVersion(serverVersion)) {
        Serial.println("✓ New version available! Starting update...");
        return performUpdate();
    } else {
        Serial.println("✓ Firmware is up to date");
        return false;
    }
}
