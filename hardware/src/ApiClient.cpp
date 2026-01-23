#include "ApiClient.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "credentials.h"

// default endpoint (can be overridden in apiClientBegin)
static const char* s_endpoint = "http://145.24.237.155:8001/data";
// default device id used in JSON payloads. Change to your preferred id.
static const char* s_deviceId = "Device002";

void apiClientBegin(const char* endpoint) {
  if (endpoint && endpoint[0] != '\0') s_endpoint = endpoint;
}

void apiClientSendCsv(const String& csv) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi not connected; skipping CSV POST.");
    return;
  }

  HTTPClient http;
  Serial.print("POST to "); Serial.println(s_endpoint);
  if (!http.begin(s_endpoint)) {
    Serial.println("HTTP begin failed");
    return;
  }

  // Build a simple JSON payload the server expects. Example:
  // {"deviceId":"Device002","value":"210,1,2,3,4"}
  String json;
  json.reserve(csv.length() + 64);
  json += "{\"deviceId\":\"";
  json += s_deviceId;
  json += "\",\"value\":\"";
  json += csv;
  json += "\"}";

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");

  Serial.print("Sending JSON payload: ");
  Serial.println(json);

  int httpCode = http.POST(json);
  if (httpCode > 0) {
    Serial.print("HTTP POST code: "); Serial.println(httpCode);
    String payload = http.getString();
    if (payload.length()) {
      Serial.print("Server response: ");
      Serial.println(payload);
    } else {
      Serial.println("Server returned no payload.");
    }
  } else {
    Serial.print("HTTP POST failed, error: ");
    Serial.println(httpCode);
  }
  http.end();
}

