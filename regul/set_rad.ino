#define radPin 21

// Radiator setter
bool setRad(float temperature, float lowThreshold) {
  if (temperature < lowThreshold) {
    digitalWrite(radPin, HIGH);
    return true;
  } 
  digitalWrite(radPin, LOW);
  return false;
}