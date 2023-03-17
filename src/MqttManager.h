#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <Arduino.h>
#include <string>

class MqttManager {
    private:
       
    public:
        MqttManager();
        ~MqttManager() { };

        void setupMQTT();
        void reconnect();
        void callback();
};
#endif