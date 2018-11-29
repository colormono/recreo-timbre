/**
    METODOS PARA EL LED
*/

void setColor(int r, int g, int b) {
  analogWrite(LED_RED, r);
  analogWrite(LED_GREEN, g);
  analogWrite(LED_BLUE, b);
}

void setLedError() {
  setColor(255, 0, 0);
}

void setLedSuccess() {
  setColor(0, 255, 0);
}

void setLedWarning() {
  setColor(0, 0, 255);
}
