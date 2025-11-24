/*********
	Based on Rui Santos work : https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/
	File mqtt_full/mqtt_full.ino
	Modified by GM

	Tests with CLI :
	
  mosquitto_pub  -h broker.hivemq.com -t "uca/iot/master/grb/ventil" -m "true" -q 1
  mosquitto_sub  -h broker.hivemq.com -t "uca/iot/master"  -q 1
*********/
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "wifi_utils.h"

/*===== MQTT broker/server ========*/
//const char* mqtt_server = "192.168.1.101"; 
//const char* mqtt_server = "public.cloud.shiftr.io"; // Failed in 2021
// need login and passwd (public,public) mqtt://public:public@public.cloud.shiftr.io
const char* mqtt_server = "broker.hivemq.com"; // anynomous Ok in 2021 
//const char* mqtt_server = "test.mosquitto.org"; // anynomous Ok in 2021
//const char* mqtt_server = "mqtt.eclipseprojects.io"; // anynomous Ok in 2021 // Don't work anymore
/*===== MQTT TOPICS ===============*/
// Publisher
#define TOPIC_MASTER "uca/iot/master"

// Thresholds subscribers (high, low, maxVentil [fire])
#define TOPIC_HT "uca/iot/master/grb/ht"
#define TOPIC_LT "uca/iot/master/grb/lt"
#define TOPIC_VT "uca/iot/master/grb/vt"
#define TOPIC_AVG_LIGHT_DECAY "uca/iot/master/grb/avgLightDecay"
#define TOPIC_LIGHT_THRESHOLD "uca/iot/master/grb/lightThreshold"

// Actionner subscribers
#define TOPIC_COOLER "uca/iot/master/grb/cooler"
#define TOPIC_HEATER "uca/iot/master/grb/heater"
#define TOPIC_VENTIL "uca/iot/master/grb/ventil"
#define TOPIC_FIRE "uca/iot/master/grb/fire"

#define USE_SERIAL Serial

// Some external variables
extern float lowThreshold, highThreshold, maxVentil, avgLightDecay;
extern int lightThreshold;
extern bool forceFire, forceHeater, forceCooler, forceVentil;
extern PubSubClient mqttclient;

/*============== CALLBACK ===================*/
void mqtt_pubcallback(char* topic, 
                      byte* payload, 
                      unsigned int length) {
  /* 
   * Callback when a message is published on a subscribed topic.
   */
  USE_SERIAL.print("Message arrived on topic : ");
  USE_SERIAL.println(topic);
  USE_SERIAL.print("=> ");

  // Byte list (of the payload) to String and print to Serial
  String message;
  for (int i = 0; i < length; i++) {
    //USE_SERIAL.print((char)payload[i]);
    message += (char)payload[i];
  }
  USE_SERIAL.println(message);

  /*
  char msg[length + 1];
  memcpy(msg, payload, length);
  msg[length] = NULL;
  message = String(msg);
  */

  // Feel free to add more if statements to control more GPIOs with MQTT

  // Callback for the temperature threshold and threshold integrity check
  if (String(topic) == TOPIC_VT) {
    float tmpFloat = message.toFloat();
    if (highThreshold < tmpFloat) {
      maxVentil = tmpFloat;
      USE_SERIAL.println("MaxVentil Threshold changed!");
    } else {
      USE_SERIAL.println("MaxVentil Threshold could not be changed!");
    }
  } else if (String(topic) == TOPIC_HT) {
    float tmpFloat = message.toFloat();
    if (tmpFloat < maxVentil && tmpFloat > lowThreshold) {
      highThreshold = tmpFloat;
      USE_SERIAL.println("High Threshold changed!");
    } else {
      USE_SERIAL.println("High Threshold could not be changed!");
    }
  } else if (String(topic) == TOPIC_LT) {
    float tmpFloat = message.toFloat();
    if (tmpFloat < highThreshold) {
      USE_SERIAL.println("Low Threshold changed!");
      lowThreshold = tmpFloat;
    } else {
      USE_SERIAL.println("Low Threshold could not be changed!");
    }
  
  // Callback for the light sensitivity parameter in the fire detection computation + integrity check and light threshold
  } else if (String(topic) == TOPIC_AVG_LIGHT_DECAY) {
    float tmpFloat = message.toFloat();
    if (avgLightDecay >= 0 && avgLightDecay <= 1) {
      USE_SERIAL.println("Average Light decay fator changed!");
      avgLightDecay = tmpFloat;
    } else {
      USE_SERIAL.println("Average Light decay fator could not be changed!");
    }
  } else if (String(topic) == TOPIC_LIGHT_THRESHOLD) {
    int tmpInt = message.toInt();
    if (tmpInt >= 0) {
      USE_SERIAL.println("Light threshold changed!");
      lightThreshold = tmpInt;
    } else {
      USE_SERIAL.println("Light threshold could not be changed!");
    }
   

  
  // Callback for the actionners
  } else if (String(topic) == TOPIC_COOLER) {
    USE_SERIAL.println("Cooler Status changed!");
    forceCooler = toBool(message);
  } else if (String(topic) == TOPIC_HEATER) {
    USE_SERIAL.println("Heater Status changed!");
    forceHeater = toBool(message);
  } else if (String(topic) == TOPIC_VENTIL) {
    USE_SERIAL.println("Ventilation Status changed!");
    forceVentil = toBool(message);
  } else if (String(topic) == TOPIC_FIRE) {
    USE_SERIAL.println("Fire Status changed!");
    forceFire = toBool(message);
  }
}

/*============= SUBSCRIBE to TOPICS ===================*/
void mqtt_subscribe_mytopics() {
  /*
   * Subscribe to MQTT topics :
   * There is no way on checking the subscriptions from a client. 
   * But you can also subscribe WHENEVER you connect. 
   * Then it is guaranteed that all subscriptions are existing.
   * => If the client is already connected then we have already subscribe
   * since connection and subscriptions go together !
   */
  // Checks whether the client is connected to the MQTT server
  while (!mqttclient.connected()) { // Loop until we're reconnected https://pubsubclient.knolleary.net/api
    USE_SERIAL.print("Attempting MQTT connection...");
    
    // Create a client ID from MAC address .. should be unique ascii string and different from all other devices using the broker !
    String mqttclientId = "ESP32-";
    mqttclientId += WiFi.macAddress(); // if we need random : String(random(0xffff), HEX);
    if (mqttclient.connect(mqttclientId.c_str(), // Mqttclient Id when connecting to the server : 8-12 alphanumeric character ASCII
			   NULL,   /* No credential */ 
			   NULL))
      {
      USE_SERIAL.println("connected");
	        
      // THEN Subscribe topics
      mqttclient.subscribe(TOPIC_HT, 1);
      mqttclient.subscribe(TOPIC_LT, 1);
      mqttclient.subscribe(TOPIC_VT, 1);
      mqttclient.subscribe(TOPIC_AVG_LIGHT_DECAY, 1);
      mqttclient.subscribe(TOPIC_COOLER, 1);
      mqttclient.subscribe(TOPIC_HEATER, 1);
      mqttclient.subscribe(TOPIC_VENTIL, 1);
      mqttclient.subscribe(TOPIC_FIRE, 1);
      // mqttclient.subscribe(anothertopic ?);
    } 
    else { // Connection to broker failed : retry !
      USE_SERIAL.print("failed, rc=");
      USE_SERIAL.print(mqttclient.state());
      USE_SERIAL.println(" try again in 5 seconds");
      delay(5000); // Wait 5 seconds before retrying
    }
  } // end while
}


void sendMQTT(float temperature, int luminosity, int fanSpeed, bool onFire, bool isHeated, bool isCooled, float lowThreshold, float highThreshold) {
  // Retrieve the JSON to send
  DynamicJsonDocument doc = createJsonState(temperature, luminosity, fanSpeed, onFire, isHeated, isCooled, lowThreshold, highThreshold);
  String payload; 
  serializeJson(doc, payload);

  // Serial info
  USE_SERIAL.print("Published payload : "); USE_SERIAL.print(payload); 
  USE_SERIAL.print(" on topic : "); USE_SERIAL.println(TOPIC_MASTER);
  
  /*--- Publish payload on TOPIC_TEMP  */
  mqttclient.publish(TOPIC_MASTER, payload.c_str());
}

