#ifndef ULUKAI_DATAEXCHANGE
#define ULUKAI_DATAEXCHANGE

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Ulukai_DataStore.hpp>

#define ULUKAI_MQTT_ROOT    "ulukai/sensor/"

class Ulukai_DataExchange
{
    public:
        Ulukai_DataExchange();
        void debug(bool activate);
        void debugConfig(bool secrets);
        void setMqttServer(String mqttServer, unsigned int mqttPort);
        void setDeviceId(String deviceID);
        void setCredentials(String mqttUsername, String mqttPassword);
        bool isConnected();
        bool reconnect();
        bool publishData(Ulukai_DataStore& ds, bool alwaysRefresh);

    private:
        bool debugMode = false;
        unsigned int lastRevisionSent;
        String mqttServer;
        unsigned int mqttPort;
        String mqttUsername;
        String mqttPassword;
        String deviceID;
        WiFiClient wifiClient;
        PubSubClient mqttClient;
        void initVars();
};

#endif