//------------------------------------------------------------------------------------------------------------------ Include libraries

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "routes.h"

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
void sendDataHTTP(float temperature, int luminosity, int fanSpeed, bool onFire, bool isHeated, bool isCooled, float lowThreshold, float highThreshold);
void receiveData(HardwareSerial &s, float *lowThreshold, float *highThreshold, float *maxVentil, float *avgLightDecay);
bool connectWifi();

//-Init
void initStrip();
void initTemp();

//-Void Loop main functions
void DoLoop(int delai);
void DoPeriodicReport(int delai);


// Initialise "constant"
float lowThreshold = 20.0f; // Usually set to 20.0f, 28.0f if tested
float highThreshold = 30.0f; // Always at 30.0f
float maxVentil = 33.5f; // Usually set to 33.5f, 31.0f if tested
float avgLightDecay = 0.3f;
int lightThreshold = 100;

// Initialise and or declare variables in global scope
float weightedLuminosity = 4000.0f;
float temperature;
int luminosity;
int fanSpeed;
bool onFire;
bool isHeated;
bool isCooled;

// Declare variables to forces component of the system to be set to particular value
bool forceFire = false;
bool forceHeater = false;
bool forceCooler = false; 
bool forceVentil = false;

// Target_sp and "port"
extern int target_sp = 10;
int target_port = 1883; 

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create MQTT Client & server
extern const char* mqtt_server;
WiFiClient espClient;           // Wifi 
PubSubClient mqttclient(espClient); // MQTT client


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
    while(!connectWifi("Mon petit ESP GB")) {
      Serial.println("Connection failed, retry...");
    }
  } else {
    connectWifi("Mon petit ESP GB");
  }
  
  // Initialize FSUSED
  FSUSED.begin(true);

  // Setup routes of the ESP Web server
  setup_http_routes(&server);
  
  // Start ESP Web server
  server.begin();

  // set server of our MQTT client
  mqttclient.setServer(mqtt_server, target_port);
  // set callback when publishes arrive for the subscribed topics
  mqttclient.setCallback(mqtt_pubcallback); 
  
  delay(1);

  //Serial.println("Setup Done!");
}

//------------------------------------------------------------------------------------------------------------------ void loop
void loop() {
  /* Process MQTT ... une fois par loop() ! */
  mqttclient.loop(); // Process MQTT event/action
  /*--- subscribe to TOPIC_LED if not yet ! */
  mqtt_subscribe_mytopics();

  // Update data every 2000 ms
  DoLoop(2000);
  DoPeriodicReport(target_sp*1000);
}

// Action to do once in a while in void loop
void DoLoop(int delai){
  static uint32_t tick = 0;
  if (millis() - tick < delai) { 
    return; 
  } else { /* Do stuff here every XX seconds */ 
    tick = millis();

    // Getter
    temperature = getTemp();
    luminosity = getPhoto();

    // Receive potential data
    //receiveData(Serial, &lowThreshold, &highThreshold, &maxVentil, &avgLightDecay);

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
  } 
}

void DoPeriodicReport(int delai) {
  static uint32_t tick = 0;
  if ((millis() - tick < delai) || delai == 0) { 
    return; 
  } else { /* Do stuff here every XX seconds */ 
    tick = millis();
    sendMQTT(temperature, luminosity, fanSpeed, onFire, isHeated, isCooled, lowThreshold, highThreshold);
  }
}