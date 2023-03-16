#include <Arduino.h>
#include <PubSubClient.h>
#include "myFunctions.cpp"
#include <iostream>
using namespace std;

#include "MyStone.h"
MyStone *myStone;

#include <WiFiManager.h>
WiFiManager wm;
#define WEBSERVER_H

//Variable pour la connection Wifi
const char *SSID = "SAC_";
const char *PASSWORD = "sac_";
String ssIDRandom;

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 
char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;
const char *topic = "zigbee2mqtt";

//Définition des trois leds de statut
#define GPIO_PIN_LED_LOCK_ROUGE 12

//Variable pour vitesse baud Serial
#define BAUD 9600

//Variable pour le code (Pin) entré
std::string code = "1234";

void connectToWiFi() {
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

void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        mqttClient.subscribe(topic);
      }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Valide le message entrant pour la demande du code de l'alarme
  if (String(topic) == "zigbee2mqtt/askCode01") {
    if(messageTemp == "on"){
      Serial.println("Veuillez entrer le PIN code pour arrêter l'alarme!");
      digitalWrite(GPIO_PIN_LED_LOCK_ROUGE, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("Code Ok!");
      digitalWrite(GPIO_PIN_LED_LOCK_ROUGE, LOW);
    }
  }
}

void setup() {
  Serial.begin(9600);
  connectToWiFi();
  setupMQTT();
  pinMode(GPIO_PIN_LED_LOCK_ROUGE, OUTPUT);
}

void loop() {
  if (!mqttClient.connected())
    reconnect();
  mqttClient.loop();
  //Si le bouton "Enter" est peser sur le Stone = Publish le code
  if(){
    if(){
      mqttClient.publish(topic, code.c_str());
    }
    else{
      Serial.print("");
    }
  }
}