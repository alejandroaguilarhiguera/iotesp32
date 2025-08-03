
// En el archivo: mqtt.h

#ifndef MQTT_H
#define MQTT_H

void mqtt_if_connected();
void setup_mqtt();
void mqtt_loop();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();


#endif
