//------------------------------------------------------------------------------------------------------------------ Include libraries

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

// Define if we allow the connection to a wireless connection
#define FORCE_CONNECTION true

// Functions signatures
//- Setters
void setStrip(float temperature, float lowThreshold, float highThreshold);
int setVentil(float temperature, bool onFire, float lowThreshold, float highThreshold, float maxVentil);
bool setClim(float temperature, float highThreshold);
bool setRad(float temperature, float lowThreshold);

//- Getters
int getPhoto();
float getTemp();

//- Utils
bool detectFire(float temperature, float maxVentil, float weightedLuminosity); // return -> bool onFire
float updateWeightedLuminosity(int luminosity, float avgLightDecay, float weightedLuminosity);
void sendData(HardwareSerial &s, float temperature, int luminosity, int fanSpeed, bool onFire, bool isHeated, bool isCooled, float lowThreshold, float highThreshold);
void receiveData(HardwareSerial &s, float *lowThreshold, float *highThreshold, float *maxVentil, float *avgLightDecay);
bool connectWifi();

//-Init
void initStrip();
void initTemp();


// Initialise constant
float lowThreshold = 20.0f; // Usually set to 20.0f, 28.0f if tested
float highThreshold = 30.0f; // Always at 30.0f
float maxVentil = 33.5f; // Usually set to 33.5f, 31.0f if tested
float avgLightDecay = 0.3f;

// Initialise weightedLuminosity in global scope
float weightedLuminosity = 4000.0f;

//------------------------------------------------------------------------------------------------------------------ void setup
void setup() {
  // Begin serial communication
  Serial.begin(9600);
  delay(2000);

  // Define the pins as either input or outputs
  pinMode(climPin, OUTPUT);
  pinMode(ventilPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(onboardPin, OUTPUT);
  pinMode(radPin, OUTPUT);
  pinMode(photoPin, INPUT);

  // For some reason, when uplodading, the ventilation is high, so set it to LOW at start
  digitalWrite(ventilPin, LOW);

  // Init strip and temperature
  initStrip();
  initTemp();

  // Force the connection or just try to connect with or without success.
  if (FORCE_CONNECTION) {
    while(!connectWifi("Mon petit ESP GB"));
  } else {
    connectWifi("Mon petit ESP GB");
  }
  
  
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

  // Receive potential data
  receiveData(Serial, &lowThreshold, &highThreshold, &maxVentil, &avgLightDecay);
  
  // Detect a potential fire when luminosity is low (luminosity is reversed due to the sensor)
  weightedLuminosity = updateWeightedLuminosity(luminosity, avgLightDecay, weightedLuminosity);
  onFire = detectFire(temperature, maxVentil, weightedLuminosity);

  // Put the climatisation and ventilation if temp too high (note that ventilation increase in speed after the threshold)
  isCooled = setClim(temperature, highThreshold);
  fanSpeed = setVentil(temperature, onFire, lowThreshold, highThreshold, maxVentil);

  // Put the radiator if temp too low
  isHeated = setRad(temperature, lowThreshold);

  // Manage the colour of the led strip depending on the temperature
  setStrip(temperature, lowThreshold, highThreshold);

  // Infos
  sendData(Serial, temperature, luminosity, fanSpeed, onFire, isHeated, isCooled, lowThreshold, highThreshold);
  //Serial.println(weightedLuminosity);

  // Delay to make the sensors work properly
  delay(2000);
}