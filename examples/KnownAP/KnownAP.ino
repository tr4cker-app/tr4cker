#import "Tr4cker.h"

Tr4cker tr4cker("put-your-device-key-here");

void setup() {
    Serial.begin(115200);
    tr4cker.addAP("MyRouter", "longpassword");
    tr4cker.addAP("my-second-router", "password");
    // Add as many AP as you like
}

void loop() {
    tr4cker.begin();
    
    // Sleep for 15 minutes
    Serial.println("Going to sleep.");
    delay(15*60*1000);
}
