/*** Basic/Static Wifi connection
     Fichier wificonnect_simple/wificonnect_simple.ino ***/
#include <WiFi.h> // https://www.arduino.cc/en/Reference/WiFi
#include "wifi_utils.h"
#define USE_SERIAL Serial

void wifi_printstatus(int C);
String translateEncryptionType(wifi_auth_mode_t encryptionType);

bool connectWifi(String hostname) {
  /* Connection from a list of SSID */
  wificonnect_multi(hostname);

  /* WiFi status     --------------------------*/
  if (WiFi.status() == WL_CONNECTED){
    USE_SERIAL.print("\nWiFi connected : yes ! \n"); 
    wifi_printstatus(0);
    return true;  
  }
  USE_SERIAL.print("\nWiFi connected : no ! \n"); 
  //  ESP.restart();
  return false;
}

void wifi_printstatus(int C){
  /* print the status of the connected wifi  in two ways ! */

  if (C){
    // Use Pure C =>  array of chars
    USE_SERIAL.printf("WiFi Status : \n");
    USE_SERIAL.printf("\tIP address : %s\n", WiFi.localIP().toString().c_str());
    USE_SERIAL.printf("\tMAC address : %s\n", WiFi.macAddress().c_str());
    USE_SERIAL.printf("\tSSID : %s\n", WiFi.SSID());
    USE_SERIAL.printf("\tReceived Signal Strength Indication : %ld dBm\n",WiFi.RSSI());
    USE_SERIAL.printf("\tReceived Signal Strength Indication : %ld %\n",constrain(2 * (WiFi.RSSI() + 100), 0, 100));
    USE_SERIAL.printf("\tBSSID : %s\n", WiFi.BSSIDstr().c_str());
    USE_SERIAL.printf("\tEncryption type : %s\n", translateEncryptionType(WiFi.encryptionType(0)));
  }
  else {
    // Use of C++ =>  String !
    String s = "WiFi Status : \n";
    //s += "\t#" + String() + "\n";
    s += "\tIP address : " + WiFi.localIP().toString() + "\n"; 
    s += "\tMAC address : " + String(WiFi.macAddress()) + "\n";
    s += "\tSSID : " + String(WiFi.SSID()) + "\n";
    s += "\tReceived Sig Strength Indication : " + String(WiFi.RSSI()) + " dBm\n";
    s += "\tReceived Sig Strength Indication : " + String(constrain(2 * (WiFi.RSSI() + 100), 0, 100)) + " %\n";
    s += "\tBSSID : " + String(WiFi.BSSIDstr()) + "\n";
    s += "\tEncryption type : " + translateEncryptionType(WiFi.encryptionType(0))+ "\n";
    USE_SERIAL.print(s);
  }
}

String translateEncryptionType(wifi_auth_mode_t encryptionType) {
  //  encryptiontype to string      
  // cf https://www.arduino.cc/en/Reference/WiFiEncryptionType 
  switch (encryptionType) {
  case (WIFI_AUTH_OPEN):
    return "Open";
  case (WIFI_AUTH_WEP):
    return "WEP";
  case (WIFI_AUTH_WPA_PSK):
    return "WPA_PSK";
  case (WIFI_AUTH_WPA2_PSK):
    return "WPA2_PSK";
  case (WIFI_AUTH_WPA_WPA2_PSK):
    return "WPA_WPA2_PSK";
  case (WIFI_AUTH_WPA2_ENTERPRISE):
    return "WPA2_ENTERPRISE";
  }
}
