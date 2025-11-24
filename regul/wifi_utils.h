/*** Multi Wifi connection
     Fichier wificonnect_multi/wifi_utils.h ***/

#include <WiFi.h> // https://www.arduino.cc/en/Reference/WiFi
#include <WiFiMulti.h>

#define WiFiMaxTry 10
#define SaveDisconnectTime 1000 // Connection may need several tries 
                      // Time in ms for save disconnection, => delay between try
                      // cf https://github.com/espressif/arduino-esp32/issues/2501  

void wificonnect_multi(String hostname);
