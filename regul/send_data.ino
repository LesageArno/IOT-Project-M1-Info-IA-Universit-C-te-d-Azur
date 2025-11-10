#include <WiFi.h>

extern String target_ip;
extern int target_port, target_sp;

DynamicJsonDocument createJsonState(float temperature, int luminosity, int fanSpeed, bool onFire, bool isHeated, bool isCooled, float lowThreshold, float highThreshold) {
  DynamicJsonDocument doc(1024);

  JsonObject status = doc.createNestedObject("status");
    status["temperature"] = temperature;
    status["light"] = luminosity;
    status["regul"] = "HALT";
    status["fire"] = onFire;
    if (isHeated) {status["heat"] = "ON";} else {status["heat"] = "OFF";}
    if (isCooled) {status["cold"] = "ON";} else {status["cold"] = "OFF";}
    status["fanspeed"] = fanSpeed;

  JsonObject location = doc.createNestedObject("location");
    location["room"] = "282";

    JsonObject gps = location.createNestedObject("gps");
      gps["lat"] = 43.61708414;
      gps["lon"] = 7.06471621;
    location["address"] = "Les lucioles";

  JsonObject regul = doc.createNestedObject("regul");
    regul["lt"] = lowThreshold;
    regul["ht"] = highThreshold;

  JsonObject info = doc.createNestedObject("info");
    info["ident"] = "ESP32 123";
    info["user"] = "AL";
    info["loc"] = "A Biot";

  JsonObject net = doc.createNestedObject("net");
    net["uptime"] = millis()/1000;
    net["ssid"] = WiFi.SSID();
    net["mac"] = WiFi.macAddress();
    net["ip"] = WiFi.localIP();

  JsonObject reporthost = doc.createNestedObject("reporthost");
    reporthost["target_ip"] = target_ip;
    reporthost["target_port"] = target_port;
    reporthost["sp"] = target_sp;
  
  return doc;
}


//------------------------------------------------------------------------------------------------------------------ Send Data
void sendData(HardwareSerial &s, float temperature, int luminosity, int fanSpeed, bool onFire, bool isHeated, bool isCooled, float lowThreshold, float highThreshold) {
  serializeJson(createJsonState(temperature, luminosity, fanSpeed, onFire, isHeated, isCooled, lowThreshold, highThreshold) , s);
  s.println();
}