# IOT : Current Sensor based on ESP32 / SCT013 / ADS1015 (No calibration needed!)

## **Features**:

* 4 independant current sensors
* Web Portal configuration through WiFi
* Send data to a MQTT server (with authentication, no SSL yet)
* 2 OLED screens to display information (one internal, and one external, not mandatory)
* Frequency of reading configurable
* Time is set through NTP server
* no calibration needed : you just have to set the nominal current size of your SCT013 (through web portal)
* RAIL DIN mounting ! (see photos)

## It is based on **simple components**:

* ESP32 (but should work with ESP8266 too)
* ADC ADS1015
* Non intrusive current sensor SCT013 (should be a Current > 1V conversion)
    * if not, you have to choose a R which cover the range of -1V..+1V of the nominal current of your SCT)

## **Technical notes**:

* at the beginning, I tried to use the analog converter of ESP32 but it is useless because of 2 points:
    * you need a reference value of the voltage measured on the SCT, which means adding composant on schematic and calibration du to variation of each one
    * because speed of the ADC of the ESP is too slow : using the Irms algorithm, you have to get a lot of points on one sinus, BUT as it is too slow, you have to get a lot of value on many sinus and so blocking one thread of your ESP for that.
* To get more detailed on source, you can ask me a private access to my private GIT repository through MP (https://private.ask.me/gogs).

## **License**

### The only usage allowed is for personnal use, at your own risk! No reproduction or modification without creating a pushing request to update this repository.

Any other usage HAVE to be granted by a writing letter of me.

### **Keywords**
iot current sensor esp32 mqtt wifi domotic esp32 esp 32 sct013 sct 013 ads1015 ads 1015 oled ssd1306 ssd 1306 arduino platformio platform io wifimanager wifi manager config portal jeedom domoticz homeassistant