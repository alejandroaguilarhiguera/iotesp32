
#include "Arduino.h"
#include "secrets.h"
#include <WiFi.h>

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Conectando.. ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while(WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.print("");
  Serial.println("WiFi conectado!");
  Serial.println("Direccion ip:");
  Serial.println(WiFi.localIP());
}
