#include "config_hacked.h"
#include <Arduino.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "LEDControll.hpp"
#include <homie.hpp>
#include <PubSubClient.h>
#include <Ticker.h>
#include <IPAddress.h>
#include "EEPROMManager.hpp"

#define CLIENTID "ESP8266Client-"
#define SERIAL true


void httpServer_ini();
void handleReboot();
void handleGet();
void handleHome();
void handleStatus();
void callback(char *topic, byte *payload, unsigned int length);
boolean reconnect();
void shedPubISR();
void wifiSetup();

const char *update_path = "/firmware";
const char *update_username = USERNAME;
const char *update_password = PASSWORD;
String ClientID;
unsigned long lastReconnectAttempt = 0;

ESP8266HTTPUpdateServer httpUpdater;
ESP8266WebServer httpServer(80);
WiFiClient espClient;
PubSubClient client;
LEDControll strip(RED, GREEN, BLUE, INVERTED);
Homie homieCTRL = Homie(&client);
EEPROMManager eepromManag;

void setup() {
        if(SERIAL) Serial.begin(115200);
        if(SERIAL) while (!Serial);

        EEPROM.begin(512);
        eepromManag.init();

        delay(1000);
        if (SERIAL) Serial.println("eeprom initialaized");
        String wifi_ssid = eepromManag.getSSID();
        String wifi_pass = eepromManag.getPW();
        IPAddress mqtt_ipAdress = eepromManag.getMQTT();

        int wifiFail = 0;
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);
  while (WiFi.status() != WL_CONNECTED) {
    if(wifiFail >= 30) wifiSetup();     //if not able to connect, starts the AP
    delay(500);
    if (SERIAL)
      Serial.print(".");
    ++wifiFail;
  }
        if(SERIAL) Serial.println("");
        if(SERIAL) Serial.print("Connected, IP address: ");
        if(SERIAL) Serial.println(WiFi.localIP());

        client = PubSubClient(mqtt_ipAdress, MQTT_PORT, callback, espClient);

        HomieDevice homieDevice = HomieDevice(DEVICE_NAME, F_DEVICE_NAME, "",
                                              CHIP_TYPE);
        HomieNode rgbStrip = HomieNode("rgb-strip", "RGB Strip", "RGB Strip");
        HomieProperties rgb = HomieProperties("rgb", "RGB", true, true, "",
                                              homie::color_t, "rgb");
        rgbStrip.addProp(rgb);
        homieDevice.addNode(rgbStrip);
        homieCTRL.setDevice(homieDevice);

        httpServer_ini();
        strip.init();
        ClientID = String(CLIENTID) + DEVICE_NAME;
        homieCTRL.connect(ClientID.c_str(), MQTT_USR, MQTT_PW);

}

void loop() {
        if (!homieCTRL.connected()) {
                unsigned long now = millis();
                if (now - lastReconnectAttempt > 5000) {
                        lastReconnectAttempt = now;
                        // Attempt to reconnect
                        if (reconnect()) {
                                lastReconnectAttempt = 0;
                        }
                }
        }
        homieCTRL.loop();
        httpServer.handleClient();
        MDNS.update();
}

void wifiSetup() {
  IPAddress ip;
  IPAddress snmask;
  String randName = DEVICE_NAME + "x" + String(rand() % 10);
  ip.fromString("192.168.0.1");
  snmask.fromString("255.255.255.0");
  WiFi.softAP(randName.c_str(),"123456789");
  WiFi.softAPConfig(ip,ip,snmask);
  if (SERIAL)
    Serial.print("AP online, IP address: ");
  if (SERIAL)
    Serial.println(WiFi.softAPIP());
  httpServer_ini();
   while (1){
      httpServer.handleClient();
      MDNS.update();
      ESP.wdtFeed();
   }
  
}

void httpServer_ini() {
        char buffer[100];
        sprintf(buffer, "%s", DEVICE_NAME);
        MDNS.begin(buffer);
        httpUpdater.setup(&httpServer, update_path, update_username, update_password);
        httpServer.on("/status",handleStatus);
        httpServer.on("/", handleHome);
        httpServer.on("/get",handleGet);
        httpServer.on("/reboot",handleReboot);
        httpServer.begin();
        MDNS.addService("http", "tcp", 80);
        if(SERIAL) Serial.printf("HTTPUpdateServer ready! Open http://%s.local%s in your "
                                 "browser and login with username '%s' and password '%s'\n",
                                 buffer, update_path, update_username, update_password);
        //------
}

boolean reconnect() {
        // Loop until we're reconnected
        return homieCTRL.connect(ClientID.c_str(), MQTT_USR, MQTT_PW);
}

void handleReboot(){
  String message;

  message += "<!DOCTYPE html><html>";
  message += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  message += "<link rel=\"icon\" href=\"data:,\"></head>";
  message += "<body><h3 align=\"left\">";
  message += "Rebooting now";
  message += "</h3>";
  message += "<p><a href=\"/\">back to root</a></p>";
  message += "</body>";
  httpServer.send(200, "text/html", message);
  delay(1000);
  ESP.restart();
}

void handleGet(){
  int argCount;
  String errorMsg;
  String message;
  argCount = httpServer.args();

  if(SERIAL){
    String savedSSID = eepromManag.getSSID();
    String savedPW = eepromManag.getPW();
    String savedMQTT =  eepromManag.getMQTTString();
    Serial.printf("saved values just read:\n");
    Serial.printf("savedSSID: %s\n", savedSSID.c_str());
    Serial.printf("savedPW: %s\n", savedPW.c_str());
    Serial.printf("savedMQTT: %s\n", savedMQTT.c_str());
  
    Serial.printf("\n argCount: %d",argCount);
    for(int i = 0; i < argCount; ++i){
      Serial.printf("\n argName%d: ",i);
      Serial.printf(httpServer.argName(i).c_str());
      Serial.printf("\n arg%d: ",i);
      Serial.printf(httpServer.arg(i).c_str());
    }
    Serial.printf("\n");
  }
  
  for(int i = 0; i < httpServer.args(); ++i){   // go through all args
      String argName = httpServer.argName(i);   // get argName 
      String argVal = httpServer.arg(i);        // get Argument
      if(argVal.length()>=512){
        if(SERIAL) Serial.printf("\n argString %s too long, abort\n", argName.c_str());
        errorMsg += "\nInput too long, did not allocate space!\n";
        continue;
      }
      if(argVal.length()==0){                       //test if Empty
        if(SERIAL) Serial.printf("\n argString %s empty, abort\n", argName.c_str());
      }
      else if(argName.equals("ssid") ){
        eepromManag.setSSID(argVal);                     //write argument into save
        if(SERIAL) Serial.printf("new SSID %s\n",eepromManag.getSSID().c_str());
      }
      else if(argName.equals("pw")){
        eepromManag.setPW(argVal);
        if(SERIAL) Serial.printf("new pw %s\n",eepromManag.getPW().c_str());
      }
      else if(argName.equals("mqtt")){
        if(!eepromManag.setMQTT(argVal)){
          errorMsg += "\n non-valid IP Address\n";
        }
        if(SERIAL) Serial.printf("new mqtt %s\n",eepromManag.getMQTTString().c_str());
      }
      
  }
  
  if(eepromManag.commit()){      //commit after preparing what to write to the eeprom
    if(SERIAL) Serial.printf("\nEEPROM commit successful!\n");
    errorMsg += "EEPROM commit successful!\n";
  }else{
    if(SERIAL) Serial.printf("\nEEPROM commit not successful!\n");
    errorMsg += "EEPROM commit not successful!\n";
  }

  

  if(SERIAL){
    String savedSSID = eepromManag.getSSID();
    String savedPW = eepromManag.getPW();
    String savedMQTT =  eepromManag.getMQTTString();
    Serial.printf("saved values just read:\n");
    Serial.printf("savedSSID: %s\n", savedSSID.c_str());
    Serial.printf("savedPW: %s\n", savedPW.c_str());
    Serial.printf("savedMQTT: %s\n", savedMQTT.c_str());
  
    Serial.printf("\n argCount: %d",argCount);
    for(int i = 0; i < argCount; ++i){
      Serial.printf("\n argName%d: ",i);
      Serial.printf(httpServer.argName(i).c_str());
      Serial.printf("\n arg%d: ",i);
      Serial.printf(httpServer.arg(i).c_str());
    }
    Serial.printf("\n");
  }
  

  message += "<!DOCTYPE html><html>";
  message += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  message += "<link rel=\"icon\" href=\"data:,\"></head>";
  message += "<body><h3 align=\"left\">";
  message += errorMsg;
  message += "</h3>";
  message += "<p><a href=\"/\">back to root</a></p>";
  message += "</body>";
  httpServer.send(200, "text/html", message);
}

void handleHome() {
  String message;
    // Die Webseite anzeigen
  message += "<!DOCTYPE html><html>";
  message += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  message += "<link rel=\"icon\" href=\"data:,\"></head>";
  message += "<body><h3 align=\"left\">Subsites</h3>";
  message += "<p><a href=\"/status\">device status</a></p>";
  message += "<p><a href=\"/firmware\">upload firmware</a></p>";
  message += "<form action=\"/get\">change ssid: <input type=\"text\" name=\"ssid\"maxlength=\"511\"><br>change password: <input type=\"password\" name=\"pw\"maxlength=\"511\"><br><input type=\"submit\" value=\"Save\"></form><br>";
  message += "<br><form action=\"/get\">change MQTT server: <input type=\"text\" name=\"mqtt\"maxlength=\"511\"><input type=\"submit\" value=\"Save\"></form>";
  message += "<p><a href=\"/reboot\"><button>Reboot</button></a> </p>";
  message += "</body>";
  httpServer.send(200, "text/html", message);
}

void handleStatus() {
        String message;
        message += "name: " + String(DEVICE_NAME) + "\n";
        message += "chip: " + String(CHIP_TYPE) + "\n";
        message += "IP: " + WiFi.localIP().toString() + "\n";
        message +="free Heap: " + String(ESP.getFreeHeap()) + "\n";
        message += "heap Fragmentation: " + String(ESP.getHeapFragmentation()) + "\n";
        message += "MaxFreeBlockSize: " + String(ESP.getMaxFreeBlockSize()) + "\n";
        message += "ChipId: " + String(ESP.getChipId()) + "\n";
        message += "CoreVersion: " + String(ESP.getCoreVersion()) + "\n";
        message += "SdkVersion: " + String(ESP.getSdkVersion()) + "\n";
        message += "SketchSize: " + String(ESP.getSketchSize()) + "\n";
        message += "FreeSketchSpace: " + String(ESP.getFreeSketchSpace()) + "\n";
        message += "FlashChipId: " + String(ESP.getFlashChipId()) + "\n";
        message += "FlashChipSize: " + String(ESP.getFlashChipSize()) + "\n";
        message += "FlashChipRealSize: " + String(ESP.getFlashChipRealSize()) + "\n";
        httpServer.send(200, "text/plain", message);
}

void callback(char *topic, byte *payload, unsigned int length) {
        string topicString = string(topic);
        string searchString = string(DEVICE_NAME) + "/rgb-strip/rgb/set";
        if(SERIAL) Serial.println(topicString.c_str());

        std::size_t found = topicString.find(searchString);
        if(found!=std::string::npos) {
                char p[length + 1];
                memcpy(p, payload, length);
                p[length] = 0;
                char *r;
                char *g;
                char *b;
                r = &p[0];
                g = strchr(p, ',') + 1;
                b = strrchr(p, ',') + 1;
                *(g-1) = 0;
                *(b-1) = 0;
                colorPoint newColor = colorPoint((uint8_t)atol(r),(uint8_t)atol(g),(uint8_t)atol(b));
                strip.moveToColor(newColor);

                string feedbackTopic = homieCTRL.getPubString("rgb-strip", "rgb");
                if(feedbackTopic == "") feedbackTopic = "error";
                client.publish(feedbackTopic.c_str(),newColor.getColorString().c_str(),true);
        }
}
