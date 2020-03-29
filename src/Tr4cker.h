/*
  Tr4cker.h - Device tracking with only Wifi.
  Created by Robert Sasak, January 16, 2020.
*/

#ifndef Tr4cker_h
#define Tr4cker_h

#include <vector>
#include "FS.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define MAX_APS 6
#define MAIN_DOMAIN "a.tr4cker.app"
#define DEVICE_KEY_LENGTH 20
#define MAX_CONNETING_TIME_TO_OPEN_AP 10
#define MAX_CONNETING_TIME_TO_KNOWN_AP 25
#define RECORD_LOG_FILE "tr4cker1.log"
#define DOMAIN_NAME_SIZE 127

typedef struct
{
  char *ssid;
  char *passphrase;
} WifiAPlist_t;

typedef enum
{
  TR_SUCCESS = 0,
  TR_NO_APS_AVAILABLE = 1,
} tr_status_t;

class Tr4cker
{
public:
  Tr4cker(char *_deviceKey);
  ~Tr4cker();
  void setBatteryPin(uint8_t pin);
  void enableHistory(bool state);
  bool addAP(const char *ssid, const char *passphrase);
  void begin();

private:
  char *_deviceKey;
  uint8_t _batteryPin = 0;
  bool isHistoryEnabled = false;
  char *encryptionTypeToString(uint8_t authMode);
  uint8_t readBattery();
  void freeAPList();
  char *getPassphrase(String ssid);
  std::vector<WifiAPlist_t> APlist;
  void readLine(File file, char *buffer);
  bool dnsLookup(char *domain);
};

#endif
