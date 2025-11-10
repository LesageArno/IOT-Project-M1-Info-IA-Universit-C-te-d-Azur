/* 
 * Auteur : G.Menez
 * Fichier : http_as_serverasync/routes.ino 
 */
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <ArduinoJson.h>


#include "routes.h"

#define USE_SERIAL Serial
extern float temperature, lowThreshold, highThreshold, maxVentil, avgLightDecay;
extern int luminosity, lightThreshold;
extern bool isHeated, isCooled, onFire;
extern bool forceFire, forceHeater, forceCooler, forceVentil;


// Helper function to transform string to boolean
bool toBool(const String &value) {
  if (value.equalsIgnoreCase("true") || value == "1" || value.equalsIgnoreCase("on")) return true;
  if (value.equalsIgnoreCase("false") || value == "0" || value.equalsIgnoreCase("off")) return false;
  return false; // default if invalid
}

/*===================================================*/
String processor(const String& var){
  /* Replaces "placeholder" in  html file with sensors values */
  /* accessors functions get_... are in sensors.ino file   */

  //Serial.println(var);
  if (var == "TEMPERATURE") {
    return String(temperature);
    /* On aimerait écrire : return get_temperature(TempSensor);
     * mais c'est un exemple de ce qu'il ne faut surtout pas écrire ! 
     * yield + async => core dump ! 
     */
    //return get_temperature(TempSensor);
  } else if (var == "LIGHT") {
    return String(luminosity);
  } else if (var == "SSID") {
    return String(WiFi.SSID());
  } else if (var == "MAC") {
    return String(WiFi.macAddress());
  } else if (var == "IP") {
    return String(WiFi.localIP());
  } else if (var == "WHERE") {
    return "Les Lucioles";
  } else if (var == "UPTIME") {
    return String(millis()/1000);
  } else if (var == "HEATER") {
    return String(isHeated);
  } else if (var == "COOLER") {
    return String(isCooled);
  } else if (var == "LT") {
    return String(lowThreshold);
  } else if (var == "HT") {
    return String(highThreshold);
  }
  return String(); // parce que => cf doc de asyncwebserver
}

/*===================================================*/
void setup_http_routes(AsyncWebServer* server) {
  /* 
   * Sets up AsyncWebServer and routes 
   */
  
  // doc => Serve files in directory "/" when request url starts with "/"
  // Request to the root or none existing files will try to server the default
  // file name "index.htm" if exists 
  // => premier param la route et second param le repertoire servi (dans le FSUSED) 
  // Sert donc les fichiers css !  
  server->serveStatic("/", FSUSED, "/").setTemplateProcessor(processor);  
  
  // Declaring root handler, and action to be taken when root is requested
  auto root_handler = server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      /* This handler will download index.html (stored as FSUSED file) and will send it back */
      request->send(FSUSED, "/index.html", String(), false, processor); 
      // cf "Respond with content coming from a File containing templates" section in manual !
      // https://github.com/me-no-dev/ESPAsyncWebServer
      // request->send_P(200, "text/html", page_html, processor); // if page_html was a string .   
    });
  
  #ifdef CSS
  // Route to load style.css file
  server.on("/esp.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(FSUSED, "/esp.css", "text/css");
  });
  #endif

  server->on("/value", HTTP_GET, [](AsyncWebServerRequest *request){
    /* The most simple route => hope a response with temperature value */ 
    //USE_SERIAL.printf("GET /temperature request \n"); 
    /* Exemple de ce qu'il ne faut surtout pas écrire car yield + async => core dump !*/
    //request->send_P(200, "text/plain", get_temperature(TempSensor).c_str());
    
    // Create the JSON to return
    DynamicJsonDocument doc(256);
    String jsonToSend;

    // If the client requested temperature
    if (request->hasArg("temperature")) {
      doc["temperature"] = temperature;
    }
      
    // If the client requested light
    if (request->hasArg("light")) {
      doc["light"] = luminosity;
    }

    // If the client requested fire status
    if (request->hasArg("fire")) {
      doc["fire"] = onFire;
    }

    // If the client requested low threshold
    if (request->hasArg("lt")) {
      doc["lt"] = lowThreshold;
    }

    // If the client requested high threshold
    if (request->hasArg("ht")) {
      doc["ht"] = highThreshold;
    }

    // Send the JSON
    serializeJson(doc, jsonToSend);
    request->send_P(200, "application/json", jsonToSend.c_str());
    
  });
  
  // This route allows users to change thresholds values through GET params
  server->on("/set", HTTP_GET, [](AsyncWebServerRequest *request){
      /* A route with a side effect : this get request has a param and should     
      *  set a new light_threshold ... used for regulation !
      */

      // Count the number of valid parameter in the request and return 404 if one is not known. 
      int paramCounter = 0;

      // Update Higher threshold
      float tmpFloat;
      if (request->hasArg("ht")) {
        //Ensure thresholds integrity before pushing updates
        tmpFloat = request->arg("ht").toFloat();
        if (tmpFloat < maxVentil && tmpFloat > lowThreshold) {
          highThreshold = tmpFloat;
        }
        paramCounter++;
      }

      // Update Lower Threshold
      if (request->hasArg("lt")) {
        tmpFloat = request->arg("lt").toFloat();
        if (tmpFloat < highThreshold) {
          lowThreshold = tmpFloat;
        }
        paramCounter++;
      }

      // Update Max Venitl Threshold
      if (request->hasArg("vt")) {
        tmpFloat = request->arg("vt").toFloat();
        if (highThreshold < tmpFloat) {
          maxVentil = tmpFloat;
        }
        paramCounter++;
      }

      // Update Average Light Decay factor (Light sensibility for the exponential average)
      if (request->hasArg("avgLightDecay")) {
        tmpFloat = request->arg("avgLightDecay").toFloat();
        if (avgLightDecay >= 0 && avgLightDecay <= 1) {
          avgLightDecay = tmpFloat;
        }
        paramCounter++;
      }

      // Action cooler
      if (request->hasArg("cooler")) {
        forceCooler = toBool(request->arg("cooler"));
        paramCounter++;
      } 

      // Action Heater
      if (request->hasArg("heater")) {
        forceHeater = toBool(request->arg("heater"));
        paramCounter++;
      }

      // Action Ventilation
      if (request->hasArg("ventil")) {
        forceVentil = toBool(request->arg("ventil"));
        paramCounter++;
      }

      // Action Fire alarm
      if (request->hasArg("fire")) {
        forceFire = toBool(request->arg("fire"));
        paramCounter++;
      }

      // If no parameter 404
      if (paramCounter >= 1) {
        request->send_P(200, "text/plain", "Operation(s) Done !");
      } else {
        request->send(404);
      }
    	
  });
  
  server->on("/target", HTTP_POST, [](AsyncWebServerRequest *request){
      /* A route receiving a POST request with Internet coordinates 
       * of the reporting target host.
       */
      Serial.println("Receive Request for a periodic report !"); 
      if (request->hasArg("ip") && request->hasArg("port") && request->hasArg("sp")) {
	      target_ip = request->arg("ip");
	      target_port = atoi(request->arg("port").c_str());
	      target_sp = atoi(request->arg("sp").c_str());
      }
      request->send(FSUSED, "/index.html", String(), false, processor);
    });
  
  // If request doesn't match any route, returns 404.
  server->onNotFound([](AsyncWebServerRequest *request){
      request->send(404);
    });
}
/*===================================================*/
