#include "EEPROMManager.hpp"


void EEPROMManager::reloadFromEEPROM(){
    char buffer[100];     //works as long nothing is longer than 100 bytes
    unsigned int j = 0;
    unsigned int i = SSIDstart;
    while(i<PWstart){
        buffer[j] = EEPROM.read(i);
        if(buffer[j] == '\0') break;
        ++j;
        ++i;
    }
    j=0;
    savedSSID = buffer;   //write into saved string

    i=PWstart;
    while(i<MQTTstart){
        buffer[j] = EEPROM.read(i);
        if(buffer[j] == '\0') break;
        ++j;
        ++i; 
    }
    j=0;
    savedPW = buffer;

    i=MQTTstart;
    while(i<(MQTTstart+IPLEN)){
        buffer[j] = EEPROM.read(i);
        if(buffer[j] == '\0') break;
        ++j;
        ++i; 
    }
    j=0;
    i=0;
    savedMQTT.fromString(buffer);
}

bool EEPROMManager::isValid(){
    char buffer[VALLEN];
    char valstring[VALLEN] = VALPATTERN;
    for(unsigned int i = 0; i < VALLEN; ++i){
        buffer[i] = EEPROM.read(i);
    }
    if(strcmp(buffer,valstring) == 0){
        return true;
    }else{
        return false;
    }
    return false;
}

void EEPROMManager::init(){
    
    EEPROM.begin(512);
    if( this->isValid()){
        reloadFromEEPROM();
    }else{
        savedSSID = "empty";
        savedPW = "empty";
        savedMQTT.fromString("0.0.0.0");
    }
}

bool EEPROMManager::setSSID(String SSID){
    if(SSID.length()<SSIDLEN){
        savedSSID = SSID;
        return true;
    }
    return false;    
}

bool EEPROMManager::setPW(String PW){
    if(PW.length()<PWLEN){
        savedPW = PW;
        return true;
    }
    return false;
}

bool EEPROMManager::setMQTT(String MQTT){
    IPAddress ip;
    if(ip.isValid(MQTT)){
        savedMQTT = ip;
        return true;
    }else{
        return false;
    }
    
}

bool EEPROMManager::setMQTTIP(IPAddress MQTT){
    savedMQTT = MQTT;
    return true;
}

bool EEPROMManager::commit(){
    unsigned int j = 0;
    for(unsigned int i = 0; i < savedSSID.length(); ++i){
        EEPROM.write(SSIDstart+i, savedSSID[i]);
        ++j;
    }
    EEPROM.write(SSIDstart+j, '\0');
    j=0;
    for(unsigned int i = 0; i < savedPW.length(); ++i){
        EEPROM.write(PWstart+i, savedPW[i]);
        ++j;
    }
    EEPROM.write(PWstart+j, '\0');
    j=0;
    String MQTTip = savedMQTT.toString();
    for(unsigned int i = 0; i < MQTTip.length(); ++i){
        EEPROM.write(MQTTstart+i, MQTTip[i]);
        ++j;
    }
    EEPROM.write(MQTTstart+j, '\0');
    j=0;
    
    if(EEPROM.commit()){
        char valpat[VALLEN] = VALPATTERN;
        for(unsigned int i = 0; i < VALLEN; ++i){
            EEPROM.write(i,valpat[i]);
        }
        return EEPROM.commit();
    }
    else{
        return false;
    }
}

String EEPROMManager::getSSID(){
    return savedSSID;
}

String EEPROMManager::getPW(){
    return savedPW;
}

IPAddress EEPROMManager::getMQTT(){
    return savedMQTT;
}

String EEPROMManager::getMQTTString(){
    return savedMQTT.toString();
}