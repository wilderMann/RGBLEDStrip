
#ifndef EEPROMMANAGER_HPP_
#define EEPROMMANAGER_HPP_

#include <Arduino.h>
#include <IPAddress.h>
#include <EEPROM.h>

#define VALPATTERN "1234"
#define VALLEN 5
#define SSIDLEN 50
#define PWLEN 50
#define IPLEN 15


class EEPROMManager {

const unsigned int SSIDstart = VALLEN;
const unsigned int PWstart = VALLEN + SSIDLEN;
const unsigned int MQTTstart = VALLEN + SSIDLEN + PWLEN;
String savedSSID;
String savedPW;
IPAddress savedMQTT;
void reloadFromEEPROM();
bool isValid();

public:
void init();
bool setSSID(String SSID);
bool setPW(String PW);
bool setMQTT(String MQTT);
bool setMQTTIP(IPAddress MQTT);
bool commit();
String getSSID();
String getPW();
IPAddress getMQTT();
String getMQTTString();
};



#endif
