#import "Tr4cker.h"

Tr4cker tr4cker("put-your-device-key-here");

void setup() {
    Serial.begin(115200);
}

void loop() {
    // Magic happen here
    // Tr4cker estimates position based on Wifi access points around
    // and send specialy crafted request over open Wifi network.
    tr4cker.begin();
    
    // Sleep for 15 minutes
    Serial.println("Going to sleep.");
    delay(15*60*1000);
}
