/**
    METODOS PARA EL DISPLAY
*/

void showLocalIp() {
  int repeat = 3;
  int counter = 0;
  chronoDisplay.restart();

  while (counter != repeat) {
    yield();
    if (chronoDisplay.hasPassed(3000)) {
      chronoDisplay.restart();
      display.print(WiFi.localIP().toString().c_str());
      counter++;
    }
  }
  
  display.clear();
}

void setDisplayLoading() {
  byte repeats = 10;
  display.snake(repeats);
}

void setDisplayText(char* str) {
  display.print(str);
  delay(500);
}

void setDisplayPlay() {
  unsigned int numMoves = 100;
  unsigned int timeDelay = 100;
  display.bouchingBall(numMoves, timeDelay);
}

void setDisplayCode() {
  byte hours = 5;
  byte minutes = 16;
  unsigned int speed = 10000;
  display.bombTimer(hours, minutes, speed, " PLAY");
}

void setDisplayCode(byte hours, byte min, unsigned int speed, String text) {
  char charBuf[15];
  text.toCharArray(charBuf, 15);
  display.bombTimer(hours, min, speed, charBuf);
}

