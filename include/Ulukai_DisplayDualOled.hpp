#ifndef ULUKAI_DISPLAY_DUAL_OLED
#define ULUKAI_DISPLAY_DUAL_OLED

#include <Arduino.h>
#include <U8g2lib.h>
#include <qrcode.h>
#include <Ulukai_DataStore.hpp>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_EXT_I2C_ADDR 0x3C
#define OLED_INT_I2C_ADDR 0x3D

#define FONT_ICONS u8g2_font_siji_t_6x10
#define FONT_TEXT u8g2_font_5x7_tr

class Ulukai_DisplayDualOled
{
    public:
        Ulukai_DisplayDualOled();
        void debug(bool activate);
        bool begin();
        bool refresh(Ulukai_DataStore& ds);
        void modeWifiConfigPortal(String configPortalSSID, String configPortalPwd, String configPortalIP);
        void drawQRCode(U8G2* oled, String msg);
        void drawQRCodeWifi(String ssid, String passphrase);
        void drawButtonEvent();
    private:
        bool debugMode = false;
        U8G2_SSD1306_128X64_NONAME_F_HW_I2C* oledExt;
        U8G2_SSD1306_128X64_NONAME_F_HW_I2C* oledInt;
        unsigned int lastDsRevisionDisplayed;
        void oledBegin(U8G2* oled);
        void drawModeWifiConfigPortal(U8G2* oled, String configPortalSSID, String configPortalPwd, String configPortalIP);
        void drawModeIrmsValues(U8G2* oled, Ulukai_DataStore* ds);
        void drawButtonEvent(U8G2* oled);
        void drawBootscreen(U8G2* oled);
};

#endif
