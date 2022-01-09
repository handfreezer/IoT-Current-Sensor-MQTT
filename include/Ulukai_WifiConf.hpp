#ifndef ULUKAI_WIFICONF
#define ULUKAI_WIFICONF

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <map>

#define WIFI_CONF_PORTAL_SSID "Yapluka"
#define WIFI_CONF_PORTAL_PWD "12345678"
#define CONFIG_FILE_PATH "/ulukai_config.json"

class Ulukai_WifiConf
{
    public:
        static Ulukai_WifiConf* getInstance();
        bool debug();
        void debug(bool activate);
        bool connect(bool blockOnPortalIfFailed);
        void addParameter(const char* id, const char* title, const char* rule);
        String getParameter(String id);
        void setConfigPortalStartedCallback(std::function<void(String ssid, String pwd)> func);
        void setParamsChangedCallback(std::function<void()> func);
        bool launchConfigPortal(int timeout);
        static void configModeCallback(WiFiManager* myWiFiManager);
        static void saveParamsCallback();
        bool isConnected();

    private:
        Ulukai_WifiConf();
        static Ulukai_WifiConf* instance;
        bool debugMode = false;
        String lastReadParamFromMemory;
        WiFiManager wifiManager;
        std::function<void()> _cbParamsUpdated = NULL;
        std::function<void(String ssid, String pwd)> _cbConfigPortalStarted = NULL;
        String portalSSID = WIFI_CONF_PORTAL_SSID;
        String portalPassphrase = WIFI_CONF_PORTAL_PWD;
        String readParamsFromMemory(String keyname);
        void _configModeCallback(WiFiManager* myWiFiManager);
        void _saveParamsCallback();
};

#endif
