#import "Tr4cker.h"

Tr4cker tr4cker("put-your-device-key-here");

void setup() {
    Serial.begin(115200);
}

void loop() {
    tr4cker.setBatteryPin(A0);
    tr4cker.enableHistory(true);
    tr4cker.begin();
    
    // Sleep for 15 minutes
    Serial.println("Going to sleep.");
    delay(1*60*1000);
}
