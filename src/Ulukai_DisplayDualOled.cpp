#include <Ulukai_DisplayDualOled.hpp>
#include <Ulukai_Ntp.hpp>

Ulukai_DisplayDualOled::Ulukai_DisplayDualOled(){
    this->oledInt = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
    this->oledExt = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
    this->lastDsRevisionDisplayed = 0;
}

void Ulukai_DisplayDualOled::debug(bool activate){
    if (true == debugMode){
        if(true == activate)
            Serial.println("Ulukai_DisplayDualOled:: Activating debug mode");
        else
            Serial.println("Ulukai_DisplayDualOled:: Disabling debug mode");
    }
    debugMode = activate;
    if (true == debugMode)
            Serial.println("Ulukai_DisplayDualOled:: debug mode activated");
}

bool Ulukai_DisplayDualOled::begin() {
    bool result = false;
    this->oledInt->setI2CAddress(OLED_INT_I2C_ADDR * 2);
    oledBegin(this->oledInt);
    this->oledExt->setI2CAddress(OLED_EXT_I2C_ADDR * 2);
    oledBegin(this->oledExt);
    result = true;
    return result;
}

void Ulukai_DisplayDualOled::oledBegin(U8G2* oled) {
  if(!oled->begin()) {
    Serial.println("SSD1306 init failed");
    delay(10*1000);
    ESP.restart();
  } else {
    oled->setFont(FONT_TEXT);
  }

  oled->firstPage();
  oled->setFont(FONT_ICONS);
  oled->drawGlyph(116, 10, 0x0e21a);
  oled->setFont(FONT_TEXT);
  oled->drawStr(0,6,"Hello World!");
  oled->nextPage();
  Serial.println("OLED : Hello World");
  delay(50);
}

bool Ulukai_DisplayDualOled::refresh(Ulukai_DataStore& ds) {
  bool result = false;
  if ( true == debugMode ) Serial.print("Ulukai_DisplayDualOled:: refresh ds pointer is NULL! ");
  if ( this->lastDsRevisionDisplayed != ds.getRevision() ) {
    this->lastDsRevisionDisplayed = ds.getRevision();
    if ( true == debugMode ) Serial.print("Ulukai_DisplayDualOled:: refresh on oledExt");
    this->drawModeIrmsValues(this->oledExt, &ds);
    if ( true == debugMode ) Serial.print("Ulukai_DisplayDualOled:: refresh on oledInt");
    this->drawModeIrmsValues(this->oledInt, &ds);
  }
  result = true;
  return result;
}

void Ulukai_DisplayDualOled::modeWifiConfigPortal(String configPortalSSID, String configPortalPwd, String configPortalIP) {
  this->drawQRCodeWifi(configPortalSSID,configPortalPwd);
  delay(10000);
  this->drawModeWifiConfigPortal(this->oledInt, configPortalSSID, configPortalPwd, configPortalIP);
  this->drawModeWifiConfigPortal(this->oledExt, configPortalSSID, configPortalPwd, configPortalIP);
}

void Ulukai_DisplayDualOled::drawModeIrmsValues(U8G2* oled, Ulukai_DataStore* ds) {
  if ( true == debugMode )
    Serial.println("Ulukai_DisplayDualOled:: switching display to ModeIrmsValues");
  int rssi = ds->getWifiQuality();
  int wifiQuality = 0;
  if(rssi <= -100)
    wifiQuality = 0;
  else if(rssi >= -50)
    wifiQuality = 100;
  else
    wifiQuality = 2 * (rssi + 100);
  oled->clearBuffer();
  oled->drawFrame(0,    0,  128, 64);
  if (true != ds->getWifiConnected()) {
    oled->setCursor(3, 1 + 1*10);
    oled->print("SSID: ");
    oled->print("NOT_CONNECTED");
  } else {
    oled->setCursor(3, 1 + 1*10);
    oled->setFont(FONT_ICONS);
    Serial.print("Ulukai_DisplayDualOled:: wifi quality:");Serial.print(ds->getWifiQuality());
    Serial.print(" - quality: ");Serial.println(wifiQuality);
    if ( (0 < wifiQuality) && (wifiQuality <=25) )
      oled->drawGlyph(3, 1 + 1*10, 0x0e217);
    else if ( (25 < wifiQuality) && (wifiQuality <= 50) )
      oled->drawGlyph(3, 1 + 1*10, 0x0e218);
    else if ( (50 < wifiQuality) && (wifiQuality <= 75) )
      oled->drawGlyph(3, 1 + 1*10, 0x0e219);
    else if ( (75 < wifiQuality) )
      oled->drawGlyph(3, 1 + 1*10, 0x0e21a);
    oled->setFont(FONT_TEXT);
    oled->setCursor(3+12, 1 + 1*10);
    oled->print(" ");
    oled->print(ds->getWifiApSsid());
    oled->print(" (");
    oled->print(ds->getIp());
    oled->print(")");
    oled->setCursor(3, 1 + 2*10);
    oled->print(Ulukai_Ntp::formatLocalTimeRFC3339(ds->getCanalIrmsTImeStamp()));
  }
  for (int i=1; i<=4; i++) {
    oled->setCursor(5, 1 + (2+i)*10 );
    oled->print("Canal "); oled->print(i); oled->print(" : ");
    oled->print(ds->getCanalIrms(i));
  }
  oled->sendBuffer();
}

void Ulukai_DisplayDualOled::drawModeWifiConfigPortal(U8G2* oled, String configPortalSSID, String configPortalPwd, String configPortalIP) {
  if ( true == debugMode )
    Serial.println("Ulukai_DisplayDualOled:: switching display to ModeConfigPortal");
  oled->clearBuffer();
  oled->drawFrame(0,    0,  128, 64);
  oled->setCursor(3, 2 + 1*10);
  oled->print("!= Config Portal Mode =!");
  oled->setCursor(5, 2 + 2*10 );
  oled->print("SSID:");
  oled->setCursor(5, 2 + 3*10 );
  oled->print(configPortalSSID);
  oled->setCursor(5, 2 + 4*10 );
  oled->print("Pwd: ");
  oled->print(configPortalPwd);
  oled->setCursor(5, 2 + 5*10 );
  oled->print("IP: ");
  oled->print(configPortalIP);
  oled->sendBuffer();
}

void Ulukai_DisplayDualOled::drawQRCode(U8G2* oled, String msg) {
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, ECC_LOW, msg.c_str());
  oled->clearBuffer();
  oled->drawBox(0,0,oled->getWidth(), oled->getHeight());
  oled->setDrawColor(2);
  int offset_x = (oled->getWidth() - qrcode.size*2) /2;
  int offset_y = (oled->getHeight() - qrcode.size*2) /2;
  for(int y=0; y<qrcode.size; y++){
    for(int x=0;x<qrcode.size;x++){
      if ( true == qrcode_getModule(&qrcode, x, y)) {
        oled->drawBox(offset_x+x*2,offset_y+y*2,2,2);
      }
    }
  }
  oled->sendBuffer();
}

void Ulukai_DisplayDualOled::drawQRCodeWifi(String ssid, String passphrase) {
  // WIFI:T:WPA;S:NomDuRéseau;P:MonMotDePasse;;
  // avec T pour WPA/WEP/vide
  // avec S pour le SSID
  // avec P pour la passphrase
  // avec H à true pour un SSID caché
  String wifi_str("WIFI:T:WPA;S:"+ssid+";P:"+passphrase+";;");
  this->oledInt->setContrast(128);
  this->drawQRCode(this->oledInt, wifi_str); 
  this->drawQRCode(this->oledExt, wifi_str); 
}

void Ulukai_DisplayDualOled::drawButtonEvent() {
    this->drawButtonEvent(this->oledInt);
    this->drawButtonEvent(this->oledExt);
}

void Ulukai_DisplayDualOled::drawButtonEvent(U8G2* oled) {
  Serial.println("drawButtonEvent begin");
  oled->firstPage();
  oled->setCursor(0, 6);
  oled->print("Reset button");
  oled->print("   pressed !!!");
  oled->nextPage();
  delay(50);
  Serial.println("drawButtonEvent done");
}

void Ulukai_DisplayDualOled::drawBootscreen(U8G2* oled){

  Serial.println("Draw boot screen");

//  oled.firstPage();
  oled->clearBuffer();
  oled->drawBox(0,    0,  5, 5);
  oled->drawBox(62,   0,  5, 5);
  oled->drawBox(123,  0,  5, 5);
  oled->drawBox(62,20,10,5);
  oled->drawBox(62,35,10,5);
  oled->setCursor(0, 17);
  oled->print("Connecting");
  oled->print("   Yapluka !!!");
  Serial.println("Draw boot screen 01");
  oled->sendBuffer();
//  do {delay(1);}while(oled.nextPage());
  delay(2000);

  //oled.clearDisplay();
  oled->firstPage();
  oled->drawBox(0,0,5,5);
  oled->drawBox(0,27,5,5);
  oled->drawBox(0,59,5,5);
  oled->nextPage();
  Serial.println("Draw boot screen 02");
  delay(2000);

  oled->firstPage();
  oled->drawBox(59,0,5,5);
  oled->drawBox(123,0,5,5);
  oled->nextPage();
  Serial.println("Draw boot screen 03");
}
