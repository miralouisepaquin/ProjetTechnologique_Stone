# ProjetTechnologique_Stone
Programmation ESP32 pour utiliser un écran Stone dans le projet d'alarme en projet technologique

## Table of Contents
1. [General Info](#general-info)
2. [Technologies](#technologies)
3. [Installation](#installation)
### General Info
***
Le système utilise la technologie MQTT afin de recevoir et transmettre les informations provenant de l'application et des senseurs. L'application utilise un API Rest sur un serveur distant avec une base de donnée en MongoDB. Les senseurs quand à eux utilisent du language Zigbee pour communiquer avec le Broker.
## Technologies
***
Voici la liste des technologies utilisées dans le projet:
* [C++](https://cplusplus.com/doc/tutorial/)
* [Mosquitto](https://mosquitto.org/): Version 5.0
* [API Rest](https://www.bezkoder.com/node-express-mongodb-crud-rest-api/): Version 4.18.1
* [me-no-dev/ESP Async WebServer]: Version 1.2.3
* [knolleary/PubSubClient@^2.8]: Version 2.8
* [WifiManager]: Version 2.0.4
## Installation
***
Les librairies sont déjà toutes dans le projets. Il suffit de s'assurer d'avoir un ESP32 pour utiliser le programme.
Voici les branchements à faire pour pouvoir utiliser le programme tel quel:

GPI0 13: fil pour bouton
GPI0 23 : Leds Rouge
