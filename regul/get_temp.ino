#include <DallasTemperature.h>
#include <OneWire.h>

#define tempPin 23


// Initialise some datas
OneWire oneWire(tempPin);
DallasTemperature tempSensor(&oneWire);

void initTemp() {
  tempSensor.begin();
}

// Temperature getter
float getTemp() {
  tempSensor.requestTemperaturesByIndex(0);
  return tempSensor.getTempCByIndex(0);
}