/**
   Timbre v0.92
   by COLORMONO

   - Intenta conectarse a la red, sino crea AP
   - Conecta con servidor NODE con Socket.io
   - Escucha y analiza mensajes emitidos en JSON
   - Genera un webserver para testing y control remoto
*/

#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include "libraries/ArduinoJson.h"

#include <string.h>
#include <Chrono.h>

#include <WebSocketsClient.h>
#include <Hash.h>

#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>
#include <SevenSegmentFun.h>

// define your default values here, if there are different values in config.json, they are overwritten.
// length should be max size + 1
char reglas_server[40] = "192.168.1.108";
char reglas_port[6] = "4001";
char timbre_name[33] = "timbre";
//default custom static IP
char static_ip[16] = "192.168.1.250";
char static_gw[16] = "192.168.1.1";
char static_sn[16] = "255.255.255.0";

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// --- CONFIG ---

//String io_server;
//uint64_t io_server_port = 4001;
int object_mode = 1;

// Config: Tiempo
Chrono chronoRecreo;
Chrono chronoDisplay;
Chrono chronoRelay;
Chrono chronoLed;

// Config: Server
const char* htmlfile = "/index.html";
ESP8266WebServer server(80);
void setupServer();
bool loadConfig();
bool saveConfig();
void handleRoot();
void handleReset();
void handleSave();
void handleDemo();
void handleTesting();
void handlePomodoro();
void handleWebRequests();
bool loadFromSpiffs(String path);
void testRelay();
void testLed();
void testDisplay();

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
int displayBright = 100;
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


// --- SETUP ---

void setup() {
  Serial.begin(115200);
  Serial.println();

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
  setDisplayText("CONNECTING TO WIFI");

  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  //Initialize File System
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(reglas_server, json["reglas_server"]);
          strcpy(reglas_port, json["reglas_port"]);
          strcpy(timbre_name, json["timbre_name"]);

          if (json["ip"]) {
            Serial.println("setting custom ip from config");
            //static_ip = json["ip"];
            strcpy(static_ip, json["ip"]);
            strcpy(static_gw, json["gateway"]);
            strcpy(static_sn, json["subnet"]);
            //strcat(static_ip, json["ip"]);
            //static_gw = json["gateway"];
            //static_sn = json["subnet"];
            Serial.println(static_ip);
            /*            Serial.println("converting ip");
                        IPAddress ip = ipFromCharArray(static_ip);
                        Serial.println(ip);*/
          } else {
            Serial.println("no custom ip in config");
          }
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
    setLedError();
  }
  //end read
  Serial.println("File System Initialized");
  Serial.println("Config loaded");
  setLedSuccess();

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_reglas_server("server", "reglas server", reglas_server, 40);
  WiFiManagerParameter custom_reglas_port("port", "reglas port", reglas_port, 5);
  WiFiManagerParameter custom_timbre_name("name", "timbre name", timbre_name, 34);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //set static ip
  IPAddress _ip, _gw, _sn;
  _ip.fromString(static_ip);
  _gw.fromString(static_gw);
  _sn.fromString(static_sn);

  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);

  //add all your parameters here
  wifiManager.addParameter(&custom_reglas_server);
  wifiManager.addParameter(&custom_reglas_port);
  wifiManager.addParameter(&custom_timbre_name);

  //reset settings - for testing
  //wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  wifiManager.setMinimumSignalQuality();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("TimbreAP", "1234")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
  strcpy(reglas_server, custom_reglas_server.getValue());
  strcpy(reglas_port, custom_reglas_port.getValue());
  strcpy(timbre_name, custom_timbre_name.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["reglas_server"] = reglas_server;
    json["reglas_port"] = reglas_port;
    json["timbre_name"] = timbre_name;
    json["object_mode"] = "1";

    json["ip"] = WiFi.localIP().toString();
    json["gateway"] = WiFi.gatewayIP().toString();
    json["subnet"] = WiFi.subnetMask().toString();

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.prettyPrintTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  setDisplayText("OK");
  Serial.println("local ip:");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.subnetMask());
  Serial.print("static_ip: ");
  Serial.println(static_ip);
  Serial.print("timbre_name: ");
  Serial.println(timbre_name);
  Serial.print("reglas_server: ");
  Serial.println(reglas_server);

  // WEBSERVER
  setupServer();
  showLocalIp();

  // START MODE
  if (object_mode == 1) {
    startModeRecreo();
  } else if (object_mode == 2) {
    startModeAutonomo();
  } else {
    startModePomodoro();
  }
}

// --- LOOP ---

void loop() {
  if (object_mode == 1) {
    webSocket.loop();
    if (isConnected) {
      uint64_t now = millis();
      // Enviar muestra de vida
      if ((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
        heartbeatTimestamp = now;
        webSocket.sendTXT("2");
        Serial.println("sending heartBeat");
      }
    }
  } else if (object_mode == 2) {
    Serial.println("Update autonomo mode");
    //updateModeAutonomo();
  } else {
    Serial.println("Update pomodoro mode");
    //updateModePomodoro();
  }

  // Server
  server.handleClient();
}

