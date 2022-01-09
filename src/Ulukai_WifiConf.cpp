#include <Ulukai_WifiConf.hpp>

Ulukai_WifiConf* Ulukai_WifiConf::instance = NULL;
Ulukai_WifiConf* Ulukai_WifiConf::getInstance() {
    if (NULL == Ulukai_WifiConf::instance) {
        Ulukai_WifiConf::instance = new Ulukai_WifiConf();
    }
    return Ulukai_WifiConf::instance;
}

Ulukai_WifiConf::Ulukai_WifiConf() {
    this->portalSSID = WIFI_CONF_PORTAL_SSID + String("-") + WiFi.macAddress();
    this->portalSSID.replace(":","");
    this->portalPassphrase = WiFi.macAddress().substring(WiFi.macAddress().length() - 8);

    this->wifiManager.setDebugOutput(true);
    std::vector<const char *> menu = {"wifi","wifinoscan","info","param","close","sep","erase","update","restart","exit"};
    wifiManager.setMenu(menu);
    wifiManager.setScanDispPerc(true); // display percentages instead of graphs for RSSI
    wifiManager.setDarkMode(false); 
    wifiManager.setCaptivePortalEnable(true);
    wifiManager.setBreakAfterConfig(true);
    wifiManager.setAPCallback(Ulukai_WifiConf::configModeCallback);
    wifiManager.setSaveParamsCallback(Ulukai_WifiConf::saveParamsCallback);

    if (true == debugMode) {
        Serial.println("Ulukai_WifiConf:: Wifi Portal Credentials");
        Serial.print("Ulukai_WifiConf:: SSID= ");
        Serial.println(portalSSID.c_str());
        Serial.print("Ulukai_WifiConf::  PWD= ");
        Serial.println(portalPassphrase.c_str());
    }
};

bool Ulukai_WifiConf::debug() {
    return this->debugMode;
}

void Ulukai_WifiConf::debug(bool activate){
    if (true == debugMode){
        if(true == activate) Serial.println("Ulukai_WifiConf:: Activating debug mode");
        else Serial.println("Ulukai_WifiConf:: Disabling debug mode");
    }
    debugMode = activate;
    //this->wifiManager.setDebugOutput(this->debugMode);
    if (true == debugMode) Serial.println("Ulukai_WifiConf:: debug mode activated");
}

void Ulukai_WifiConf::setParamsChangedCallback(std::function<void()> func) {
    this->_cbParamsUpdated = func;
}

void Ulukai_WifiConf::setConfigPortalStartedCallback(std::function<void(String ssid, String pwd)> func) {
    this->_cbConfigPortalStarted = func;
}

void Ulukai_WifiConf::configModeCallback(WiFiManager* myWiFiManager) {
    Ulukai_WifiConf::getInstance()->_configModeCallback(myWiFiManager);
}

void Ulukai_WifiConf::_configModeCallback(WiFiManager* myWiFiManager) {
    //wifi_config_t conf_current;
    if (true == this->debug()) Serial.println("Ulukai_WifiConf:: WifiManager Config Portal Started (callback)");
    //esp_wifi_get_config(WIFI_IF_AP, &conf_current);
    if (NULL != this->_cbConfigPortalStarted) this->_cbConfigPortalStarted(this->portalSSID, this->portalPassphrase);
    //oleds.modeWifiConfigPortal(myWiFiManager->getConfigPortalSSID(), String(((const char*)conf_current.ap.password)), WiFi.softAPIP().toString());
}

void Ulukai_WifiConf::saveParamsCallback(){
    Ulukai_WifiConf::getInstance()->_saveParamsCallback();
}

void Ulukai_WifiConf::_saveParamsCallback() {
    if (true == this->debug()) Serial.println("Ulukai_WifiConf:: dsaving config");
    DynamicJsonDocument doc(1024);
    int paramsCount = this->wifiManager.getParametersCount();
    WiFiManagerParameter** paramsList = this->wifiManager.getParameters(); 
    for(int i=0; i<paramsCount; i++) {
            doc[paramsList[i]->getID()] = paramsList[i]->getValue();
    }
    File configFile = SPIFFS.open(CONFIG_FILE_PATH, "w");
    if (!configFile) {
        if (true == this->debug()) Serial.println("Ulukai_WifiConf:: failed to open config file for writing");
    } else {
        serializeJson(doc, Serial);
        serializeJson(doc, configFile);
        configFile.close();
    }
    if (NULL != this->_cbParamsUpdated) this->_cbParamsUpdated();
}

void Ulukai_WifiConf::addParameter(const char* id, const char* title, const char* rule) {
    String value = this->readParamsFromMemory(String(id));
    this->wifiManager.addParameter(new WiFiManagerParameter(id, title, value.c_str(), 50));
    if (true == this->debug()) {
        Serial.print("Ulukai_WifiConf::addparameter : ");
        Serial.print(id); Serial.print(" - ");Serial.println(value.c_str());
    }
}

String Ulukai_WifiConf::getParameter(String id) {
    String result = "";
    int paramsCount = this->wifiManager.getParametersCount();
    if (true == this->debug()) {
        Serial.print("Ulukai_WifiConf::getparameter : count=");
        Serial.println(paramsCount);
    }
    WiFiManagerParameter** paramsList = this->wifiManager.getParameters(); 
    for(int i=0; i<paramsCount; i++) {
        if (true == this->debug()) {
            Serial.print("Ulukai_WifiConf::getparameter : enum : ");
            Serial.print(paramsList[i]->getID()); Serial.print(" - ");Serial.println(paramsList[i]->getValue());
        }
        if ( 0 == id.compareTo(paramsList[i]->getID()) ) {
            result = paramsList[i]->getValue();
            break;
        }
    }
    if (true == this->debug()) {
        Serial.print("Ulukai_WifiConf::getparameter : ");
        Serial.print(id); Serial.print(" - ");Serial.println(result);
    }
    return result;
}

String Ulukai_WifiConf::readParamsFromMemory(String keyname) {
    this->lastReadParamFromMemory = "";
    if (true == debugMode) Serial.println("Ulukai_WifiConf:: WifiManager Config Portal Started (callback)");
    if (! SPIFFS.begin()) {
        if (true == debugMode) Serial.println("Ulukai_WifiConf:: failed to mount SPIFFS");
        SPIFFS.format();
    }
    if (! SPIFFS.begin()) {
        if (true == debugMode) Serial.println("Ulukai_WifiConf:: failed to mount SPIFFS");
    } else {
        if (true == debugMode) Serial.println("Ulukai_WifiConf:: mounted SPIFFS");
        if (! SPIFFS.exists(CONFIG_FILE_PATH)) {
            if (true == debugMode) Serial.println("Ulukai_WifiConf:: no config");
        } else {
            if (true == debugMode) Serial.println("Ulukai_WifiConf:: reading file");
            File configFile = SPIFFS.open(CONFIG_FILE_PATH, "r");
            if (! configFile) {
                if (true == debugMode) Serial.println("Ulukai_WifiConf:: failed to open file");
            } else {
                if (true == debugMode) Serial.println("Ulukai_WifiConf:: opened config file");
                size_t size = configFile.size();
                // Allocate a buffer to store contents of the file.
                std::unique_ptr<char[]> buf(new char[size]);
                configFile.readBytes(buf.get(), size);
                DynamicJsonDocument json(2048);
                auto deserializeError = deserializeJson(json, buf.get());
                serializeJson(json, Serial);
                if ( deserializeError ) {
                    if (true == debugMode) Serial.println("Ulukai_WifiConf:: failed to load json config");
                } else {
                    if (true == debugMode) Serial.println("Ulukai_WifiConf:: parsed json");
                    const char* value = json[keyname];
                    if ( NULL != value ) this->lastReadParamFromMemory = value;
                }
                configFile.close();
            }
        }
    }
    return this->lastReadParamFromMemory;
}

bool Ulukai_WifiConf::launchConfigPortal(int timeout) {
    this->wifiManager.setConfigPortalTimeout(timeout);
    this->wifiManager.setCaptivePortalEnable(true);
    return this->wifiManager.startConfigPortal(this->portalSSID.c_str(), this->portalPassphrase.c_str());
}

bool Ulukai_WifiConf::connect(bool blockOnPortalIfFailed) {
    this->wifiManager.setCaptivePortalEnable(blockOnPortalIfFailed);
    return this->wifiManager.autoConnect(this->portalSSID.c_str(), this->portalPassphrase.c_str());
}

bool Ulukai_WifiConf::isConnected() {
    return WiFi.isConnected();
}

