#include <Adafruit_NeoPixel.h>

#define ledPin 13
#define NUMLEDS 5


Adafruit_NeoPixel strip(NUMLEDS, ledPin, NEO_GRB + NEO_KHZ800);
void initStrip() {
  strip.begin();
}

// LED strip manager
void setStrip(float temperature, float lowThreshold, float highThreshold) {
  int r, g, b;

  // Select colour depending on temperature
  if (temperature > highThreshold) {
    r = 89; g = 0; b = 0;
  } else if (temperature < lowThreshold) {
    r = 0; g = 0; b = 89;
  } else {
    r = 0; g = 89; b = 0; 
  }

  // Set same colour for each del
  for (int i=0; i<NUMLEDS; i++) {
    strip.setPixelColor(i, strip.Color(r,g,b));
  }
  strip.show();
}