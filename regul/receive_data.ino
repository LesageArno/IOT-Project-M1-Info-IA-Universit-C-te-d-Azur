void receiveData(HardwareSerial &s, float *lowThreshold, float *highThreshold, float *maxVentil, float *avgLightDecay) {
  JsonDocument doc;
  if (s.available()) {
    deserializeJson(doc, s.readString());
  }

  String command = doc["command"];
  float val = doc["value"];

  if (command.equals("LowThreshold") && (val < *highThreshold)) {
    *lowThreshold = val;
  } else if (command.equals("HighThreshold") && (val > *lowThreshold) && (val < *maxVentil)) {
    *highThreshold = val;
  } else if (command.equals("VentilThreshold") && (val > *highThreshold)) {
    *maxVentil = val;
  } else if (command.equals("LightDecayFactor")) {
    *avgLightDecay = val;
  }
}