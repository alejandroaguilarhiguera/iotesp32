#include "SPIFFS.h"
#include "syncTime.h"
#include "wifiHandler.h"
#include "qrReader.h"
#include "mqtt.h"

void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);

  setup_qr();
  
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  setup_wifi();
  syncTime();

  delay(10);

  setup_mqtt();
  delay(20);
 

}

void loop() {
  mqtt_if_connected();

  
  read_qr();

  // TODO: Facial recognition
  // read_facila_recognition();
  // TODO: Fingerprint
  // read_finger_print();
  // TODO: RFID
  // read_rfid();
  

  mqtt_loop();
}
