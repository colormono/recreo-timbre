/**
   METODOS PARA EL OBJETO
*/

void startModeRecreo() {
  setDisplayText("RECREO");
  // Connect to WebSocket server
  webSocket.beginSocketIO(io_server, io_server_port);
  webSocket.onEvent(webSocketEvent);
  delay(2000);
  display.clear();
}

void startModePomodoro() {
  setDisplayText("POMODORO");
  delay(2000);
  display.clear();
}

void startModeAutonomo() {
  setDisplayText("AUTO");
  delay(2000);
  display.clear();
}

// RECREO RANDOM
void startRecreo() {
  int timePlay = random(10, 100);
  int timeWork = random(1, 3);
  startFocus(timeWork);
  startBreak(timePlay);
}

// RECREO DEMO
void startRecreo(int timePlay, int timeWork) {
  startFocus(timeWork);
  startBreak(timePlay);
}

// CODE
void startFocus(int timer) {
  setLedWarning();
  setDisplayLoading();
  setDisplayText("CODE");
  ring(1000);
  setLedSuccess();
  setDisplayCode(timer, 0, 60, " END");
  ring(1000);
  display.clear();
  setLedWarning();
}

// PLAY
void startBreak(int timer) {
  setLedWarning();
  setDisplayLoading();
  setDisplayText("PLAY");
  ring(1000);
  setLedSuccess();
  unsigned int numMoves = timer;
  unsigned int timeDelay = 100;
  display.bouchingBall(numMoves, timeDelay);
  ring(1000);
  display.clear();
  setLedWarning();
}

