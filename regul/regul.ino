//------------------------------------------------------------------------------------------------------------------ Include libraries
#include <Adafruit_NeoPixel.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <ArduinoJson.h>

//------------------------------------------------------------------------------------------------------------------ Before setup
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
//- Setters
void setStrip(float temperature);
int setVentil(float temperature, bool onFire); // consider onFire to stop fan whenever it happen
bool setClim(float temperature);
bool setRad(float temperature);

//- Getters
int getPhoto();
float getTemp();

//- Utils
bool detectFire(float temperature); // return -> bool onFire
void updateWeightedLuminosity(int luminosity);
void sendData(HardwareSerial &s, float temperature, int luminosity, int fanSpeed, bool onFire, bool isHeated, bool isCooled);


// Initialise some datas
OneWire oneWire(tempPin);
DallasTemperature tempSensor(&oneWire);
Adafruit_NeoPixel strip(NUMLEDS, ledPin, NEO_GRB + NEO_KHZ800);

// Initialise constant
const float lowThreshold = 20.0f; // Usually set to 20.0f, 28.0f if tested
const float highThreshold = 30.0f; // Always at 30.0f
const float maxVentil = 33.5f; // Usually set to 33.5f, 31.0f if tested
const float avgLightDecay = 0.3f;

// Initialise weightedLuminosity in global scope
float weightedLuminosity = -1.0f;

//------------------------------------------------------------------------------------------------------------------ void setup
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

  //Serial.println("Setup Done!");
}

//------------------------------------------------------------------------------------------------------------------ void loop
void loop() {
  // Initialise scope variables
  float temperature;
  int luminosity;
  int fanSpeed;
  bool onFire;
  bool isHeated;
  bool isCooled;

  // Getters
  temperature = getTemp();
  luminosity = getPhoto();
  
  // Detect a potential fire when luminosity is low (luminosity is reversed due to the sensor)
  updateWeightedLuminosity(luminosity);
  onFire = detectFire(temperature);

  // Put the climatisation and ventilation if temp too high (note that ventilation increase in speed after the threshold)
  isCooled = setClim(temperature);
  fanSpeed = setVentil(temperature, onFire);

  // Put the radiator if temp too low
  isHeated = setRad(temperature);

  // Manage the colour of the led strip depending on the temperature
  setStrip(temperature);

  // Infos
  sendData(Serial, temperature, luminosity, fanSpeed, onFire, isHeated, isCooled);
  // Serial.print("Luminosity: "); Serial.print(luminosity, DEC); Serial.print(" [weighted:"); Serial.print(weightedLuminosity, DEC); Serial.println("]");
  // Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" C");
  // Serial.print("Fire: "); Serial.println(onFire);
  // Serial.print("FanSpeed: "); Serial.println(fanSpeed);

  // Delay to make the sensors work properly
  delay(2000);
}



//------------------------------------------------------------------------------------------------------------------ Getter functions
// Temperature getter
float getTemp() {
  tempSensor.requestTemperaturesByIndex(0);
  return tempSensor.getTempCByIndex(0);
}

// Luminosity getter
int getPhoto() {
  return analogRead(photoPin);//(4095-analogRead(photoPin))/4095;
}

//------------------------------------------------------------------------------------------------------------------ Setter functions
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
bool setClim(float temperature) {
  if (temperature > highThreshold) {
    digitalWrite(climPin, HIGH);
    return true;
  }
  digitalWrite(climPin, LOW);
  return false;
}

// Radiator setter
bool setRad(float temperature) {
  if (temperature < lowThreshold) {
    digitalWrite(radPin, HIGH);
    return true;
  } 
  digitalWrite(radPin, LOW);
  return false;
}

// Ventilation manager
int setVentil(float temperature, bool onFire) {
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

// update weigthed luminosity using exponential decay, this avoid enabling alarm with 
void updateWeightedLuminosity(int luminosity) {
  weightedLuminosity = avgLightDecay * weightedLuminosity + (1-avgLightDecay) * luminosity;
}

//------------------------------------------------------------------------------------------------------------------ Fire Detection
bool detectFire(float temperature) {
  // Hard limit because it may be messy if we transfer in percent
  if ((temperature > maxVentil) && weightedLuminosity < 100.0f) {
    digitalWrite(onboardPin, HIGH);
    return true;
  }
  digitalWrite(onboardPin, LOW);
  return false;
}

//------------------------------------------------------------------------------------------------------------------ Send Data

void sendData(HardwareSerial &s, float temperature, int luminosity, int fanSpeed, bool onFire, bool isHeated, bool isCooled) {
  DynamicJsonDocument doc(1024);

  JsonObject status = doc.createNestedObject("status");
    status["temperature"] = temperature;
    status["light"] = luminosity;
    status["regul"] = "HALT";
    status["fire"] = onFire;
    if (isHeated) {status["heat"] = "ON";} else {status["heat"] = "OFF";}
    if (isCooled) {status["cold"] = "ON";} else {status["cold"] = "OFF";}
    status["fanspeed"] = fanSpeed;

  JsonObject location = doc.createNestedObject("location");
    location["room"] = "282";

    JsonObject gps = location.createNestedObject("gps");
      gps["lat"] = 43.61708414;
      gps["lon"] = 7.06471621;
    location["address"] = "Les lucioles";

  JsonObject regul = doc.createNestedObject("regul");
    regul["lt"] = 26;
    regul["ht"] = 25.89999962;

  JsonObject info = doc.createNestedObject("info");
    info["ident"] = "ESP32 123";
    info["user"] = "AL";
    info["loc"] = "A Biot";

  JsonObject net = doc.createNestedObject("net");
    net["uptime"] = "55";
    net["ssid"] = "Livebox-B870";
    net["mac"] = "AC:67:B2:37:C9:48";
    net["ip"] = "192.168.1.45";

  JsonObject reporthost = doc.createNestedObject("reporthost");
    reporthost["target_ip"] = "127.0.0.1";
    reporthost["target_port"] = 1880;
    reporthost["sp"] = 2;

  serializeJson(doc, s);
  s.println();
}
