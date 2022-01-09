#include <Ulukai_IRMS_ADS1015.hpp>

Ulukai_IRMS_ADS1015::Ulukai_IRMS_ADS1015(int calCanal1) {
    this->nbCanal = 1;
    calibrationCanal[0] = calCanal1;
}

Ulukai_IRMS_ADS1015::Ulukai_IRMS_ADS1015(int calCanal1,int calCanal2) {
    this->nbCanal = 2;
    calibrationCanal[0] = calCanal1;
    calibrationCanal[1] = calCanal2;
}

Ulukai_IRMS_ADS1015::Ulukai_IRMS_ADS1015(int calCanal1,int calCanal2,int calCanal3) {
    this->nbCanal = 3;
    calibrationCanal[0] = calCanal1;
    calibrationCanal[1] = calCanal2;
    calibrationCanal[2] = calCanal3;
}

Ulukai_IRMS_ADS1015::Ulukai_IRMS_ADS1015(int calCanal1,int calCanal2,int calCanal3,int calCanal4) {
    this->nbCanal = 4;
    calibrationCanal[0] = calCanal1;
    calibrationCanal[1] = calCanal2;
    calibrationCanal[2] = calCanal3;
    calibrationCanal[3] = calCanal4;
}

bool Ulukai_IRMS_ADS1015::setCalibrationCanal(int index, int calCanal) {
    bool result = false;
    if ( (0 >= index) || (this->nbCanal < index) )
    {
        Serial.println("Ulukai_IRMS_ADS1015 : set canal calibration index invalid");
        Serial.print("nbCanal = ");Serial.println(this->nbCanal);
        Serial.print("index = ");Serial.println(index);
    } else {
        if ( calCanal <= 0 ) {
            Serial.println("Ulukai_IRMS_ADS1015 : set canal calibration value invalid");
            Serial.print("index = ");Serial.println(index);
            Serial.print("calibration value = ");Serial.println(calCanal);
        } else {
            this->calibrationCanal[index-1] = calCanal;
            result = true;
        }
    }
    return result;
}

float Ulukai_IRMS_ADS1015::readCanal(int index){
    Adafruit_ADS1015 *ads;
    float result = 0.;
    if (true != initDone){
        Serial.println("Ulukai_IRMS_ADS1015 : init NOT done");
        result = 0.;
    } else if ( (0 >= index) || (this->nbCanal < index) )
    {
        Serial.println("Ulukai_IRMS_ADS1015 : read canal index invalid");
        Serial.print("nbCanal = ");Serial.println(this->nbCanal);
        Serial.print("index = ");Serial.println(index);
        result = 0.;
    } else {
        Serial.print("Ulukai_IRMS_ADS1015 : read canal index = ");Serial.println(index);
        ads = canal[index - 1];
        int pin = (index - 1) % 2;

        int16_t adc;
        double sumI = 0.0, sqI = 0.0;
        unsigned int nbSample = 0;
        unsigned int endTime = millis() + 100; // 5*20ms => 5sinus
        
        while ( millis() < endTime ) {
            if (0 == pin)
                adc = ads->readADC_Differential_0_1();
            else
                adc = ads->readADC_Differential_2_3();
            sqI = adc * adc;
            sumI += sqI;
            nbSample++;
        }
        adc = sqrt(sumI / nbSample);
        float voltage = adc * 0.0005;
        float current = voltage * calibrationCanal[index - 1];
        if (debugMode) {
            Serial.print("readCurrent pin = "); Serial.println(index);
            Serial.print("Differential square/root: "); Serial.print(adc); Serial.print(" ("); Serial.print(voltage); Serial.print("mV - sample : "); Serial.print(nbSample); Serial.println(")");
            Serial.print("Current: "); Serial.print(current); Serial.println("A");
        }
        result = current;
    }
    return result;
}

void Ulukai_IRMS_ADS1015::debug(bool activate){
    if (true == debugMode){
        if(true == activate)
            Serial.println("Ulukai_IRMS_ADS1015:: Activating debug mode");
        else
            Serial.println("Ulukai_IRMS_ADS1015:: Disabling debug mode");
    }
    debugMode = activate;
    if (true == debugMode)
            Serial.println("Ulukai_IRMS_ADS1015:: debug mode activated");
}

bool Ulukai_IRMS_ADS1015::initADS(){
    for(int i=0;i<MAX_IRMS_NB_CANAL;i++)
        canal[i] = NULL;
    if (1 <= this->nbCanal) {
        canal[0] = new Adafruit_ADS1015();
        canal[0]->begin(adrI2CBase);
        canal[0]->setGain(GAIN_FOUR);
    }
    if (2 <= this->nbCanal)
        canal[1] = canal[0];
    if (3 <= this->nbCanal) {
        canal[2] = new Adafruit_ADS1015();
        canal[2]->begin(adrI2CBase+1);
        canal[2]->setGain(GAIN_FOUR);
    }
    if (4 <= this->nbCanal)
        canal[3] = canal[2];
    initDone = true;
    
    if(debugMode){
        Serial.print("Ulukai_IRMS_ADS1015:: Result of initADS: "); Serial.println(initDone);
    }
    return initDone; //TODO check if all went good or not...
}