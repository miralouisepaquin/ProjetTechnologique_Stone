#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
#include <PubSubClient.h>
#include "myFunctions.cpp"
#include <iostream>
using namespace std;

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

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ip";
const char* PARAM_INPUT_2 = "dns";
const char* PARAM_INPUT_3 = "gateway";
const char* PARAM_INPUT_4 = "subnet";
const char* PARAM_INPUT_5 = "ssid";
const char* PARAM_INPUT_6 = "pass";


//Variables to save values from HTML form
String ip;
String dns;
String gateway;
String subnet;
String ssid;
String pass;

// File paths to save input values permanently
const char* dnsPath = "/dns.txt";
const char* subnetPath = "/subnet.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";

IPAddress localIP;
IPAddress localDNS;
IPAddress localGateway;
IPAddress localSubnet;

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Read File from SPIFFS
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }
  
  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;     
  }
  return fileContent;
}

// Write file to SPIFFS
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

// Initialize WiFi
bool initWiFi() {
  if(ssid=="" || ip==""){
    Serial.println("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip.c_str());
  localDNS.fromString(dns.c_str());
  localSubnet.fromString(subnet.c_str());
  localGateway.fromString(gateway.c_str());


  if (!WiFi.config(localIP, localGateway, localSubnet, localDNS)){
    Serial.println("STA Failed to configure");
    return false;
  }
  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.println("Connecting to WiFi...");

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while(WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println("Failed to connect.");
      return false;
    }
  }

  Serial.println(WiFi.localIP());
  return true;
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
  // Serial port for debugging purposes
  Serial.begin(115200);

  initSPIFFS();
  
  // Load values saved in SPIFFS
  ssid = readFile (SPIFFS, ssidPath);
  pass = readFile (SPIFFS, passPath);
  dns = readFile(SPIFFS, dnsPath);
  subnet = readFile(SPIFFS, subnetPath);
  ip = readFile(SPIFFS, ipPath);
  gateway = readFile (SPIFFS, gatewayPath);
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ip);
  Serial.println(dns);
  Serial.println(gateway);
  Serial.println(subnet);

  if(initWiFi()) {
    setupMQTT();
  }
  else {
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("ESP-WIFI-MANAGER", NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP); 

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/wifimanager.html", "text/html");
    });
    
    server.serveStatic("/", SPIFFS, "/");
    
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ip value
          if (p->name() == PARAM_INPUT_1) {
            ip = p->value().c_str();
            Serial.print("IP set to: ");
            Serial.println(ip);
            // Write file to save value
            writeFile(SPIFFS, ipPath, ip.c_str());
          }
          // HTTP POST dns value
          if (p->name() == PARAM_INPUT_2) {
            dns = p->value().c_str();
            Serial.print("DNS set to: ");
            Serial.println(dns);
            // Write file to save value
            writeFile(SPIFFS, dnsPath, dns.c_str());
          }
          // HTTP POST gateway value
          if (p->name() == PARAM_INPUT_3) {
            gateway = p->value().c_str();
            Serial.print("gateway set to: ");
            Serial.println(gateway);
            // Write file to save value
            writeFile(SPIFFS, gatewayPath, gateway.c_str());
          }
          // HTTP POST subnet value
          if (p->name() == PARAM_INPUT_4) {
            subnet = p->value().c_str();
            Serial.print("subnet set to: ");
            Serial.println(subnet);
            // Write file to save value
            writeFile(SPIFFS, subnetPath, subnet.c_str());
          }
          // HTTP POST ip value
          if (p->name() == PARAM_INPUT_5) {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }// HTTP POST ip value
          if (p->name() == PARAM_INPUT_6) {
            pass = p->value().c_str();
            Serial.print("Pass set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(SPIFFS, passPath, pass.c_str());
          }
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip);
      delay(30);
      ESP.restart();
    });
    server.begin();
  }

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