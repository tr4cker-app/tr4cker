#import "Tr4cker.h"

Tr4cker tr4cker("put-your-device-key-here");

#define INTERVAL 15

void setup() {
    Serial.begin(115200);
}

void loop() {
    // We do not have access to real real date and time. 
    // So instead of storing actual time, tr4acker need to know how often do we perform location tracking. 
    // enableHistory takes integer representig measuring interval in minutes.
    // This have to be the same as deep sleep interval
    tr4cker.enableHistory(INTERVAL); // in minutes
    tr4cker.begin();
    
    Serial.println("Going to deep sleep");
    ESP.deepSleep(INTERVAL * 60 * 1000 * 1000);
}
