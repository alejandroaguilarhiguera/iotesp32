#include <ArduinoJson.h>
#include "SPIFFS.h"
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* mqtt_server = "xxxx.amazonaws.com";
const int mqtt_port = 8883;
const char* ssid = "xxxxxxx";
const char* password = "xxxxxxx";

String Read_rootca = R"(-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----)";
String Read_cert = R"(-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----)";
String Read_privatekey = R"(-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----)";



// *******************
#define BUFFER_LEN 256
long lastMsg = 0;
char msg[BUFFER_LEN];
int value = 0;
char mac_Id[18];
byte mac[6];

WiFiClientSecure espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  // example
  // {
  // "pin": 17,
  // "action": "OFF"
  // }
  // {
  // "pin": 17,
  // "action": "ON"
  // }

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


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando.. ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

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

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    String clientId = "esp32_dev";

    syncTime(); // Sincroniza la hora antes de intentar conectar

    // Antes de intentar conectar el cliente MQTT, asegúrate de que el cliente seguro no esté ya conectado
    if (espClient.connected()) {
      Serial.println("espClient ya conectado, desconectando...");
      espClient.stop(); // Asegura una desconexión limpia del cliente TLS
    }

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado");
      if (client.subscribe("commands")) {
        Serial.println("Listening 'commands'");
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

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  setup_wifi();

  syncTime();

  delay(10);


  //****************
  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }
  
  char* pRead_rootca;
  pRead_rootca = (char *)malloc(sizeof(char) * (Read_rootca.length() + 1));
  strcpy(pRead_rootca, Read_rootca.c_str());

  char* pRead_cert;
  pRead_cert = (char *)malloc(sizeof(char) * (Read_cert.length() + 1));
  strcpy(pRead_cert, Read_cert.c_str());

  char* pRead_privatekey;
  pRead_privatekey = (char *)malloc(sizeof(char) * (Read_privatekey.length() + 1));
  strcpy(pRead_privatekey, Read_privatekey.c_str());

  Serial.println("================================================================================================");
  Serial.println();
  Serial.println("Root CA:");
  Serial.write(pRead_rootca);
  Serial.println("================================================================================================");
  Serial.println();
  Serial.println("Cert:");
  Serial.write(pRead_cert);
  Serial.println("================================================================================================");
  Serial.println();
  Serial.println("privateKey:");
  Serial.write(pRead_privatekey);
  Serial.println("================================================================================================");

  espClient.setCACert(pRead_rootca);
  espClient.setCertificate(pRead_cert);
  espClient.setPrivateKey(pRead_privatekey);

  client.setServer(mqtt_server, mqtt_port);
  
  client.setCallback(callback);


  //******************************************
  WiFi.macAddress(mac);
  snprintf(mac_Id, sizeof(mac_Id), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(mac_Id);
  //****************************************
  delay(20);
 

}

void loop() {
  if (!client.connected()) {
    Serial.println("MQTT Client disconnected. retry connection...");
    reconnect();
  }
  
  client.loop();
}