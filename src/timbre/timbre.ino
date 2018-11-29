/**
   Timbre v0.92
   by COLORMONO

   - Intenta conectarse a la red, sino crea AP
   - Conecta con servidor NODE con Socket.io
   - Escucha y analiza mensajes emitidos en JSON
   - Genera un webserver para testing y control remoto
*/

#include "FS.h"
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <string.h>
#include  <Chrono.h>

#include <WebSocketsClient.h>
#include <Hash.h>

#include <ArduinoJson.h>
#include <SPI.h>

#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>
#include <SevenSegmentFun.h>

// --- CONFIG ---

String io_server;
uint64_t io_server_port = 4001;
int object_mode;

// Config: Tiempo
Chrono chronoRecreo;
Chrono chronoDisplay;
Chrono chronoRelay;
Chrono chronoLed;


// Config: Server
ESP8266WebServer server(80);
void setupServer();
bool loadConfig();
bool saveConfig();
void handleRoot();
void handleSave();
void handleDemo();


// --- INPUTS ---

// WebSockets
WebSocketsClient webSocket;
#define MESSAGE_INTERVAL 30000
#define HEARTBEAT_INTERVAL 25000
uint64_t messageTimestamp = 0;
uint64_t heartbeatTimestamp = 0;
bool isConnected = false;
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
void parseMsg(String msg);


// --- OUTPUTS ---

// Output: Led
#define LED_RED     15 // PWM D8
#define LED_GREEN   12 // PWM D6
#define LED_BLUE    13 // D7
void setColor(int r, int g, int b);
void setLedError();
void setLedSuccess();
void setLedWarning();

// Output: Relay
#define PIN_RELAY   14 // D5
void ringOn();
void ringOff();
void ring(long tiempo);

// Output: Display
#define PIN_DIO     5  // D1
#define PIN_CLK     4  // PWM D2
int displayBright = 100; // Brillo al 10%, usar 100% en producción
SevenSegmentFun display(PIN_CLK, PIN_DIO);
void showLocalIp();
void setDisplayText(char* str);
void setDisplayLoading();
void setDisplayPlay();
void setDisplayCode();
void setDisplayCode(byte hours, byte min, unsigned int speed, String text);

// Objeto
void startModeRecreo();
void startModeAutonomo();
void startModePomodoro();
void startFocus(int timer);
void startBreak(int timer);
void startRecreo();
void startRecreo(int timePlay, int timeWork);


// --- SETUP ---

void setup() {
  Serial.begin(115200);

  // LED
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  analogWriteRange(255);

  // Relay
  pinMode(PIN_RELAY, OUTPUT);
  ringOff();

  // Display
  display.begin();
  display.setBacklight(displayBright);
  setDisplayText("LOAD");

  // WIFI Connect
  //wifiManager.resetSettings();
  WiFiManager wifiManager;

  // Intentar conectarse con la data del EEPROM
  // sino inicializar como AP
  wifiManager.autoConnect("TimbreAP");
  Serial.println("conectado... :)");

  // Load config
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    setLedError();
    return;
  }
  if (!loadConfig()) {
    Serial.println("Failed to load config");
  } else {
    Serial.println("Config loaded");
    setLedSuccess();
  }

  // WEBSERVER
  setupServer();
  showLocalIp();

  // SWITCH MODE
  switch (object_mode) {
    case 1:
      startModeRecreo();
      break;
    case 2:
      startModeAutonomo();
      break;
    case 3:
      startModePomodoro();
      break;
  }
}


// --- LOOP ---

void loop() {

  // Recreo WS
  if (object_mode == 1) {
    webSocket.loop();

    // Si está conectado
    if (isConnected) {
      uint64_t now = millis();

      // Enviar saludo
      if (now - messageTimestamp > MESSAGE_INTERVAL) {
        messageTimestamp = now;
        // example socket.io message with type "messageType" and JSON payload
        webSocket.sendTXT("42[\"messageType\",{\"greeting\":\"hello\"}]");
      }

      // Enviar muestra de vida
      if ((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
        heartbeatTimestamp = now;
        // socket.io heartbeat message
        webSocket.sendTXT("2[\"heartBeat\",{\"heartBeat\":\"1\"}]");
      }
    }
  }

  // Autonomo
  else if (object_mode == 2) {
    startRecreo();
  }

  // Pomodoro
  else if (object_mode == 3) {
    // control from app
  }

  // Server
  server.handleClient();
}

