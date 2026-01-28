// ApiClient.h - small wrapper to POST CSV payloads
#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <Arduino.h>

// Initialize the client (optionally set endpoint)
void apiClientBegin(const char* endpoint = nullptr);

// Send CSV payload (blocking). Payload should be a compact CSV string like "210,123,456,..."
void apiClientSendCsv(const String& csv);

// Backwards-compatible wrapper name used by main.cpp in older edits
// keep this symbol so existing calls to sendCsvToApi() continue to work
inline void sendCsvToApi(const String& csv) { apiClientSendCsv(csv); }

#endif // API_CLIENT_H

