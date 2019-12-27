#define LED D7
#define SENSORONE A0
#define SENSORTWO A5

void setup() {
    Serial.begin(115200);
    Time.zone(-6);
    Time.beginDST();
    pinMode(LED, OUTPUT);
    pinMode(SENSORONE, INPUT);
    pinMode(SENSORTWO, INPUT);

    RGB.control(true); 
    RGB.brightness(0);
}


//how many minutes fan needs to be on before alerting
int fanOnTime = 5;
int fanOffTime = 5;

//3 days
int fanOffAlertTime = 3 * 24 * 60;
//12 hours
//int fanOffAlertTime = 1 * 12 * 60;

//1 min
// int fanOffAlertTime = 1;


bool fanOneHasBeenOn = false;
bool fanOneHasBeenOffAlert = true;

bool fanTwoHasBeenOn = false;
bool fanTwoHasBeenOffAlert = true;

unsigned long lastTimeOneOn = 0;
unsigned long lastTimeOneOff = 0;
unsigned long lastTimeAlertOneOff = 0;

unsigned long lastTimeTwoOn = 0;
unsigned long lastTimeTwoOff = 0;
unsigned long lastTimeAlertTwoOff = 0;

uint32_t msDelay;


void loop() {
    if (millis() - msDelay < 3000) return;
    msDelay = millis();
  
    int sensorOneValue = analogRead(SENSORONE);
    int sensorTwoValue = analogRead(SENSORTWO);

    detectPower(sensorOneValue,"dryerOne ", lastTimeOneOn, lastTimeOneOff, lastTimeAlertOneOff, fanOneHasBeenOn, fanOneHasBeenOffAlert);
    detectPower(sensorTwoValue,"dryerTwo ", lastTimeTwoOn, lastTimeTwoOff, lastTimeAlertTwoOff, fanTwoHasBeenOn, fanTwoHasBeenOffAlert);
}


void detectPower(int sensorVal, char *dryerName, unsigned long &lastTimeOn, unsigned long &lastTimeOff, unsigned long &lastTimeAlertOff, bool &fanHasBeenOn, bool &fanHasBeenOffAlert){
    char alertInfo[40];
    char sensorStr[10];
    char lastOffTimeStr[20];


    strcpy(alertInfo, dryerName);
    sprintf(sensorStr, "Power: %ld", sensorVal);
    strcat(alertInfo, sensorStr);
    
    if(sensorVal<2800 || sensorVal > 2890){
        Particle.publish("ElectricityON", alertInfo, 60, PRIVATE);

        //fan on
        digitalWrite(LED, HIGH);

        if ((millis() - lastTimeOn) >=  fanOnTime * 60 * 1000) {
        	(lastTimeOn) = millis();
        	//fan was previously off but now is on for x time
            if(!fanHasBeenOn){
                Particle.publish("fan_on", dryerName, 60,  PRIVATE);
                fanHasBeenOn = true;
                fanHasBeenOffAlert = false;
            } 
        }
    } else {
        digitalWrite(LED, LOW);
        Particle.publish("ElectricityOff", alertInfo, 60, PRIVATE);

        if ((millis() - lastTimeOff) >=  fanOffTime * 60 * 1000) {
        	(lastTimeOff) = millis();
        	//fan was previously on but now is off for x time
            if(fanHasBeenOn){
                Particle.publish("fan_off", dryerName, 60,  PRIVATE);
                fanHasBeenOn = false;
            } 
        }

        if ((millis() - lastTimeAlertOff) >=  fanOffAlertTime * 60  * 1000) {
        	(lastTimeAlertOff) = millis();
        	//fan was previously on but now is off for x time
            if(fanHasBeenOffAlert){
                Particle.publish("fan_off_days_alert", dryerName, 60,  PRIVATE);
            } else { 
                fanHasBeenOffAlert = true;
            } 
        }
    }
}
