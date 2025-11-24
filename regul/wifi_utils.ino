/*** Multi Wifi connection
     Fichier wificonnect_multi/wifi_utils.ino ***/

#include <WiFi.h> // https://www.arduino.cc/en/Reference/WiFi
#include "wifi_utils.h"

#define USE_SERIAL Serial

/*--------------------------------------------------------------------------*/
void wificonnect_multi(String hostname){
  int nbtry = 0; // Nb of try to connect
   
  WiFiMulti wm; // Creates an instance of the WiFiMulti class
  // Attention ! PAS arrivé à sortir l'instance de la fonction => heap error ! Why ???? 
  wm.addAP("HUAWEI-6EC2", "FGY9MLBL");
  wm.addAP("HUAWEI-553A", "QTM06RTT");
  wm.addAP("GMAP", "vijx47050");
  wm.addAP("IOT", "iotmiage");
  wm.addAP("Bbox-EA6814CF", "Hvtx6bGfD4Sn7PCs2r");
  wm.addAP("Livebox-46F0", "EUedRJuyxCXiJaD3fo");
  wm.addAP("Microonde_a_air_comprime", "12345678");
  wm.addAP("ArnoTragbarenHotspot", "Ah76&OP*");
  
  //WiFi.mode(WIFI_OFF);   
  WiFi.mode(WIFI_STA); // Set WiFi to station mode 
  // delete old config
  // WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.disconnect(true); // Disconnect from an AP if it was previously connected
  // WiFi.persistent(false); // Avoid to store Wifi configuration in Flash
  
  // Define hostname  => in C str ! not C++
  WiFi.setHostname(hostname.c_str());
  
  // Try to connect a particular number of time
  while(wm.run() != WL_CONNECTED && (nbtry < WiFiMaxTry)) {
    delay(SaveDisconnectTime);
    nbtry++;
  }
}