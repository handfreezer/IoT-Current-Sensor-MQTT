#include <Ulukai_DataExchange.hpp>
#include <Ulukai_WifiConf.hpp>

void Ulukai_DataExchange::initVars() {
    this->lastRevisionSent = 0;
    this->mqttServer = "";
    this->mqttPort = 0;
    this->mqttUsername = "";
    this->mqttPassword = "";
    this->deviceID = "";
    this->mqttClient.setClient(wifiClient);
}

Ulukai_DataExchange::Ulukai_DataExchange() {
    this->initVars();
};

void Ulukai_DataExchange::debug(bool activate){
    if (true == debugMode){
        if(true == activate) Serial.println("Ulukai_DataExchange:: Activating debug mode");
        else Serial.println("Ulukai_DataExchange:: Disabling debug mode");
    }
    debugMode = activate;
    if (true == debugMode) Serial.println("Ulukai_DataExchange:: debug mode activated");
}

void Ulukai_DataExchange::debugConfig(bool secrets) {
    Serial.print("Ulukai_DataExchange:debugConfig:mqttserver "); Serial.println(this->mqttServer);
    Serial.print("Ulukai_DataExchange:debugConfig:mqttport "); Serial.println(this->mqttPort);
    Serial.print("Ulukai_DataExchange:debugConfig:mqttdeviceid "); Serial.println(this->deviceID);
    Serial.print("Ulukai_DataExchange:debugConfig:mqttusername "); Serial.println(this->mqttUsername);
    if ( true == secrets) {
        Serial.print("Ulukai_DataExchange:debugConfig:mqttpassword "); Serial.println(this->mqttPassword);
    }
}

void Ulukai_DataExchange::setMqttServer(String mqttServer, unsigned int mqttPort) {
    this->mqttServer = mqttServer;
    this->mqttPort = mqttPort;
    Serial.print("Ulukai_DataExchange:set:mqttserver "); Serial.println(this->mqttServer);
    Serial.print("Ulukai_DataExchange:set:mqttport "); Serial.println(this->mqttPort);
}

void Ulukai_DataExchange::setDeviceId(String deviceID) {
    this->deviceID = deviceID;
    Serial.print("Ulukai_DataExchange:set:deviceid "); Serial.println(this->deviceID);
}

void Ulukai_DataExchange::setCredentials(String mqttUsername, String mqttPassword) {
    this->mqttUsername = mqttUsername;
    this->mqttPassword = mqttPassword;
    Serial.print("Ulukai_DataExchange:set:mqttusername "); Serial.println(this->mqttUsername);
    Serial.print("Ulukai_DataExchange:set:mqttpassword "); Serial.println(this->mqttPassword);
}

bool Ulukai_DataExchange::isConnected() {
    return this->mqttClient.connected();
}

bool Ulukai_DataExchange::reconnect() {
    bool result = false;
    bool connected = false;
    if (true == debugMode) Serial.println("Ulukai_DataExchange: debut reconnect");
    if ( false == Ulukai_WifiConf::getInstance()->isConnected() ) {
        if (true == debugMode) Serial.println("Ulukai_DataExchange: no LAN connected");
    } else {
        if (true == debugMode) Serial.println("Ulukai_DataExchange: LAN connected");
        if (this->mqttClient.connected()) {
            if (true == debugMode) Serial.println("Ulukai_DataExchange: mqtt already connected");
            this->mqttClient.disconnect();
        }
        if (true == debugMode) {
            Serial.println("Ulukai_DataExchange: set mqtt server");
            Serial.print("Ulukai_DataExchange:mqttserver "); Serial.println(this->mqttServer.c_str());
            Serial.print("Ulukai_DataExchange:mqttport "); Serial.println(this->mqttPort);
        }
        this->mqttClient.setServer(this->mqttServer.c_str(), this->mqttPort);
        if (true == debugMode) Serial.println("Ulukai_DataExchange: mqtt server and port done");
        if ( 0 == this->mqttUsername.compareTo("") ) {
            if (true == debugMode) Serial.println("Ulukai_DataExchange: mqtt deviceid only");
            connected = this->mqttClient.connect(this->deviceID.c_str());
        } else {
            if (true == debugMode) Serial.println("Ulukai_DataExchange: mqtt deviceid and credentials");
            connected = this->mqttClient.connect(this->deviceID.c_str(), this->mqttUsername.c_str(), this->mqttPassword.c_str());
        }
        if (true == debugMode) Serial.println("Ulukai_DataExchange: mqtt deviceid and cred done");
        if (! connected ) {
            if (true == debugMode) Serial.println("setup: failed to connect to MQTT server");
        } else {
            result = true;
        }
    }
    return result;
}

bool Ulukai_DataExchange::publishData(Ulukai_DataStore& ds, bool alwaysRefresh) {
    bool result = false;
    if ( true != this->mqttClient.connected()) {
        if (true == debugMode) Serial.println("Ulukai_DataExchange: can't publish, not connected");
    } else {
        result = true;
        if ( (true == alwaysRefresh) || (this->lastRevisionSent < ds.getRevision()) ) {
            this->lastRevisionSent = ds.getRevision();
            String topic = ULUKAI_MQTT_ROOT + this->deviceID + "/current";
            String json = "{";
            for(int i=1; i<=ds.getCanalCount; i++) {
                if ( 1 < i ) json += ",";
                json += "\"canal_" + String(i) + "\": " + String(ds.getCanalIrms(i));
            }
            json += "}";
            result = this->mqttClient.publish(topic.c_str(), json.c_str());
        }
    }
    return result;
}
