#include <WiFi.h>

//------------------------------------------------------------------------------------------------------------------ Send Data
void sendData(HardwareSerial &s, float temperature, int luminosity, int fanSpeed, bool onFire, bool isHeated, bool isCooled, float lowThreshold, float highThreshold) {
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
    net["uptime"] = "...";
    net["ssid"] = WiFi.SSID();
    net["mac"] = WiFi.macAddress();
    net["ip"] = WiFi.localIP();

  JsonObject reporthost = doc.createNestedObject("reporthost");
    reporthost["target_ip"] = "127.0.0.1";
    reporthost["target_port"] = 1880;
    reporthost["sp"] = 2;

  serializeJson(doc, s);
  s.println();
}