#include <Ulukai_Ntp.hpp>

Ulukai_Ntp::Ulukai_Ntp() {
    this->nbNtpServer = 0;
    this->setNtpClient(ULUKAI_NTP_SERVER_DEFAULT_01,
        ULUKAI_NTP_SERVER_DEFAULT_02,
        ULUKAI_NTP_SERVER_DEFAULT_03,
        ULUKAI_NTP_DAYLIGHT_OFFSET_DEFAULT,
        ULUKAI_NTP_GMT_OFFSET_DEFAULT);
}

void Ulukai_Ntp::debug(bool activate){
    if (true == debugMode){
        if(true == activate)
            Serial.println("Ulukai_Ntp:: Activating debug mode");
        else
            Serial.println("Ulukai_Ntp:: Disabling debug mode");
    }
    debugMode = activate;
    if (true == debugMode) Serial.println("Ulukai_Ntp:: debug mode activated");
}

void Ulukai_Ntp::setNtpClient(const char* ntp_server_01, const char* ntp_server_02, const char* ntp_server_03, int dayLightOffset, int gmtOffset) {
    this->nbNtpServer = 3;
    this->ntp_server[0] = ntp_server_01;
    this->ntp_server[1] = ntp_server_02;
    this->ntp_server[2] = ntp_server_03;
    this->ntp_dayLightOffset = dayLightOffset;
    this->ntp_gmtOffset = gmtOffset;
    this->ntpRestart();
}

void Ulukai_Ntp::ntpRestart() {
    configTime(this->ntp_gmtOffset,
        this->ntp_dayLightOffset,
        this->ntp_server[0].c_str(),
        this->ntp_server[1].c_str(),
        this->ntp_server[2].c_str());
}

bool Ulukai_Ntp::getTimeLocal(struct tm* timeinfo) {
    bool result = false;
    if ( NULL == timeinfo ) {
        if (true == this->debugMode) Serial.println("Ulukai_Ntp:: timeinfo is NULL");
    } else {
        if (true == this->debugMode) Serial.println("Ulukai_Ntp:: Calling getLocalTime");
        result = getLocalTime(timeinfo, 5000);
        if(! result) {
            if (true == this->debugMode) Serial.println("Ulukai_Ntp:: Failed to obtain time");
        } else {
            if (true == this->debugMode) this->debugLocalTime(timeinfo);
        }
    }
    return result;
}

void Ulukai_Ntp::debugLocalTime(struct tm* timeinfo) {
  Serial.println(timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}

String Ulukai_Ntp::formatLocalTimeRFC3339(const struct tm* timeinfo) {
    char buf[64];
    strftime(buf, 64,"%Y-%m-%dT%H:%M:%S%z", timeinfo);
    return String(buf);
}
