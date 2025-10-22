#define climPin 19

// Climatisation setter
bool setClim(float temperature, float highThreshold) {
  if (temperature > highThreshold) {
    digitalWrite(climPin, HIGH);
    return true;
  }
  digitalWrite(climPin, LOW);
  return false;
}