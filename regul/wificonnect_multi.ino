/*** Basic/Static Wifi connection
     Fichier wificonnect_simple/wificonnect_simple.ino ***/
#include <WiFi.h> // https://www.arduino.cc/en/Reference/WiFi
#include "wifi_utils.h"
#define USE_SERIAL Serial

bool connectWifi(String hostname) {
  /* Connection from a list of SSID */
  wificonnect_multi(hostname);

  /* WiFi status     --------------------------*/
  if (WiFi.status() == WL_CONNECTED){
    // USE_SERIAL.print("\nWiFi connected : yes ! \n"); 
    // wifi_printstatus(0);
    return true;  
  }
  // USE_SERIAL.print("\nWiFi connected : no ! \n"); 
  //  ESP.restart();
  return false;
}
