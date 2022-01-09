#ifndef ULUKAI_DATASTORE
#define ULUKAI_DATASTORE

#include <Arduino.h>

#define NB_CANAL_IRMS   (4)

class Ulukai_DataStore
{
    public:
        const int getCanalCount = NB_CANAL_IRMS;
        Ulukai_DataStore();
        void debug(bool activate);
        unsigned long getRevision();
        void setDateTime(struct tm* timeinfo);
        const struct tm * getDateTime();
        void setCanalIrms(int canal, float value, struct tm& timestamp);
        float getCanalIrms(int canal);
        const struct tm* getCanalIrmsTImeStamp();
        void setWifiInfo(bool connected, String apSsid, int quality, String ip);
        bool getWifiConnected();
        int getWifiQuality();
        String getWifiApSsid();
        String getIp();

    private:
        bool debugMode = false;
        unsigned int revision = 0;
        struct tm datetime;
        float iRms[NB_CANAL_IRMS];
        struct tm timeStampIrms;
        bool wifiConnected;
        int wifiQuality;
        String wifiApSsid;
        String ip;
};

#endif
