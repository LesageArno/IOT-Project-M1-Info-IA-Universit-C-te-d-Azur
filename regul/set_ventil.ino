#define ventilPin 27

extern bool forceVentil;

// Ventilation manager
int setVentil(float temperature, bool onFire, float lowThreshold, float highThreshold, float maxVentil) {

  // If we force the activation of the ventil from the API
  if (forceVentil && !onFire) {
    analogWrite(ventilPin, 255);
    return 255;
  }

  int fanSpeed = 0;
  if (temperature > highThreshold && !onFire) {
    // Set the ventillation from 50 to 255 depending on the temperature above the threshold
    fanSpeed = (int)min((int)map(temperature, highThreshold, maxVentil, 50, 255), 255);
    analogWrite(ventilPin, fanSpeed);
  } else {
    analogWrite(ventilPin, 0);
  }
  return fanSpeed;
}