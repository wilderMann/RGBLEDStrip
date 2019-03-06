#include "config.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;
  pinMode(D7, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D5, OUTPUT);
  analogWrite(D7, 0);
  analogWrite(D6, 0);
  analogWrite(D5, 0);
}

void loop() {
  for (int i = 0; i <= 10; i++) {
    analogWrite(D7, i * 100);
    delay(100);
  }
  for (int i = 10; i >= 0; i--) {
    analogWrite(D7, i * 100);
    delay(100);
  }
  for (int i = 0; i <= 10; i++) {
    analogWrite(D6, i * 100);
    delay(100);
  }
  for (int i = 10; i >= 0; i--) {
    analogWrite(D6, i * 100);
    delay(100);
  }
  for (int i = 0; i <= 10; i++) {
    analogWrite(D5, i * 100);
    delay(100);
  }
  for (int i = 10; i >= 0; i--) {
    analogWrite(D5, i * 100);
    delay(100);
  }
  analogWrite(D7, 1024);
  analogWrite(D6, 1024);
  analogWrite(D5, 1024);
  delay(2000);
  analogWrite(D7, 0);
  analogWrite(D6, 0);
  analogWrite(D5, 0);

  analogWrite(D7, 10240);
  delay(2000);
  analogWrite(D7, 0);

  analogWrite(D6, 10240);
  delay(2000);
  analogWrite(D6, 0);

  analogWrite(D5, 10240);
  delay(2000);
  analogWrite(D5, 0);
}
