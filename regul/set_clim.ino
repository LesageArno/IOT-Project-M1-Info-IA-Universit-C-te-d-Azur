#define climPin 19

extern bool forceCooler;

// Climatisation setter
bool setClim(float temperature, float highThreshold) {

  // If we force the activation of the cooler from the API
  if (forceCooler) {
    digitalWrite(climPin, HIGH);
    return true;
  }

  if (temperature > highThreshold) {
    digitalWrite(climPin, HIGH);
    return true;
  }
  digitalWrite(climPin, LOW);
  return false;
}