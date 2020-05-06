# Tr4cker ESP8266 [![Build Status](https://travis-ci.com/tr4cker-app/tr4cker.svg?branch=master)](https://travis-ci.com/tr4cker-app/tr4cker)

Tracking position of any Wifi capable device.

<img width="697" alt="map" src="https://user-images.githubusercontent.com/281281/78156218-c6c7d200-743e-11ea-80fc-2c969a0e954f.png">

## Motivation

This project combines two well know techniques into one solution that suprisingly works.



### MAC address geolocation

It is not secret that mobile phones are not only using GPS to obtain their position but scan for near by Wifi access points. Every access point has a unique MAC address. Wifi only capable device can scan near by Wifi access points and their singnal strength and using triangulation estimate exact position map. The same way GPS module does with help of sattelites. Google has a detail map of all the access points and offers an geolocaion API where one can simply send a query with list of MAC addresses of access points and receive back a position(latitue, longitude and accuracy).

### DNS tunneling

This is all nice but how can a device send a request to Google geolocation API without internet. Obviously it has to connect to one of many open Wifi networks(Hotspots). Most of the open Wifi networks has a some sort of captive portal where user has at minimum agree to terms and in worst case even pay. If only there was a way to bypass these captive portals. There is and it is called DNS tunneling.

DNS tunneling is a techinque that use DNS protocal to "tunnel" additional data. Idea is very simple. Device makes a DNS query for a domain data-to-transfer.tr4cker.app . Special-dns-server not only reply with a IP address but also write down data-to-transfer to database. Unlike other protocols DNS queries does not go directly to the server but rather propagate via intermidiate DNS servers. Starting with Wifi access point gateway who sends to its DNS server and so on until query will reach tr4cker.app server.

## Usage

This a a bare minimum example. It assumes that you have logg on to [tr4cker.app](https://tr4cker.app), created new device and obtained **deviceKey**.

```cpp
#import "Tr4cker.h"

Tr4cker tr4cker("put-your-device-key-here");

void setup() {
    Serial.begin(115200);
    // Uncomment following line if you do not have any open Wifi networks near by
    //tr4cker.addAP("MyRouter", "longpassword");
}

void loop() {
    // Magic happens here
    // Tr4cker estimates position based on Wifi access points around
    // and send specialy crafted request over open Wifi network.
    tr4cker.begin();

    // Sleep for 15 minutes
    Serial.println("Going to sleep.");
    delay(15*60*1000);
}
```

For more examples see **examples** folder. You may for example save energy by putting device into deep sleep or add known access points for testing purpose.

## Supported devices

Currently this repo support _ESP8266_ and _ESP32_. Feel free to open issue with any other Wifi capable board.

## Developing

### Create symlink to Arduino library folder

```sh
./symlink.sh
```

Once done, simply call unsymlink to remove symlink.

```sh
./unsymlink.sh
```
