#define onboardPin 2

//------------------------------------------------------------------------------------------------------------------ Fire Detection
bool detectFire(float temperature, float maxVentil, float weightedLuminosity) {
  // Hard limit because it may be messy if we transfer in percent
  if ((temperature > maxVentil) && (weightedLuminosity < 100.0f)) {
    digitalWrite(onboardPin, HIGH);
    return true;
  }
  digitalWrite(onboardPin, LOW);
  return false;
}