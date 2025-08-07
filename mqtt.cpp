#include "Arduino.h"
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "SPIFFS.h"
#include "syncTime.h"
#include <ArduinoJson.h>

#define BUFFER_LEN 256
long lastMsg = 0;
char msg[BUFFER_LEN];

WiFiClientSecure espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    String clientId = "esp32_dev";
    String thingName = THING_NAME;
    String topic = "devices/" + thingName + "/commands";

    syncTime(); // Sincroniza la hora antes de intentar conectar

    // Antes de intentar conectar el cliente MQTT, asegúrate de que el cliente seguro no esté ya conectado
    if (espClient.connected()) {
      Serial.println("espClient ya conectado, desconectando...");
      espClient.stop(); // Asegura una desconexión limpia del cliente TLS
    }

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado");
      if (client.subscribe(topic.c_str())) {
        Serial.print("Listening ");
        Serial.print(topic.c_str());
        Serial.println();
        
      } else {
        Serial.println("¡error to subscribe 'commands'!");
        client.disconnect(); 
      }
    } else {
      Serial.printf("Error connection MQTT, rc=%d. ", client.state());
      Serial.printf("State WiFiClientSecure: connect=%d, enable=%d. waiting 5 seconds...\n", 
                    espClient.connected(), espClient.available());
      delay(5000);
    }
  }
}

void mqtt_if_connected() {
  if (!client.connected()) {
    Serial.println("MQTT Client disconnected. retry connection...");
    reconnect();
  }
}

void mqtt_loop() {
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  char messageBuffer[length + 1];
  for (int index = 0; index < length; index++) {
    messageBuffer[index] = (char)payload[index];
  }
  messageBuffer[length] = '\0';

  Serial.println(messageBuffer);

  if (String(topic) == "commands") {
    StaticJsonDocument<100> doc;

    DeserializationError error = deserializeJson(doc, messageBuffer);

    if (error) {
      Serial.print(F("Error JSON: "));
      Serial.println(error.f_str());
      return;
    }

    // Extrae los valores del JSON
    int pin = doc["pin"];
    const char* action = doc["action"];

    Serial.print("Pin: ");
    Serial.println(pin);
    Serial.print("Action: ");
    Serial.println(action);

    if (strcmp(action, "ON") == 0) {
      digitalWrite(pin, HIGH);
      Serial.println("LED Encendido");
    } else if (strcmp(action, "OFF") == 0) {
      digitalWrite(pin, LOW);
      Serial.println("LED Apagado");
    }
  }
}

void setup_mqtt() {
  
  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

  espClient.setCACert(MQTT_ROOT_CA);
  espClient.setCertificate(MQTT_CERT);
  espClient.setPrivateKey(MQTT_PRIVATE_KEY);

  client.setServer(MQTT_HOST, MQTT_PORT);
  
  client.setCallback(callback);
}
