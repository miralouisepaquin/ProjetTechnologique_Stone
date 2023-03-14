#include <Arduino.h>
#include <PubSubClient.h>
#include "myFunctions.cpp"
using namespace std;

#include <WiFiManager.h>
WiFiManager wm;
#define WEBSERVER_H

//Variable pour la connection Wifi
const char *SSID = "SAC_";
const char *PASSWORD = "sac_";
String ssIDRandom;

//Variable pour vitesse baud Serial
#define BAUD 9600

//Variable pour le code (Pin) entré
std::string code = "1234";

//fonction statique qui permet aux objets d'envoyer des messages (callBack) 
//  arg0 : Action 
// arg1 ... : Parametres
std::string CallBackMessageListener(string message) {
    while(replaceAll(message, std::string("  "), std::string(" ")));
    //Décortiquer le message
    string actionToDo = getValue(message, ' ', 0);
    string arg1 = getValue(message, ' ', 1);
    string arg2 = getValue(message, ' ', 2);
    string arg3 = getValue(message, ' ', 3);
    string arg4 = getValue(message, ' ', 4);
    string arg5 = getValue(message, ' ', 5);
    string arg6 = getValue(message, ' ', 6);
    string arg7 = getValue(message, ' ', 7);
    string arg8 = getValue(message, ' ', 8);
    string arg9 = getValue(message, ' ', 9);
    string arg10 = getValue(message, ' ', 10);
    
    if (string(actionToDo.c_str()).compare(string("askCode")) == 0) {     
        return(code.c_str()); }
   
    std::string result = "";
    return result;
    }

void setup() {
  Serial.begin(BAUD);
  delay(100);
  
  //---------Connection au WifiManager--------------
    String ssIDRandom, PASSRandom;
    String stringRandom;
    stringRandom = get_random_string(4).c_str();
    ssIDRandom = SSID;
    ssIDRandom = ssIDRandom + stringRandom;
    stringRandom = get_random_string(4).c_str();
    PASSRandom = PASSWORD;
    PASSRandom = PASSRandom + stringRandom;

    if (!wm.autoConnect(ssIDRandom.c_str(), PASSRandom.c_str())){
      Serial.println("Erreur de connexion.");
      }
    else {
      Serial.println("Connexion Établie:");
      }
}

void loop() {
  // put your main code here, to run repeatedly:
}