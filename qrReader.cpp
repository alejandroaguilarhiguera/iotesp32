
#include "Arduino.h"
#include "httpRequest.h"

#define QR Serial1

void setup_qr() {
  // Inicializa el lector QR en los pines 26 (RX) y 27 (TX)
  QR.begin(9600, SERIAL_8N1, 26, 27);
}

void read_qr() {
  if (QR.available()) {
    // Lee el código QR completo hasta el carácter de retorno de carro
    String qrCode = QR.readStringUntil('\r');
    qrCode.trim(); // Limpia espacios o caracteres invisibles

    if (qrCode.length() > 0) {
      Serial.print("Código QR Leído: ");
      Serial.println(qrCode);
      
      // Llama a la función para enviar la petición GET
      sendValidationCode(qrCode);
    }
  }
}
