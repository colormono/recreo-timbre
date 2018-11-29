/*
   Sockets round 2
   - Conecta con servidor NODE con Socket.io
   - Escucha mensajes emitidos en JSON
   - Analiza los mensajes
   - Controla color del led desde HTML

   Help: https://github.com/Links2004/arduinoWebSockets
*/
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#include <ArduinoJson.h>

// Wifi
const char* wifi_ssid = "quale-privada";
const char* wifi_password = "quale2010pri";

// Server
const char* io_server = "192.168.1.107";
uint64_t io_server_port = 3000;

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
        Serial.printf("[WSc] get text: %s\n", payload);
        String text = ((const char *)&payload[0]);
        int pos = text.indexOf('{');
        String msg = text.substring(pos, text.length() - 1);
        parseMsg(msg);

        // Esto todavía no aplica        
        // Descifrar llave a controlar
        int keyPosInit = text.indexOf('"') + 1;
        int keyPosEnd = text.indexOf('"', keyPosInit);
        String key = text.substring(keyPosInit, keyPosEnd);
        Serial.println("Key: " + key);
        //switch
        if (key == "led") {
  
        } else {

        }

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
  const char* palabra = root["word"];
  long ringFor = root["ringFor"];
  long playFor = root["playFor"];
  long workFor = root["workFor"];
  int r =00 root["r"];
  int g = root["g"];
  int b = root["b"];
  analogWrite(LED_RED,    r);
  analogWrite(LED_GREEN,  g);
  analogWrite(LED_BLUE,   b);
  Serial.print("r: ");
  Serial.println(r);
  Serial.print("g: ");
  Serial.println(g);
  Serial.print("b: ");
  Serial.println(b);

  // Print values.
  Serial.print("palabra: ");
  Serial.println(palabra);
  Serial.print("ringFor: ");
  Serial.println(ringFor);
  Serial.print("playFor: ");
  Serial.println(playFor);
  Serial.print("workFor: ");
  Serial.println(workFor);
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
