#import "Tr4cker.h"

Tr4cker tr4cker("put-your-device-key-here");

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    tr4cker.begin();

    // Deep sleep for 15 minutes
    Serial.println("Going to deep sleep.");
    ESP.deepSleep(1 * 60 * 1000 * 1000);

    // Troubleshoting: Device does not awake after deep sleep
    // Sometimes you may need to connect RST pin to another pin to activate deep sleep.
}
