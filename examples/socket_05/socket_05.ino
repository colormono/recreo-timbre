/*
   Sockets round 2
   - Conecta con servidor NODE con Socket.io
   - Escucha mensajes emitidos en JSON
   - Analizar pattern enviado desde dashboard con:
   --- Led RGB (color)
   --- Relay (on/off)
   --- Display (string/timer)

   Help: https://github.com/Links2004/arduinoWebSockets
*/
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>
#include <SevenSegmentFun.h>

// Wifi
const char* wifi_ssid = "quale-privada";
const char* wifi_password = "quale2010pri";

// Server
const char* io_server = "192.168.1.108";
uint64_t io_server_port = 4001;

// WebSockets
WebSocketsClient webSocket;
#define MESSAGE_INTERVAL 30000
#define HEARTBEAT_INTERVAL 25000
uint64_t messageTimestamp = 0;
uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

// Outputs
#define LED_RED     15 //PWM D8
#define LED_GREEN   12 //PWM D6
#define LED_BLUE    13 //D7
const int PIN_RELAY = 14; // D5
const byte PIN_DIO = 5; // D1
const byte PIN_CLK = 4; // PWM D2

// Display
SevenSegmentFun display(PIN_CLK, PIN_DIO);

// First time
void setup() {
  // Initialize Serial port
  Serial.begin(115200);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Esto que és?
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  // LED
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, 0);
  digitalWrite(LED_GREEN, 0);
  digitalWrite(LED_BLUE, 0);
  analogWriteRange(255);

  // Relay
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);
  delay(1000);
  digitalWrite(PIN_RELAY, HIGH);
  delay(1000);
  digitalWrite(PIN_RELAY, LOW);

  // Display (Brillo al 10%, usar 100% en producción)
  display.begin();
  display.setBacklight(10);
  display.print("LOAD");

  // Connect to WebSocket server
  //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
  webSocket.beginSocketIO(io_server, io_server_port);
  webSocket.onEvent(webSocketEvent);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {

    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      isConnected = false;
      break;

    case WStype_CONNECTED:
      {
        Serial.printf("[WSc] Connected to url: %s\n",  payload);
        isConnected = true;
        // send message to server when Connected
        // socket.io upgrade confirmation message (required)
        webSocket.sendTXT("5");
      }
      break;

    case WStype_TEXT:
      {
        Serial.printf("Payload: %s\n", payload);
        String text = ((const char *)&payload[0]);
        int pos = text.indexOf('{');
        String msg = text.substring(pos, text.length() - 1);

        // Llave a controlar
        int keyPosInit = text.indexOf('"') + 1;
        int keyPosEnd = text.indexOf('"', keyPosInit);
        String key = text.substring(keyPosInit, keyPosEnd);
        Serial.println("Key: " + key);

        // Analizar payload según clave
        /*
          switch (key) {
          case "led":
            //Hacer algo sólo para el led
            break;
          default:
            break;
          }
        */
        parseMsg(msg);

        // send message to server
        // webSocket.sendTXT("message here");
      }
      break;

    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
  }

}

void parseMsg(String msg) {
  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(7) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonBuffer jsonBuffer(capacity);
  JsonObject& root = jsonBuffer.parseObject(msg);
  if (!root.success()) {
    Serial.println(F("Parsing failed!"));
    return;
  }

  // Fetch values.

  // Estado
  const char* estado = root["status"];
  Serial.print("estado: ");
  Serial.println(estado);
  if ( strcmp(estado, "sonando") == 0 ) {
    ringOn();
  } else {
    ringOff();
  }

  // Energía
  long energy = root["energy"];
  Serial.print("energy: ");
  Serial.println(energy);

  // Tempo
  long tempo = root["tempo"];
  Serial.print("tempo: ");
  Serial.println(tempo);

  // Palabra Clave
  String palabraClave = root["palabraClave"];
  Serial.print("palabraClave: ");
  Serial.println(palabraClave);
  display.print(palabraClave);

  // Paleta
  int r = root["palette"][0]["r"];
  int g = root["palette"][0]["g"];
  int b = root["palette"][0]["b"];
  analogWrite(LED_RED,    r);
  analogWrite(LED_GREEN,  g);
  analogWrite(LED_BLUE,   b);
  Serial.print("r: ");
  Serial.println(r);
  Serial.print("g: ");
  Serial.println(g);
  Serial.print("b: ");
  Serial.println(b);
}

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

void loop() {
  webSocket.loop();

  if (isConnected) {

    uint64_t now = millis();

    if (now - messageTimestamp > MESSAGE_INTERVAL) {
      messageTimestamp = now;
      // example socket.io message with type "messageType" and JSON payload
      webSocket.sendTXT("42[\"messageType\",{\"greeting\":\"hello\"}]");
    }
    if ((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
      heartbeatTimestamp = now;
      // socket.io heartbeat message
      webSocket.sendTXT("2[\"heartBeat\",{\"heartBeat\":\"1\"}]");
    }
  }
}
