#include <ArduinoJson.h>
#include "SPIFFS.h"
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <time.h>
#include "Arduino.h"

void syncTime() {
  Serial.println("Sync hours server NTP...");
  configTime(-25200, 0, "pool.ntp.org", "time.nist.gov", "0.cl.pool.ntp.org");
  
  struct tm timeinfo;
  int attempts = 0;
  while (!getLocalTime(&timeinfo) && attempts < 15) { 
    Serial.print("Fail NTP, retry ");
    Serial.print(attempts + 1);
    Serial.println("/15...");
    delay(1000); 
    attempts++;
  }

  if (attempts >= 15) {
    Serial.println("CRITICAL ERROR: fail to get NTP. restart ESP32...");
    delay(5000);
    ESP.restart(); // Reiniciar si no se puede sincronizar la hora, es indispensable
  } else {
    Serial.println("hours updated successfuly.");
    char timeStr[70];
    strftime(timeStr, sizeof(timeStr), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    Serial.println(timeStr);
  }
}
