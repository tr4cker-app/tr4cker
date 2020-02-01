#import "Tr4cker.h"

Tr4cker tr4cker("put-your-device-key-here");

void setup() {
    Serial.begin(115200); // you can remove this line as well if you do not output anything
    // TODO: NOT WORKING YET    
    tr4cker.begin();
}

void loop() {

}
