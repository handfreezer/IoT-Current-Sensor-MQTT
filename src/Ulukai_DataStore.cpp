#include <Ulukai_DataStore.hpp>

Ulukai_DataStore::Ulukai_DataStore() {
    this->revision = 0;
    this->debugMode = false;
    memset(&(this->datetime), 0, sizeof(struct tm));
    memset(&(this->timeStampIrms), 0, sizeof(struct tm));
    memset(&(this->iRms), 0, NB_CANAL_IRMS * sizeof(float));
    this->wifiConnected = false;
    this->wifiQuality = 0;
    this->wifiApSsid = "";
    this->ip = "";
};

void Ulukai_DataStore::debug(bool activate){
    if (true == debugMode){
        if(true == activate)
            Serial.println("Ulukai_DataStore:: Activating debug mode");
        else
            Serial.println("Ulukai_DataStore:: Disabling debug mode");
    }
    debugMode = activate;
    if (true == debugMode)
            Serial.println("Ulukai_DataStore:: debug mode activated");
}

unsigned long Ulukai_DataStore::getRevision() {
    return this->revision;
}

void Ulukai_DataStore::setDateTime(struct tm* timeinfo) {
    if ( NULL == timeinfo) {
        if ( true == debugMode )
            Serial.println("Ulukai_DataStore:: setDateTime with NULL param");
    } else {
        memcpy(&this->datetime, timeinfo, sizeof(struct tm));
        this->revision++;
    }
}

const struct tm * Ulukai_DataStore::getDateTime() {
    return &(this->datetime);
}

void Ulukai_DataStore::setCanalIrms(int canal, float value, struct tm& timestamp) {
    if (! ((1 <= canal) and (canal <= this->getCanalCount))) {
        if ( true == debugMode )
            Serial.println("Ulukai_DataStore:: setCanalIrms with bad canal number");
    } else {
        this->iRms[canal] = value;
        memcpy(&(this->timeStampIrms), &timestamp, sizeof(struct tm));
        this->revision++;
    }
}

float Ulukai_DataStore::getCanalIrms(int canal) {
    float result = 0.;
    if (! ((1 <= canal) and (canal <= this->getCanalCount))) {
        if ( true == debugMode )
            Serial.println("Ulukai_DataStore:: setCanalIrms with bad canal number");
    } else {
        result = this->iRms[canal];
    }
    return result;
}

const struct tm* Ulukai_DataStore::getCanalIrmsTImeStamp() {
    return &(this->timeStampIrms);
}

void Ulukai_DataStore::setWifiInfo(bool connected, String apSsid, int quality, String ip) {
    this->wifiConnected = connected;
    this->wifiQuality = quality;
    this->wifiApSsid = apSsid;
    this->ip = ip;
    this->revision++;
}

bool Ulukai_DataStore::getWifiConnected() {
    return this->wifiConnected;
}

int Ulukai_DataStore::getWifiQuality() {
    return this->wifiQuality;
}

String Ulukai_DataStore::getWifiApSsid() {
    return this->wifiApSsid;
}

String Ulukai_DataStore::getIp() {
    return this->ip;
}
