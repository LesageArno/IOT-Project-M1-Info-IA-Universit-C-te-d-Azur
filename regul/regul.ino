// Include libraries
#include <Adafruit_NeoPixel.h>
#include <DallasTemperature.h>
#include <OneWire.h>

// Define input and output pins
#define tempPin 23
#define climPin 19
#define ventilPin 27
#define radPin 21
#define ledPin 13
#define photoPin 33
#define onboardPin 2

// Define the number of DEL
#define NUMLEDS 5

// Functions signatures
void setStrip(float temperature);
void setVentil(float temperature);
void setClim(float temperature);
void setRad(float temperature);

float getPhoto();
float getTemp();

void detectFire(float temperature, int luminosity);

// Initialise some datas
OneWire oneWire(tempPin);
DallasTemperature tempSensor(&oneWire);
Adafruit_NeoPixel strip(NUMLEDS, ledPin, NEO_GRB + NEO_KHZ800);

// Initialise constant
const float lowThreshold = 20.0f; // Usually set to 20.0f, 28.0f if tested
const float highThreshold = 30.0f; // Always at 30.0f
const float maxVentil = 33.5f; // Usually set to 33.5f, 31.0f if tested


void setup() {
  // Begin serial communication
  Serial.begin(9600);
  delay(2000);

  // Begin strip and tempSensor
  tempSensor.begin();
  strip.begin();

  // Define the pins as either input or outputs
  pinMode(climPin, OUTPUT);
  pinMode(ventilPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(onboardPin, OUTPUT);
  pinMode(radPin, OUTPUT);

  pinMode(photoPin, INPUT);
  delay(1);

  Serial.println("Setup Done!");
}


void loop() {
  // Initialise scope variables
  float temperature;
  int luminosity;

  // Getters
  temperature = getTemp();
  luminosity = getPhoto();
  //Serial.print("Luminosity: "); Serial.println(luminosity, DEC);

  // Put the climatisation and ventilation if temp too high (note that ventilation increase in speed after the threshold)
  setClim(temperature);
  setVentil(temperature);

  // Put the radiator if temp too low
  setRad(temperature);

  // Manage the colour of the led strip depending on the temperature
  setStrip(temperature);

  // Detect a potential fire when luminosity is low (luminosity is reversed due to the sensor)
  detectFire(temperature, luminosity);

  // Delay to make the sensors work properly
  delay(2000);
}





// Temperature getter
float getTemp() {
  float t;
  tempSensor.requestTemperaturesByIndex(0);
  t = tempSensor.getTempCByIndex(0);
  Serial.print("Temperature: "); Serial.print(t); Serial.println(" C");
  return t;
}

// Luminosity getter
float getPhoto() {
  return analogRead(photoPin);//(4095-analogRead(photoPin))/4095;
}

// LED strip manager
void setStrip(float temperature) {
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

// Climatisation setter
void setClim(float temperature) {
  if (temperature > highThreshold) {
    digitalWrite(climPin, HIGH);
  } else {
    digitalWrite(climPin, LOW);
  }
}

// Radiator setter
void setRad(float temperature) {
  if (temperature < lowThreshold) {
    digitalWrite(radPin, HIGH);
  } else {
    digitalWrite(radPin, LOW);
  }
}

// Ventilation manager
void setVentil(float temperature) {
  if (temperature > highThreshold) {
    // Set the ventillation from 50 to 255 depending on the temperature above the threshold
    analogWrite(ventilPin, (int)min((int)map(temperature, highThreshold, maxVentil, 50, 255), 255));
  } else {
    analogWrite(ventilPin, 0);
  }
}

// Fire detection
void detectFire(float temperature, int luminosity) {
  // Hard limit because it may be messy if we transfer in percent
  if ((temperature > maxVentil) && luminosity < 100) {
    digitalWrite(onboardPin, HIGH);
  } else {
    digitalWrite(onboardPin, LOW);
  }
}