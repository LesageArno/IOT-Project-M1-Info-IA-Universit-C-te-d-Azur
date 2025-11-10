#define radPin 21

extern bool forceHeater;

// Radiator setter
bool setRad(float temperature, float lowThreshold) {

  // If we force the activation of the heater from the API
  if (forceHeater) {
    digitalWrite(radPin, HIGH);
    return true;
  }

  if (temperature < lowThreshold) {
    digitalWrite(radPin, HIGH);
    return true;
  } 
  digitalWrite(radPin, LOW);
  return false;
}