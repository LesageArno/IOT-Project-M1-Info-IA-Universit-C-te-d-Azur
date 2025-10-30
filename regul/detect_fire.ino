#define onboardPin 2

extern int lightThreshold;

//------------------------------------------------------------------------------------------------------------------ Fire Detection
bool detectFire(float temperature, float maxVentil, float weightedLuminosity) {
  // Hard limit because it may be messy if we transfer in percent
  if ((temperature > maxVentil) && (weightedLuminosity < lightThreshold)) {
    digitalWrite(onboardPin, HIGH);
    return true;
  }
  digitalWrite(onboardPin, LOW);
  return false;
}