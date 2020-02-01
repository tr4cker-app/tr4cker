#import "Tr4cker.h"

Tr4cker tr4cker("put-your-device-key-here");

void setup() {
    Serial.begin(115200);
}

void loop() {
    int start = millis();
    tr4cker.begin();
    int end = millis();
    int delta = end - start;
    
    
    Serial.print("Tr4cker was running for ");
    Serial.print(delta);
    Serial.println(" ms.");
}
