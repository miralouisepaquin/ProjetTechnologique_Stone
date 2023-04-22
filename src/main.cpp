#include <Arduino.h>
#include <PubSubClient.h>
#include "myFunctions.cpp"
#include <iostream>
using namespace std;

#include <WiFiManager.h>
WiFiManager wm;
#define WEBSERVER_H

/*IPAddress ip(172, 16, 23, 100);
IPAddress dns(172, 16, 4, 2);
IPAddress gateway(172, 16, 4, 2);
IPAddress subnet(255, 255, 252, 0);*/

//Variable pour la connection Wifi
const char *SSID = "SAC_";
const char *PASSWORD = "sac_";
String ssIDRandom;

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 
const char *mqttServer = "172.16.5.100";
int mqttPort = 1883;
const char *topic = "zigbee2mqtt";

//Pour la gestion des boutons
#include "MyButton.h"
MyButton *myButtonAction = NULL;
MyButton *myButtonReset = NULL;

//Définition des trois leds de statut
#define GPIO_PIN_LED_LOCK_ROUGE 23

//Deinition des PIN pour la connexion serie
#define RXD2 18
#define TXD2 19
//Variable pour vitesse baud Serial
#define BAUD_RATE 115200

//Variable pour le code (Pin) entré
std::string code = "1234";
//Variable pour le button Enter
bool buttonEnter = 0;
//Variable pour le message du subscribe
String messageTemp;

void connectToWiFi() {
  String ssIDRandom, PASSRandom;
    String stringRandom;
    stringRandom = get_random_string(4).c_str();
    ssIDRandom = SSID;
    ssIDRandom = ssIDRandom + stringRandom;
    stringRandom = get_random_string(4).c_str();
    PASSRandom = PASSWORD;
    PASSRandom = PASSRandom + stringRandom;

    char strToPrint[128];
    sprintf(strToPrint, "Identification : %s   MotDePasse: %s", ssIDRandom, PASSRandom);
    Serial.println(strToPrint);

    //WiFi.config(ip, gateway, subnet, dns);
    if (!wm.autoConnect(ssIDRandom.c_str(), PASSRandom.c_str())){
      Serial.println("Erreur de connexion.");
      }
    else {
      Serial.println("Connexion Établie:");
      }
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
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
    }

  // Valide le message entrant pour la demande du code de l'alarme
  if (strcmp(topic,"zigbee2mqtt")==0) {
    delay(100);
    if(strcmp(messageTemp.c_str(),"askCode01")==0 && digitalRead(GPIO_PIN_LED_LOCK_ROUGE)==false){
      Serial.println("\nVeuillez entrer le PIN code pour arrêter l'alarme!");
      digitalWrite(GPIO_PIN_LED_LOCK_ROUGE, HIGH);
    }
  }
}

void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);
}

void setup() {
  Serial.begin(9600);
  connectToWiFi();
  setupMQTT();

  //Gestion des boutons
  myButtonAction = new MyButton();        //Pour lire le bouton actions
  myButtonAction->init(T4);
  int sensibilisationButtonAction = myButtonAction->autoSensibilisation();

  Serial.print("sensibilisationButtonAction : "); Serial.println(sensibilisationButtonAction);

  pinMode(GPIO_PIN_LED_LOCK_ROUGE, OUTPUT);
}

void loop() {
  if (!mqttClient.connected()){
    reconnect();    
    }    
  mqttClient.loop();

  //Gestion du bouton Action (remplace le enter du Stone)
  int buttonAction = myButtonAction->checkMyButton();
  if(buttonAction > 2  && digitalRead(GPIO_PIN_LED_LOCK_ROUGE)==true)  {  //Si appuyé plus de 0.2 secondes
      buttonEnter = 1;
      }

  delay(100);
  //Si le bouton "Enter" est peser sur le Stone = Publish le code
  if(buttonEnter == 1){
    mqttClient.publish("zigbee2mqtt/code01", code.c_str());
    digitalWrite(GPIO_PIN_LED_LOCK_ROUGE, LOW);
    buttonEnter = 0;
  }

  messageTemp="";
  delay(10);
}