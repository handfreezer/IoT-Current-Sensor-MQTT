#ifndef ULUKAI_NTP
#define ULUKAI_NTP

#include <Arduino.h>

#define ULUKAI_NTP_NB_NTP_SERVER_MAX    (3)
#define ULUKAI_NTP_SERVER_DEFAULT_01 "0.pool.ntp.org"
#define ULUKAI_NTP_SERVER_DEFAULT_02 "1.pool.ntp.org"
#define ULUKAI_NTP_SERVER_DEFAULT_03 "2.pool.ntp.org"
#define ULUKAI_NTP_GMT_OFFSET_DEFAULT (3600)
#define ULUKAI_NTP_DAYLIGHT_OFFSET_DEFAULT (0)

class Ulukai_Ntp
{
    public:
        Ulukai_Ntp();
        void debug(bool activate);
        void setNtpClient(const char* ntp_server_01, const char* ntp_server_02, const char* ntp_server_03, int dayLightOffset, int gmtOffset);
        void ntpRestart();
        bool getTimeLocal(struct tm* timeinfo);
        static String formatLocalTimeRFC3339(const struct tm* timeinfo);

    private:
        bool debugMode = false;
        char nbNtpServer;
        String ntp_server[ULUKAI_NTP_NB_NTP_SERVER_MAX];
        int ntp_dayLightOffset;
        int ntp_gmtOffset;
        void debugLocalTime(struct tm* timeinfo);
};

#endif
