/**
    METODOS PARA EL RELAY
*/

// Prender alarma
void ringOn() {
  Serial.println("Prender relay");
  digitalWrite(PIN_RELAY, HIGH);
}

// Apagar alarma
void ringOff() {
  Serial.println("Apagar relay");
  digitalWrite(PIN_RELAY, LOW);
}

// Sonar por un tiempo
void ring(long tiempo) {
  Serial.print("Ring for: ");
  Serial.println(tiempo);
  ringOn();
  delay(tiempo);
  ringOff();
}


