#include <Arduino.h>
#include <Wire.h>
#include <Ulukai_IRMS_ADS1015.hpp>
#include <Ulukai_DataStore.hpp>
#include <Ulukai_DisplayDualOled.hpp>
#include <Ulukai_DataExchange.hpp>
#include <Ulukai_WifiConf.hpp>
#include <Ulukai_Ntp.hpp>

#define DEBUG true
#define LOOP_DELAY_SECONDS  (1)
#define LOOP_DELAY_MILLIS   (1000 * LOOP_DELAY_SECONDS)
unsigned long timeLastLoop = 0;

#define READING_IRMS_FREQ_SEC_DEFAULT (30)
unsigned int readingIrmsFrequencySec = READING_IRMS_FREQ_SEC_DEFAULT;

Ulukai_IRMS_ADS1015 currentReader(60,60,50,50);
Ulukai_DataStore datastore;
Ulukai_DisplayDualOled oleds;
Ulukai_DataExchange dataexchange;
Ulukai_WifiConf* wifiConf = NULL;
Ulukai_Ntp ntpClient;

bool loopFirstEntry = true;
bool buttonEvent = false;

#if DEBUG == true
  #define f_debug(a)  (Serial.println(a))
#else
  #define f_debug(a)  (true)
#endif

void IRAM_ATTR buttonResetEvent() {
  f_debug("button Reset Event");
  buttonEvent = true;
  return;
}

void connectDataExchange() {
  readingIrmsFrequencySec = wifiConf->getParameter("irms_read_freq_sec").toInt();
  if ( readingIrmsFrequencySec <= 0 ) readingIrmsFrequencySec = READING_IRMS_FREQ_SEC_DEFAULT;
  currentReader.setCalibrationCanal(1, wifiConf->getParameter("irms_cal_1").toInt());
  currentReader.setCalibrationCanal(2, wifiConf->getParameter("irms_cal_2").toInt());
  currentReader.setCalibrationCanal(3, wifiConf->getParameter("irms_cal_3").toInt());
  currentReader.setCalibrationCanal(4, wifiConf->getParameter("irms_cal_4").toInt());
  dataexchange.setMqttServer(wifiConf->getParameter("mqtt_server"), atoi(wifiConf->getParameter("mqtt_port").c_str()));
  dataexchange.setDeviceId(wifiConf->getParameter("mqtt_deviceid"));
  dataexchange.setCredentials(wifiConf->getParameter("mqtt_username"), wifiConf->getParameter("mqtt_password"));
  dataexchange.reconnect();
}

void paramsUpdatedCallback() {
  connectDataExchange();
}

void configPortalStartedCallback(String ssid, String passphrase) {
  oleds.modeWifiConfigPortal(ssid, passphrase, WiFi.softAPIP().toString());
}

void setup_wm() {
  wifiConf = Ulukai_WifiConf::getInstance();
  wifiConf->setConfigPortalStartedCallback(configPortalStartedCallback);
  wifiConf->setParamsChangedCallback(paramsUpdatedCallback);
  wifiConf->addParameter("mqtt_server", "MQTT server", "");
  wifiConf->addParameter("mqtt_port", "MQTT port", "");
  wifiConf->addParameter("mqtt_deviceid", "MQTT DeviceID", "");
  wifiConf->addParameter("mqtt_username", "MQTT Username", "");
  wifiConf->addParameter("mqtt_password", "MQTT Password", "");
  wifiConf->addParameter("irms_read_freq_sec", "Irms Read Freq. (sec):", "");    
  wifiConf->addParameter("irms_cal_1", "Irms Max Canal 1:", "");    
  wifiConf->addParameter("irms_cal_2", "Irms Max Canal 2:", "");    
  wifiConf->addParameter("irms_cal_3", "Irms Max Canal 3:", "");    
  wifiConf->addParameter("irms_cal_4", "Irms Max Canal 4:", "");    
  wifiConf->connect(true);
  datastore.setWifiInfo(WiFi.isConnected(), WiFi.SSID(), WiFi.RSSI(), WiFi.localIP().toString());
}

void setup() {
#if DEBUG == true
  Serial.begin(115200);
#endif 
  f_debug("Init Wire");
  // i2c for the OLED panel
  //Wire.begin(5, 4); 
  Wire.begin(21,22);

  f_debug("Init display");
  oleds.begin();

  f_debug("Init wifi");
  setup_wm();
  f_debug(WiFi.localIP().toString());

  if ( true == WiFi.isConnected() )
    ntpClient.ntpRestart();

  f_debug("Set interrupt on GPIO15 (pin 22)");
  pinMode(15,INPUT);
  attachInterrupt(15, buttonResetEvent, RISING);
  f_debug("Set interrupt on GPIO15 (pin 22) done");

  f_debug("Init currentReader");
  currentReader.initADS();
  f_debug("Init currentReader done");

  f_debug("Init mqtt");
  connectDataExchange();
  f_debug("Init mqtt done");

  timeLastLoop = 0;
}

void loop() {
  timeLastLoop = millis();
  unsigned long seconds = timeLastLoop / LOOP_DELAY_MILLIS;
  Serial.print("seconds() : "); Serial.println(seconds);
  struct tm timeinfo;
  memset(&timeinfo, 0, sizeof(struct tm));
  if ( (true == loopFirstEntry) || ( 0 == (seconds % readingIrmsFrequencySec)) ) {
    f_debug("updating localtime to DS");
    ntpClient.debug(true);
    if(!ntpClient.getTimeLocal(&timeinfo)){
      f_debug("Failed to obtain time");
    } else {
      datastore.setDateTime(&timeinfo);
    }
    ntpClient.debug(false);
    f_debug("updating irms to DS");
    for(int i=0;i<4;i++){
      float current = currentReader.readCanal(i+1);
      datastore.setCanalIrms(i+1, current, timeinfo);
      Serial.println(current);
    }
  }
  f_debug("Publishing data");
  if ( false == dataexchange.publishData(datastore, false) )
  {
    f_debug("Failed to publish data");
  }

  f_debug("Testing event Button");
  if ( true == buttonEvent ) {
    f_debug("button Reset Event in loop detected");
    buttonEvent = false;
    oleds.drawButtonEvent();
    wifiConf->launchConfigPortal(60);
    datastore.setWifiInfo(WiFi.isConnected(), WiFi.SSID(), WiFi.RSSI(), WiFi.localIP().toString());
  }

  Serial.print("Datastore revision : "); Serial.println(datastore.getRevision());
  f_debug("refreshing OLEDs");
  oleds.refresh(datastore);

  f_debug("Pause about 1sec");
  while(true) {
    unsigned long current_millis = millis();
    if (((LOOP_DELAY_MILLIS + timeLastLoop) <= current_millis )
        || (current_millis < timeLastLoop )) {
          f_debug("New loop have to be done");
          break;
        }
    delay(1);
  }
  if ( true == loopFirstEntry ) {
    loopFirstEntry = false;
    f_debug("first Entry Loop done");
  }
}