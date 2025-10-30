/* 
 * Auteur : G.Menez
 * Fichier : http_as_serverasync/routes.ino 
 */
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "routes.h"

#define USE_SERIAL Serial
extern float temperature;
extern int luminosity, lightThreshold;

/*===================================================*/
String processor(const String& var){
  /* Replaces "placeholder" in  html file with sensors values */
  /* accessors functions get_... are in sensors.ino file   */

  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(temperature);
    /* On aimerait écrire : return get_temperature(TempSensor);
     * mais c'est un exemple de ce qu'il ne faut surtout pas écrire ! 
     * yield + async => core dump ! 
     */
    //return get_temperature(TempSensor);
  }
  else if(var == "LIGHT"){
    return String(luminosity);
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
  
  server->on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
      /* The most simple route => hope a response with temperature value */ 
      USE_SERIAL.printf("GET /temperature request \n"); 
      /* Exemple de ce qu'il ne faut surtout pas écrire car yield + async => core dump !*/
      //request->send_P(200, "text/plain", get_temperature(TempSensor).c_str());
      request->send_P(200, "text/plain", String(temperature).c_str());
    });

  server->on("/light", HTTP_GET, [](AsyncWebServerRequest *request){
      /* The most simple route => hope a response with light value */ 
      request->send_P(200, "text/plain", String(luminosity).c_str());
    });

  // This route allows users to change thresholds values through GET params
  server->on("/set", HTTP_GET, [](AsyncWebServerRequest *request){
      /* A route with a side effect : this get request has a param and should     
       *  set a new light_threshold ... used for regulation !
       */
      if (request->hasArg("light_threshold")) { // request may have arguments
	lightThreshold = atoi(request->arg("light_threshold").c_str());
	request->send_P(200, "text/plain", "Threshold Set !");
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
