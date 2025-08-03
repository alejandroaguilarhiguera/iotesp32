#include "Arduino.h"
#include "secrets.h"
#include <WiFi.h>
#include <HTTPClient.h>


void sendValidationCode(String code) {
  // Solo intenta la petición si el Wi-Fi está conectado
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    const String webHook = HTTP_VALIDATION_CODE;
    // Construye la URL final
    String url = webHook + "?code=" + code;
    
    Serial.print("Enviando petición a: ");
    Serial.println(url);

    // Inicia la petición
    http.begin(url);
    
    // Envía la petición GET
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("Petición enviada, código de respuesta: %d\n", httpCode);

      // Opcional: imprimir la respuesta del servidor
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Respuesta del servidor:");
        Serial.println(payload);
      }
    } else {
      Serial.printf("Fallo en la petición, error: %s\n", http.errorToString(httpCode).c_str());
    }

    // Libera los recursos
    http.end();
  } else {
    Serial.println("Error: Wi-Fi no conectado.");
  }
}
