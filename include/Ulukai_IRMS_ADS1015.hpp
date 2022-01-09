#ifndef ULUKAI_IRMS_ADS1015
#define ULUKAI_IRMS_ADS1015

#include <Adafruit_ADS1X15.h>

#define MAX_IRMS_NB_CANAL  (4)

class Ulukai_IRMS_ADS1015
{
    public:
        Ulukai_IRMS_ADS1015(int calCanal1);
        Ulukai_IRMS_ADS1015(int calCanal1,int calCanal2);
        Ulukai_IRMS_ADS1015(int calCanal1,int calCanal2,int calCanal3);
        Ulukai_IRMS_ADS1015(int calCanal1,int calCanal2,int calCanal3,int calCanal4);
        static const int nbCanalMax = MAX_IRMS_NB_CANAL;
        bool initADS();
        void debug(bool activate);
        bool setCalibrationCanal(int index, int calCanal);
        float readCanal(int index);
    private:
        bool debugMode = false;
        bool initDone = false;
        int nbCanal;
        int adrI2CBase = 0x48;
        int calibrationCanal[MAX_IRMS_NB_CANAL];
        Adafruit_ADS1015* canal[MAX_IRMS_NB_CANAL] = {NULL, NULL, NULL, NULL};
};

#endif
