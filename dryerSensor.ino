// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_MPRLS.h>

/*!
 * @file mprls_simpletest.ino
 *
 * A basic test of the sensor with default settings
 * 
 * Designed specifically to work with the MPRLS sensor from Adafruit
 * ----> https://www.adafruit.com/products/3965
 *
 * These sensors use I2C to communicate, 2 pins (SCL+SDA) are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.  
 *
 * MIT license, all text here must be included in any redistribution.
 *
 */
 

// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin
#define LED D7

Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);

void setup() {
    Serial.begin(115200);
    Time.zone(-6);
    Time.beginDST();
    pinMode(LED, OUTPUT);
  
    Particle.publish("MPRLS Simple Test");
    if (! mpr.begin()) {
        Particle.publish("Failed to communicate with MPRLS sensor, check wiring?");

        while (1) {
            delay(10);
        }
    }
    
    RGB.control(true); 
    RGB.brightness(0);
    Particle.publish("Found MPRLS sensor");

}

unsigned long lastTimeOn = 0;
unsigned long lastTimeOff = 0;
unsigned long lastTimeAlertOff = 0;


bool fanHasBeenOn = false;
bool fanHasBeenOffAlert = true;
int fanOnTime = 30;
int fanOffTime = 5;
//3 days
//int fanOffAlertTime = 3 * 24 * 60;
//12 hours
int fanOffAlertTime = 1 * 12 * 60;



void loop() {
    long pressure_hPa = mpr.readPressure();
    char psiStr[15];
    sprintf(psiStr, "%ld", pressure_hPa);
        
    Particle.publish("PSI", psiStr, 60, PRIVATE);
    
    unsigned long nowOn = millis();
    unsigned long nowOff = millis();
    unsigned long nowAlertOff = millis();

     
    //1 hour
    
    if(pressure_hPa>990){
        //fan on
        digitalWrite(LED, HIGH);
        if ((nowOn - lastTimeOn) >=  fanOnTime * 60 * 1000) {
        	lastTimeOn = nowOn;
        	//fan was previously off but now is on for x time
            if(!fanHasBeenOn){
                Particle.publish("fan_on", NULL, 60,  PRIVATE);
                fanHasBeenOn = true;
                fanHasBeenOffAlert = false;
            } 
        }
    } else {
        digitalWrite(LED, LOW);

        if ((nowOff - lastTimeOff) >=  fanOffTime * 60 * 1000) {
        	lastTimeOff = nowOff;
        	//fan was previously on but now is off for x time
            if(fanHasBeenOn){
                Particle.publish("fan_off", NULL, 60,  PRIVATE);
                fanHasBeenOn = false;
            } 
        }

        if ((nowAlertOff - lastTimeAlertOff) >=  fanOffAlertTime * 60  * 1000) {
        	lastTimeAlertOff = nowAlertOff;
        	//fan was previously on but now is off for x time
            if(fanHasBeenOffAlert){
                Particle.publish("fan_off_days_alert", NULL, 60,  PRIVATE);
            } else { 
                fanHasBeenOffAlert = true;
            } 
        }

      
    }

    delay(1000);
}





