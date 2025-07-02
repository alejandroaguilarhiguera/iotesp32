#include <ArduinoJson.h>
#include "SPIFFS.h"
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* mqtt_server = "a3rxr0m5l4pnta-ats.iot.us-east-1.amazonaws.com";
const int mqtt_port = 8883;
const char* ssid = "Bf";
const char* password = "12345678";

String Read_rootca = R"(-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----)";
String Read_cert = R"(-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUeEvxWNHPji7or0qGdXYmRg8CNJswDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDYxNjAyMjgw
NFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAPEw/7GHBbdXJ/HhWHUY
vS1OBaTbVYQRkVC88S277Y2QnKZVflDD8spKxX9kmXuv67VZxHwIwdKLhrtbpzum
xEAy5QyLRUAElBgBAMWmCLabNedYiiV0j8QLyWghOy7p0oy1xyINp6OrP69Hh2iz
5yHyTFxuLrVK/TGZcjACSf8+v5l451qaP6v+/sT06h4p6h2n1daZrAPWx5NeNFLe
sB1Yo1NOF2KylVTgSvrNyLAgoX6//ymkX0AppapGDWqQ/YzrU1rehxJmlmbUDBJN
TldcMSR48SQtmU2dveDWiNp9f0+/kcb5dQix77UetbIB9FPeF8bZc8NLPlAr2nR+
udcCAwEAAaNgMF4wHwYDVR0jBBgwFoAUkmUBkQ1qi3+46LS/dDrf6OhE/qowHQYD
VR0OBBYEFHiFCmgrhryfknHYAfqNs1DGRHaQMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAB1FpbHnkSOW2spM800QZTlR3A
U+qqm65LT0RIQzZoTJrfwZDBkFUzxPLC9z72NfcpIy7LhdV4diHtioWYRge1piUe
yzCSLhW6YvOXOeSZcDgLr3UOzxndfEhggO7yOgUBCuqi6QD40jIsdL5D6bpZaqQD
8FlmsmqtxaYtLJFkkNIhck3dOABlzVyAGJClshERujghKBNsRL1aH4PLRhEdCx17
waA9+4hbSMKSrQuc0NkzC+dlGdlGqPkST15BteGTpEc+iHJ4Cc8MsEFcm33m1Obj
VFVsg5f/0Cg387RdfstMeYjGULhD9G38BhTg9zF/slRIxLxRsl5g8zqAcrr0
-----END CERTIFICATE-----)";
String Read_privatekey = R"(-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA8TD/sYcFt1cn8eFYdRi9LU4FpNtVhBGRULzxLbvtjZCcplV+
UMPyykrFf2SZe6/rtVnEfAjB0ouGu1unO6bEQDLlDItFQASUGAEAxaYItps151iK
JXSPxAvJaCE7LunSjLXHIg2no6s/r0eHaLPnIfJMXG4utUr9MZlyMAJJ/z6/mXjn
Wpo/q/7+xPTqHinqHafV1pmsA9bHk140Ut6wHVijU04XYrKVVOBK+s3IsCChfr//
KaRfQCmlqkYNapD9jOtTWt6HEmaWZtQMEk1OV1wxJHjxJC2ZTZ294NaI2n1/T7+R
xvl1CLHvtR61sgH0U94Xxtlzw0s+UCvadH651wIDAQABAoIBADpLfYGLAPavOhhc
PuUOdardkL+nw6OIG3dIM4K6SVBOA46wWzdqJOR/7fw8dFxQyMesSrEv7KjW0Fm4
bVMdm5Fou7OsdbBjaufiVi1vU+vO19hyrfEfmx/nOWbRoVxVZ25bASZZ5orIeTwE
LdZApTyvM4Wjm+Z1mXzDebXg18G5ELO4xeNK4lk/PG/rvPUfLRomiz99qlJWNHUV
+RwJ5LeIuAnICka4lz8xz3aNhqHfzyRcv9QFtDNun1ljJzkYyrHvPvWs3YQ/RedO
/8bqfAYitW/k/sUlX1Kkf/yxijHjsipWMRyJpFxFo1PLoeHHO8ONhVfEPc5K0xID
8XYjBwECgYEA/LTevMz83ZbMpMdRkXN7H5l6IBrNEoHLjwXDHOLMGD28OtqRYjNU
qfG83Ocm2cmUrEk4fwh6QkvBbPg5UjKFOKbnWjSSWhXF684MtKS7BjNW6yqzZLSF
LWE8OS/I8+vmkrkr5Yat+EoBbPhuYz/8kR/FSV0FP3n/S36TbiffxbUCgYEA9FW1
rfp3idaX4kd2JO+HczjfAzY5kHUbJLfYZ9NSMXNOEEDhnrVEnGSLJQU4Tc+v4oYE
EjfUpZlW2exzHxf7KIoOj5EpNw2/A/YMhyu5ihHwqHRubT3xRUO4nofpyRxksWdh
EEeevMdR3+8hO3kdN5Y1n8hRAtr/5vmQF5QtONsCgYEAxfqtxhySvse17CJ7tQhU
HZ2QIDbfEvC9GX/aLHdCjntPTIyP+e9dFQdef3JY7acP4yo6KMvrfKT9FPIjdlI+
LPQVrIYjYpBMT0FOkve7zk/KeNMrWGpVLAhRtjvJo8/dBHYt/dRV13+jPkwjr34h
0VCir6dAxUo+yFaSCmsS1UECgYEA9AdyiAvjPiC823wCDIvLHZJMclnDDntXm8me
crwFmGwQKH2Umb1zhDuLGXNoM5bdlQHCqStnz+khbFAS/pgl+exA9LTHlDu/bsZc
5tgN3E9kq+p1q9e01Df5+SPHpFweWWzl7auKaAMuy9h6uXqSasU0U5xvBMoT0e2p
M1OEAwcCgYB9KRywhJ+qXs0NAt5o06qxlw7HJKtresdPm3rsQ1K+W1AfVpqMFtfB
jRaQFlXFvdiPDiifI0zkOfPRN0kVcTaNWNSDKAPnhVVOOHFYRHXw3pjl4S3+uqM5
+fHaf/NeGXiyj+bJZZIS4LN10O+3p0c8uu5UgggtbmcYR6K14hWjvA==
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