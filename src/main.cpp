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

#define CLIENTID "ESP8266Client-"
#define SERIAL false


void httpServer_ini();
void handleStatus();
void callback(char *topic, byte *payload, unsigned int length);
boolean reconnect();
void shedPubISR();

const char *update_path = "/firmware";
const char *update_username = USERNAME;
const char *update_password = PASSWORD;
String ClientID;
unsigned long lastReconnectAttempt = 0;
uint8_t flag_pubTimer = 1;

ESP8266HTTPUpdateServer httpUpdater;
ESP8266WebServer httpServer(80);
WiFiClient espClient;
PubSubClient client(MQTT_IP, MQTT_PORT, callback, espClient);
LEDControll strip(RED, GREEN, BLUE);
Homie homieCTRL = Homie(&client);
Ticker ShedPub;

void setup() {
        if(SERIAL) Serial.begin(115200);
        if(SERIAL) while (!Serial);
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                if(SERIAL) Serial.print(".");
        }
        if(SERIAL) Serial.println("");
        if(SERIAL) Serial.print("Connected, IP address: ");
        if(SERIAL) Serial.println(WiFi.localIP());

        HomieDevice homieDevice = HomieDevice(DEVICE_NAME, F_DEVICE_NAME, WiFi.localIP().toString().c_str(),
                                              WiFi.macAddress().c_str(), FW_NAME, FW_VERSION,
                                              "esp01-s", "60");
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
        if(flag_pubTimer) {
                long time = millis() / 1000;
                string topic = "homie/" + string(DEVICE_NAME) + "/$stats/uptime";
                char payload[20];
                sprintf(payload, "%ld", time);
                client.publish(topic.c_str(), payload,true);
                ShedPub.once(60.0, shedPubISR);
                flag_pubTimer = 0;
        }
        homieCTRL.loop();
        httpServer.handleClient();
        MDNS.update();
}

void httpServer_ini() {
        char buffer[100];
        sprintf(buffer, "%s", DEVICE_NAME);
        MDNS.begin(buffer);
        httpUpdater.setup(&httpServer, update_path, update_username, update_password);
        httpServer.on("/status",handleStatus);
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

void handleStatus() {
        String message;
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
        }
}

void shedPubISR() {
        flag_pubTimer = 1;
}
