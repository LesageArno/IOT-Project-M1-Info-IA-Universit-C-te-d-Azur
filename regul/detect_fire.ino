#define onboardPin 2


extern int lightThreshold;
extern bool forceFire;

//------------------------------------------------------------------------------------------------------------------ Fire Detection
bool detectFire(float temperature, float maxVentil, float weightedLuminosity) {

  // If we force fire detection from the API
  if (forceFire) {
    digitalWrite(onboardPin, HIGH);
    return true;
  }

  // Hard limit because it may be messy if we transfer in percent
  if ((temperature > maxVentil) && (weightedLuminosity < lightThreshold)) {
    digitalWrite(onboardPin, HIGH);
    return true;
  }
  digitalWrite(onboardPin, LOW);
  return false;
}