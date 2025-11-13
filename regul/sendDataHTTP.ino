#include <HTTPClient.h>
#include <WiFi.h>
#include "wifi_utils.h"

extern int target_port;
extern String target_ip;

void sendDataHTTP(float temperature, int luminosity, int fanSpeed, bool onFire, bool isHeated, bool isCooled, float lowThreshold, float highThreshold) {
  HTTPClient http;

  // Build full URL
  String url = "http://" + target_ip + ":" + String(target_port) + "/esp";
  Serial.println(url);

  // Retrieve the JSON to send via POST
  DynamicJsonDocument doc = createJsonState(temperature, luminosity, fanSpeed, onFire, isHeated, isCooled, lowThreshold, highThreshold);
  String payload; 
  serializeJson(doc, payload);


  // Start http connection
  http.begin(url);
  http.addHeader("Content-Type", "plain/text");

  // Perform POST
  int httpCode = http.POST(payload);

  // Handle response
  if (httpCode > 0) {
    Serial.printf("POST code: %d\n", httpCode);
    String response = http.getString();
    Serial.println("Response: " + response);
  } else {
    Serial.printf("POST failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  // Cut connection
  http.end();
}
