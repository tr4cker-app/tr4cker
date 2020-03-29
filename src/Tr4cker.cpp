/*
  Tr4ker.cpp - Device tracking with only Wifi.
  Created by Robert Sasak, January 16, 2020.
*/

#include "ESP8266WiFi.h"
#include "Tr4cker.h"

#define DEBUG true // set true or false to enable or disable debug
#define Serial \
    if (DEBUG) \
    Serial // the compiler will ignore or include serial

Tr4cker::Tr4cker(char *deviceKey)
{
    _deviceKey = deviceKey;
}

void Tr4cker::setBatteryPin(uint8_t batteryPin)
{
    _batteryPin = batteryPin;
}

void Tr4cker::begin()
{
    if (!_deviceKey || *_deviceKey == 0x00)
    {
        Serial.println("DeviceKey is not set.");
        return;
    }
    if (strlen(_deviceKey) != DEVICE_KEY_LENGTH)
    {
        Serial.println("DeviceKey has wrong length.");
        return;
    }

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    // WiFi.persistent(false);
    // WiFi.disconnect(true);
    // WiFi.setAutoConnect(false);
    // WiFi.setPhyMode(WIFI_PHY_MODE_11B);
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.println("\nScannning nearby Wifi networks.");
    int n = WiFi.scanNetworks();

    if (n == 0)
    {
        Serial.println("No networks found.");
        return; // tr_status_t.TR_NO_APS_AVAILABLE;
    }

    // Sort APs by RSSI
    // Store order in array indeces where indeces[0] is index of strongest AP
    int indices[n];
    for (int i = 0; i < n; i++)
    {
        indices[i] = i;
    }
    // Bubble sort
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i]))
            {
                int t = indices[i];
                indices[i] = indices[j];
                indices[j] = t;
            }
        }
    }

    Serial.print(n);
    Serial.println(" networks found.");
    Serial.println("#\tRSSI\tChannel\tENC\t\tMAC\t\t\tSSID");
    for (int i = 0; i < n; ++i)
    {
        Serial.print(i + 1);
        Serial.print("\t");
        Serial.print(WiFi.RSSI(indices[i]));
        Serial.print("\t");
        Serial.print(WiFi.channel(indices[i]));
        Serial.print("\t");
        Serial.printf("%-14s", encryptionTypeToString(WiFi.encryptionType(indices[i])));
        Serial.print("\t");
        Serial.print(WiFi.BSSIDstr(indices[i]));
        Serial.print("\t");
        Serial.print(WiFi.SSID(indices[i]));
        Serial.println();
    }
    Serial.println();

    uint8_t *bssid[MAX_APS];
    byte nullBssid[] = {0, 0, 0, 0, 0, 0};
    int rssi[MAX_APS];
    for (int i = 0; i < MAX_APS; i++)
    {
        if (n <= i)
        {
            bssid[i] = nullBssid;
            rssi[i] = 0;
        }
        else
        {
            bssid[i] = WiFi.BSSID(indices[i]);
            int r = -WiFi.RSSI(indices[i]);
            // make sure that received signal strength indicator is between 0 and -99
            if (r > 99)
            {
                r = 99;
            }
            if (r < 0)
            {
                r = 0;
            }
            rssi[i] = r;
        }
    }

    uint8_t batteryLevel = readBattery();
    char domain[DOMAIN_NAME_SIZE];
    sprintf(domain, "%02x%02x%02x%02x%02x%02x%02x%02d%02x%02x%02x%02x%02x%02x%02d%02x%02x%02x%02x%02x%02x%02d.%02x%02x%02x%02x%02x%02x%02d%02x%02x%02x%02x%02x%02x%02d%02x%02x%02x%02x%02x%02x%02d%s.%s\0",
            batteryLevel,
            bssid[0][0], bssid[0][1], bssid[0][2], bssid[0][3], bssid[0][4], bssid[0][5], rssi[0],
            bssid[1][0], bssid[1][1], bssid[1][2], bssid[1][3], bssid[1][4], bssid[1][5], rssi[1],
            bssid[2][0], bssid[2][1], bssid[2][2], bssid[2][3], bssid[2][4], bssid[2][5], rssi[2],
            bssid[3][0], bssid[3][1], bssid[3][2], bssid[3][3], bssid[3][4], bssid[3][5], rssi[3],
            bssid[4][0], bssid[4][1], bssid[4][2], bssid[4][3], bssid[4][4], bssid[4][5], rssi[4],
            bssid[5][0], bssid[5][1], bssid[5][2], bssid[5][3], bssid[5][4], bssid[5][5], rssi[5],
            _deviceKey, MAIN_DOMAIN);

    Serial.println("Special DNS request:");
    Serial.println(domain);

    bool success = false;
    for (int i = 0; i < n; i++)
    {
        char *passphrase;
        int keepTrying;
        if (WiFi.encryptionType(indices[i]) == ENC_TYPE_NONE)
        {
            char *passphrase = "";
            keepTrying = MAX_CONNETING_TIME_TO_OPEN_AP;
            Serial.print("Connecting to open network: ");
        }
        else
        {
            passphrase = getPassphrase(WiFi.SSID(indices[i]));
            if (passphrase == 0)
            {
                // Unkown network, try next one
                continue;
            }
            keepTrying = MAX_CONNETING_TIME_TO_KNOWN_AP;
            Serial.print("Connecting to known network: ");
        }
        Serial.print(WiFi.SSID(indices[i]));
        Serial.print(" ");
        Serial.println(WiFi.BSSIDstr(indices[i]));

        WiFi.begin(
            WiFi.SSID(indices[i]),
            passphrase,
            WiFi.channel(indices[i]),
            WiFi.BSSID(indices[i]),
            true);

        while (keepTrying > 0)
        {
            if (WiFi.status() == WL_CONNECTED)
            {
                break;
            }
            delay(500);
            Serial.print(".");
            keepTrying--;
        }
        Serial.println();
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("Connected to AP.");
            Serial.print("Gateway: ");
            Serial.println(WiFi.gatewayIP());
            Serial.print("IP:      ");
            Serial.println(WiFi.localIP());
            Serial.print("MASK:    ");
            Serial.println(WiFi.subnetMask());
            Serial.print("DNS:     ");
            Serial.println(WiFi.dnsIP());

            if (dnsLookup(domain))
            {
                success = true;
                break;
            }
        }
        else
        {
            Serial.println("Could not connect to AP.");
        }
    }
    if (success && interval > 0)
    {
        Serial.println("Reading location history log file.");
        SPIFFS.begin();
        File file = SPIFFS.open(RECORD_LOG_FILE, "r");
        if (!file)
        {
            Serial.println("Cannot open log file. No history locations.");
        }
        else
        {
            int fileSize = file.size();
            char domainWithTime[DOMAIN_NAME_SIZE];
            int records = fileSize / (DOMAIN_NAME_SIZE + 1);
            Serial.print("There are ");
            Serial.print(records);
            Serial.println(" location records that need to be send.");
            while (file.available())
            {
                readLine(file, domain);
                sprintf(domainWithTime, "%04x%s", interval * records, domain);
                dnsLookup(domainWithTime);
            }
            file.close();
            SPIFFS.remove(RECORD_LOG_FILE);
        }
    }
    else
    {
        Serial.println("Failed to connect to any open network.");
        if (interval > 0)
        {
            SPIFFS.begin();
            File file = SPIFFS.open(RECORD_LOG_FILE, "a");
            if (!file)
            {
                Serial.println("Cannot open record log file.");
            }
            else
            {
                file.println(domain);
                file.close();
                Serial.println("Wrote location record to log file so it can be send on next successful connection.");
            }
        }
    }
    Serial.println("");
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);
}

char *Tr4cker::encryptionTypeToString(uint8_t authMode)
{
    switch (authMode)
    {
    case ENC_TYPE_NONE:
        return "OPEN";
    case ENC_TYPE_WEP:
        return "WEP";
    case ENC_TYPE_TKIP:
        return "WPA_PSK";
    case ENC_TYPE_CCMP:
        return "WPA2_PSK";
    case ENC_TYPE_AUTO:
        return "WPA_WPA2_PSK";
    default:
        return "UNKOWN";
    }
}

uint8_t Tr4cker::readBattery()
{
    // Battery pin is not set
    if (_batteryPin == 0)
    {
        return 0;
    }
    return analogRead(_batteryPin);
}

bool Tr4cker::addAP(const char *ssid, const char *passphrase)
{
    WifiAPlist_t newAP;

    if (!ssid || *ssid == 0x00 || strlen(ssid) > 31)
    {
        // fail SSID too long or missing!
        Serial.println("No ssid or ssid too long");
        return false;
    }

    if (passphrase && strlen(passphrase) > 63)
    {
        // fail passphrase too long!
        Serial.println("Passphrase too long");
        return false;
    }

    newAP.ssid = strdup(ssid);

    if (!newAP.ssid)
    {
        Serial.println("Fail newAP.ssid == 0");
        return false;
    }

    if (passphrase && *passphrase != 0x00)
    {
        newAP.passphrase = strdup(passphrase);
        if (!newAP.passphrase)
        {
            Serial.println("Fail newAP.passphrase == 0");
            free(newAP.ssid);
            return false;
        }
    }
    else
    {
        newAP.passphrase = NULL;
    }

    APlist.push_back(newAP);
    Serial.printf("Add known SSID: %s\n", newAP.ssid);
    return true;
}

char *Tr4cker::getPassphrase(String ssid)
{
    for (uint32_t x = 0; x < APlist.size(); x++)
    {
        WifiAPlist_t entry = APlist[x];
        if (ssid == entry.ssid)
        {
            return entry.passphrase;
        }
    }
    return 0;
}

Tr4cker::~Tr4cker()
{
    freeAPList();
}

void Tr4cker::freeAPList()
{
    for (uint32_t i = 0; i < APlist.size(); i++)
    {
        WifiAPlist_t entry = APlist[i];
        if (entry.ssid)
        {
            free(entry.ssid);
        }
        if (entry.passphrase)
        {
            free(entry.passphrase);
        }
    }
    APlist.clear();
}

void Tr4cker::enableHistory(bool interval)
{
    this->interval = interval;
}

void Tr4cker::readLine(File file, char *buffer)
{
    int l = file.readBytesUntil('\n', buffer, DOMAIN_NAME_SIZE);
    buffer[l] = 0;
}

bool Tr4cker::dnsLookup(char *domain)
{
    IPAddress result;
    int err = WiFi.hostByName(domain, result);
    Serial.print("Performing DNS lookup for: ");
    Serial.println(domain);
    if (err == 1)
    {
        Serial.print("DNS lookup successfull IP address: ");
        Serial.println(result);
        return true;
    }
    Serial.print("DNS lookup failed with error code: ");
    Serial.println(err);
    return false;
}